#include "LedController.h"

LedController::LedController(int updateSpeed, const int dataPin, int amountLeds)
{
    this->updateSpeed = updateSpeed;
    this->amountLeds = amountLeds;
    
    leds = new CRGB[amountLeds];    

    FastLED.addLeds<WS2812B, 0, COLOR_ORDER>(leds, amountLeds);
    FastLED.setCorrection(TypicalPixelString);
    FastLED.setBrightness(255);
}

LedController::~LedController()
{
    delete[] leds;
}

void LedController::displayWeatherOnce(WeatherLedInfo* weatherLedInfo)
{   
    Serial.println("displayWeatherOnce called!");
    Serial.println(String("TempChange: ") + String(weatherLedInfo->temperatureChange));
    Serial.println(String("Weather: ") + String(weatherLedInfo->weatherType));

    switch(weatherLedInfo->temperatureChange)
    {
        case MuchColder:
            enqueueShow(ShowTempMuchColder);
            break;
        case MuchWarmer:
            enqueueShow(ShowTempMuchWarmer);
            break;
        case LittleColder:
            enqueueShow(ShowTempLittleColder);
            break;
        case LittleWarmer:
            enqueueShow(ShowTempLittleWarmer);
            break;
    }

    switch(weatherLedInfo->weatherType)
    {
        case ClearSky:
            enqueueShow(ShowWeatherClearSky);
            break;
        case FewClouds:
            enqueueShow(ShowWeatherFewClouds);
            break;
        case Cloudy:
            enqueueShow(ShowWeatherCloudy);
            break;
        case ShowerRain:
            enqueueShow(ShowWeatherShowerRain);
            break;
        case Rain:
            enqueueShow(ShowWeatherRain);
            break;
        case Thunderstorm:
            enqueueShow(ShowWeatherThunderstorm);
            break;
        case Snow:
            enqueueShow(ShowWeatherSnow);
            break;
        case Fog:
            enqueueShow(ShowWeatherFog);
            break;
    }
}

void LedController::displayErrorContinuous()
{
    Serial.println("displayError called!");
    currentShow = ShowError;
    nextShow = ShowNothing;
    afterNextShow = ShowNothing;
}

void LedController::displayNothing()
{
    enqueueShow(ShowNothing);
}

void LedController::handleLeds()
{
    unsigned long currentTime = millis();

    if (currentTime > lastUpdated + updateSpeed || currentTime < lastUpdated)
    {
        lastUpdated = currentTime;

        if (currentShow == ShowNothing)
        {
            showCounter = 0;
            currentShow = nextShow;
            nextShow = afterNextShow;
            afterNextShow = ShowNothing;
        }

        if (currentShow != ShowNothing)
        {
            handleShow();
            showCounter++;
        }
    }
}

void LedController::enqueueShow(LedMode next)
{
    if (nextShow != ShowNothing && afterNextShow != ShowNothing)
    {
        nextShow = next;
        afterNextShow = ShowNothing;
    }
    else if (nextShow != ShowNothing)
    {
        afterNextShow = next;
    }
    else
    {
        nextShow = next;
    }
}


void LedController::handleShow()
{   
    switch (currentShow)
    {
        // Temperature shows
        case ShowTempMuchColder:
            handleShowTemp(0, 0, 255);
            break;
        case ShowTempLittleColder:
            handleShowTemp(40, 90, 255);
            break;
        case ShowTempMuchWarmer:
            handleShowTemp(255, 0, 0);
            break;
        case ShowTempLittleWarmer:
            handleShowTemp(255, 90, 65);
            break;
        // Weather shows
        case ShowWeatherRain:
            handleShowCirclingGradient(0, 0, 10, 0, 0, 150, true);
            break;
        case ShowWeatherShowerRain:
            handleShowCirclingGradient(255, 255, 255, 25, 25, 255, false);
            break;
        case ShowWeatherSnow:
            handleShowCirclingGradient(255, 255, 255, 0, 0, 0, true);
            break;
        case ShowWeatherCloudy:
            handleShowCirclingGradient(255, 255, 255, 100, 100, 100, false);
            break;
        case ShowWeatherFog:
            handleSingleColor(50, 50, 50);
            break;
        case ShowWeatherClearSky:
            handleShowCirclingGradient(255, 130, 10, 255, 190, 0, false);
            break;
        case ShowWeatherFewClouds:
            handleShowCirclingGradient(255, 255, 255, 255, 150, 20, false);
            break;
        case ShowWeatherThunderstorm:
            handleFlashing(255, 255, 255);
            break;
        case ShowError:
            handleShowError();
            break;
    }
}

void LedController::handleShowTemp(int r, int g, int b)
{
    if (showCounter >= 256)
    {
        resetLeds();
    }
    else 
    {        
        if (showCounter >= 0 && showCounter <= 127)
        {  
            if (showCounter == 0)
            {
                for (int i = 0; i < amountLeds; i++)
                {
                    leds[i].r = r;
                    leds[i].g = g;            
                    leds[i].b = b;
                }   
            }               
            
            FastLED.setBrightness(showCounter * 2);
        }
        else if (showCounter > 127 && showCounter < 255)
        {
            FastLED.setBrightness((255 - showCounter) * 2);
        }     

        FastLED.show();
    }
}

void LedController::handleShowCirclingGradient(int r1, int g1, int b1, int r2, int g2, int b2, bool fast) 
{
    if (showCounter >= 900)
    {
        resetLeds();
    }
    else
    {
        if (showCounter == 0)
        {
            for(int i = 0; i < amountLeds; i++)
            {
                leds[i].r = i % 2 == 0 ? r1 : r2;
                leds[i].g = i % 2 == 0 ? g1 : g2;
                leds[i].b = i % 2 == 0 ? b1 : b2;
            }
        }
        else if (showCounter >= 0 && showCounter < 900)
        {            
            float progress;
            if (fast)
                progress = (showCounter % CIRCLING_FAST) / float(CIRCLING_FAST);
            else
                progress = (showCounter % CIRCLING_SLOW) / float(CIRCLING_SLOW);

            for(int i = 0; i < amountLeds; i++)
            {               
                leds[i].r = calculateProgressOscillating(r1, r2, progress);
                leds[i].g = calculateProgressOscillating(g1, g2, progress);
                leds[i].b = calculateProgressOscillating(b1, b2, progress);
            }
        }

        handleFading(850, showCounter);
        FastLED.show();
    }
}

void LedController::handleSingleColor(int r, int g, int b)
{
    if (showCounter >= 900)
    {
        resetLeds();
    }
    else
    {
        if (showCounter == 0)
        {
            for(int i = 0; i < amountLeds; i++)
            {
                leds[i].r = r;
                leds[i].g = g;
                leds[i].b = b;
            }
        }
        
        handleFading(850, showCounter);
        FastLED.show();
    }
}

void LedController::handleFlashing(int r, int g, int b)
{
    if (showCounter >= 900)
    {
        resetLeds();
    }
    else
    {
        if((showCounter >= 50 && showCounter <= 55) ||
            (showCounter >= 100 && showCounter <= 110) ||
            (showCounter >= 115 && showCounter <= 120) ||
            (showCounter >= 300 && showCounter <= 305) ||
            (showCounter >= 350 && showCounter <= 360) ||
            (showCounter >= 370 && showCounter <= 375) ||
            (showCounter >= 390 && showCounter <= 395) ||
            (showCounter >= 410 && showCounter <= 415) ||
            (showCounter >= 435 && showCounter <= 445) ||
            (showCounter >= 600 && showCounter <= 605) ||
            (showCounter >= 610 && showCounter <= 615) ||
            (showCounter >= 630 && showCounter <= 640) ||
            (showCounter >= 650 && showCounter <= 655) ||
            (showCounter >= 660 && showCounter <= 665) ||
            (showCounter >= 690 && showCounter <= 695)
            )
        {
            for(int i = 0; i < amountLeds; i++)
            {
                leds[i].r = r;
                leds[i].g = g;
                leds[i].b = b;
            }
        }
        else   
        {
            for(int i = 0; i < amountLeds; i++)
            {
                leds[i].r = 0;
                leds[i].g = 0;
                leds[i].b = 0;
            }
        }              

        FastLED.show();
    }
}

int LedController::calculateProgress(int i1, int i2, float factor)
{
    return i1 + ((i2 - i1) * factor);
}

int LedController::calculateProgressOscillating(int i1, int i2, float progress)
{
    double progress2 = sin(progress * PI); 
    return calculateProgress(i1, i2, progress2);
}

void LedController::resetLeds()
{
    for (int i = 0; i < amountLeds; i++)
    {
        leds[i].r = 0;
        leds[i].g = 0;
        leds[i].b = 0;                           
    }   
    FastLED.setBrightness(255);

    currentShow = ShowNothing;
}

void LedController::handleFading(int showCountMax, int counter)
{
    if (counter <= 127)
    {
        FastLED.setBrightness(counter * 2);
    }
    else if(counter >= showCountMax - 127 && counter <= showCountMax)
    {
        FastLED.setBrightness((showCountMax - counter) * 2);
    }
}

void LedController::handleShowError()
{
    if (nextShow != ShowNothing)
    {
        resetLeds();
    }
    else
    {
        if (showCounter >= 900)
        {
            showCounter = 0;
        }
        else
        {
            if (showCounter == 0)
            {
                for(int i = 0; i < amountLeds; i++)
                {
                    leds[i].r = 255;
                    leds[i].g = 0;
                    leds[i].b = 0;
                }
            }
            
            handleFading(400, showCounter);
            FastLED.show();        
        }
    }
}

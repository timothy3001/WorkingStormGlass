#ifndef LedController_h_
#define LedController_h_

#include "WeatherLedInfo.h"
#include <Arduino.h>
#include <FastLED.h>

#define COLOR_ORDER GRB

#define CIRCLING_FAST 100
#define CIRCLING_SLOW 250

enum LedMode
{
    // Default
    ShowNothing,
    // Temperature
    ShowTempMuchWarmer,
    ShowTempMuchColder,
    ShowTempLittleWarmer,
    ShowTempLittleColder,
    // Weather
    ShowWeatherClearSky,
    ShowWeatherFewClouds,
    ShowWeatherCloudy,
    ShowWeatherShowerRain,
    ShowWeatherRain,
    ShowWeatherThunderstorm,
    ShowWeatherSnow,
    ShowWeatherFog,
    // Error
    ShowError
};

class LedController
{
    public:
        LedController(int updateSpeed, const int dataPin, int amountLeds);
        ~LedController();

        void displayWeatherOnce(WeatherLedInfo* weatherLedInfo);
        void displayErrorContinuous();
        void displayNothing();

        void handleLeds();        

    private:
        CRGB* leds = 0;
        int amountLeds = 0;

        LedMode currentShow = ShowNothing;
        LedMode nextShow = ShowNothing;
        LedMode afterNextShow = ShowNothing;

        void enqueueShow(LedMode next);

        int updateSpeed = 0;
        int showCounter = 0;
        unsigned long lastUpdated = 0;

        void handleShow();
        int calculateProgress(int i1, int i2, float factor);
        int calculateProgressOscillating(int min, int max, float progress);     
        void handleFading(int showCountMax, int counter);
        void resetLeds();
        // Functions to handle different states
        void handleShowTemp(int r, int g, int b);
        void handleShowCirclingGradient(int r1, int g1, int b1, int r2, int g2, int b2, bool fast);   
        void handleSingleColor(int r, int g, int b);
        void handleFlashing(int r, int g, int b);        
        void handleShowError();
};
 
#endif
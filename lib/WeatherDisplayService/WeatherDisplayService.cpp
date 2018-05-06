#include "WeatherDisplayService.h"


WeatherDisplayService::WeatherDisplayService(LedController* ledController, String locationString, int predictionTimeStage, int intervalWeatherUpdateSeconds, int intervalWeatherDisplaySeconds)
{
    this->ledController = ledController;    
    this->intervalWeatherDisplaySeconds = intervalWeatherDisplaySeconds;
    this->intervalWeatherUpdateSeconds = intervalWeatherUpdateSeconds;
    this->locationString = locationString;
    this->predictionTimeStage = predictionTimeStage;
}

WeatherDisplayService::~WeatherDisplayService()
{

}

void WeatherDisplayService::handleWeather()
{
    unsigned long currentTime = millis();

    if (currentTime > lastWeatherUpdate + intervalWeatherUpdateSeconds * 1000 || 
        currentTime < lastWeatherUpdate || 
        lastWeatherUpdate == 0)
    {
        lastWeatherUpdate = currentTime;

        NullableDouble* nullableDoubleCurrent = getCurrentTemperature();

        if (nullableDoubleCurrent)
        {
            double tempCurrent = nullableDoubleCurrent->value;
            delete nullableDoubleCurrent;

            Serial.println(String("Current temp: ") + tempCurrent);
            processWeatherData(tempCurrent);
            return;
        }

        ledController->displayErrorContinuous();
        delete currentWeatherInfo;
        currentWeatherInfo = 0;
    }    

    if (currentTime > lastWeatherDisplay + intervalWeatherDisplaySeconds * 1000 ||
        currentTime < lastWeatherDisplay ||
        lastWeatherDisplay == 0)
    {
        lastWeatherDisplay = currentTime;
        if (currentWeatherInfo != 0)
        {
            ledController->displayWeatherOnce(currentWeatherInfo);
        }
    }

}

NullableDouble* WeatherDisplayService::getCurrentTemperature()
{
    HTTPClient client;
    String url = String("http://dataservice.accuweather.com/currentconditions/v1/") +
        String(locationString) + 
        String("?apikey=") + String(APIKEY);
    client.begin(url);
    client.setTimeout(10000);
    
    int result = client.GET();
    Serial.println(result);
    if (result == HTTP_CODE_OK)
    {
        StaticJsonBuffer<1000> jsonBuffer;
        JsonArray& weatherObjList = jsonBuffer.parseArray(client.getString());

        if (weatherObjList.success() && weatherObjList.size() > 0)
        {           
            JsonObject& weatherObj = weatherObjList[0]; 

            if(weatherObj.containsKey("Temperature"))
            {            
                JsonObject& temperatureObj = weatherObj["Temperature"];
                if (temperatureObj.containsKey("Metric"))    
                {
                    JsonObject& metricTempObj = temperatureObj["Metric"];
                    if (metricTempObj.containsKey("Value"))
                    {
                        double temperature = metricTempObj["Value"];
                        NullableDouble* nullableDoubleResult = new NullableDouble();  
                        nullableDoubleResult->value = temperature; 

                        return nullableDoubleResult;                      
                    }
                }
            }
        }
    }
    Serial.println("Couldn't get current temp data!");
    return 0;        
}

void WeatherDisplayService::processWeatherData(double tempCurrent)
{
    HTTPClient client;
    String url = String("http://dataservice.accuweather.com/forecasts/v1/hourly/12hour/") +
        String(locationString) + 
        String("?apikey=") + String(APIKEY) +
        String("&metric=true");
    client.begin(url);
    client.setTimeout(10000);

    Serial.println(String("Free heap: ") + ESP.getFreeHeap());
    
    delay(1000);

    int result = client.GET();
    Serial.println(result);
    if (result == HTTP_CODE_OK)
    {
        Serial.println("Tryin to get weatherData...");
        DynamicJsonBuffer jsonBuffer(8192);

        JsonArray& weatherObjList = jsonBuffer.parseArray(client.getStream());

        if (weatherObjList.success() && predictionTimeStage < weatherObjList.size())
        {
            JsonObject& weatherObj = weatherObjList[predictionTimeStage];
            if (weatherObj.containsKey("WeatherIcon"))
            {
                int weatherIconId = weatherObj["WeatherIcon"];
                WeatherType weatherType = getWeatherTypeForIconId(weatherIconId);
                if (weatherObj.containsKey("Temperature"))
                {
                    JsonObject& tempObject = weatherObj["Temperature"];
                    if (tempObject.containsKey("Value"))
                    {
                        double tempForecast = tempObject["Value"];
                        double tempDiff = tempForecast - tempCurrent;

                        TemperatureChange tempChange;

                        if (tempDiff > THRESHOLD_TEMPDIFF || tempDiff < -THRESHOLD_TEMPDIFF)
                            tempChange = tempDiff < 0 ? MuchColder : MuchWarmer;
                        else
                            tempChange = tempDiff < 0 ? LittleColder : LittleWarmer;

                        delete currentWeatherInfo;

                        currentWeatherInfo = new WeatherLedInfo();
                        currentWeatherInfo->weatherType = weatherType;
                        currentWeatherInfo->temperatureChange = tempChange;
                        return;
                    }
                }                
            }            
        }
    }           
    Serial.println("Couldn't get new weather data!");
    ledController->displayErrorContinuous();
    delete currentWeatherInfo;
    currentWeatherInfo = 0;
}

WeatherType WeatherDisplayService::getWeatherTypeForIconId(int iconId)
{
    WeatherType weatherType = Unknown;
    switch (iconId)
    {
        case 1:
        case 2:
        case 3:
        case 33:
        case 34:
            weatherType = ClearSky;
            break;
        case 4:
        case 5:
        case 35:
        case 36:
            weatherType = FewClouds;
            break;                    
        case 6:
        case 7:
        case 8:
        case 37:
        case 38:
            weatherType = Cloudy;
            break;
        case 11:
            weatherType = Fog;
            break;
        case 12:
        case 13:
        case 14:
        case 39:
        case 40:
            weatherType = ShowerRain;
            break;
        case 15:
        case 16:
        case 17:
        case 41:
        case 42:
            weatherType = Thunderstorm;  
            break;
        case 18:
        case 26:
        case 29:
            weatherType = Rain;
            break;
        case 19:
        case 20:
        case 21:
        case 22:
        case 23:
        case 24:
        case 25:
        case 43:
        case 44:
            weatherType = Snow;
            break;
    }

    return weatherType;
}
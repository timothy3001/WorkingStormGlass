#ifndef WeatherDisplayService_h_
#define WeatherDisplayService_h_

#include "LedController.h"
#include "WeatherLedInfo.h"
#include <ESP8266HTTPClient.h>
#include <Arduino.h>
#include <ArduinoJson.h>

#define THRESHOLD_TEMPDIFF 5.0
// Get an api key here: https://developer.accuweather.com/packages (click "Free: Get started!")
#define APIKEY "!!!ENTER YOUR KEY HERE!!!"

class NullableDouble
{
    public:
        double value;
};

class WeatherDisplayService
{
    public:
        WeatherDisplayService(LedController* ledController, String locationString, int predictionTimeStage, int intervalWeatherUpdateSeconds, int intervalWeatherDisplaySeconds);
        ~WeatherDisplayService();

        void handleWeather();

    private:
        void processWeatherData(double tempCurrent);
        NullableDouble* getCurrentTemperature();
        WeatherType getWeatherTypeForIconId(int iconId);

        unsigned long lastWeatherUpdate = 0;
        unsigned long lastWeatherDisplay = 0;   

        LedController* ledController = 0;
        WeatherLedInfo* currentWeatherInfo = 0;

        int intervalWeatherUpdateSeconds;
        int intervalWeatherDisplaySeconds;
        int predictionTimeStage;
        String locationString;

        bool errorAppeared = false;    
};




#endif
#include <Arduino.h>
#include <ESP8266WiFi.h>
#include <WiFiClient.h>
#include <ESP8266WebServer.h>
#include <FastLED.h>
#include "Configuration.h"
#include "WebserverConfiguration.h"
#include "WeatherDisplayService.h"
#include "LedController.h"

#define ACCESS_POINT_NAME "StormGlass"

#define NUM_LEDS 4
#define DATA_PIN 0
#define UPDATE_SPEED_LEDS 10

extern "C" {
  #include "user_interface.h"
}

Configuration config;
LedController ledController(UPDATE_SPEED_LEDS, DATA_PIN, NUM_LEDS);
ESP8266WebServer server(80);
WeatherDisplayService* weatherDisplayService = 0;

void createAccessPoint();
void createConfigurationWebserver();
void setupWifi();
void logFreeHeap();

void setup() 
{
  wifi_station_set_hostname("StormGlass");

  Serial.begin(115200);

  Serial.println("Loading configuration...");
  config.load();
  Serial.println(String("Configuration loaded. AP: ") + String(config.wifiSsid) + 
    String(" Password: ") + String(config.wifiPassword) + 
    String(" Location: ") + String(config.locationKey));
  
  Serial.println("Setting up wifi...");
  setupWifi();
  Serial.println("Wifi set up!");

  Serial.println("Setting up web server...");
  new WebserverConfiguration(&server, &config);
  server.begin();
  Serial.println("Web server set up!");    

  if (WiFi.status() == WL_CONNECTED)
  {
    Serial.println("Setting up WeatherDisplayService...");
    weatherDisplayService = new WeatherDisplayService(&ledController, config.locationKey, 5, 3600, 60);
    Serial.println("WeatherDisplayService set up!");
  }
}

void loop() {
  server.handleClient();
  logFreeHeap();  
  if (weatherDisplayService != 0)
  {
    weatherDisplayService->handleWeather();
    ledController.handleLeds();
  }  

  delay(10);
}

void setupWifi()
{
  if (!(config.wifiSsid && config.wifiPassword))
  {
    Serial.println("WiFi information missing! Setting access point up...");

    WiFi.mode(WIFI_AP);
    WiFi.softAP(ACCESS_POINT_NAME, "");
  }
  else
  {
    WiFi.mode(WIFI_STA);
    WiFi.begin(config.wifiSsid, config.wifiPassword);

    unsigned long startTryingToConnect = millis();

    while(WiFi.status() != WL_CONNECTED)
    {
      if (millis() > (startTryingToConnect + 30000))
      {
        Serial.println("Could not connect to access point! Setting access point up...");
        WiFi.mode(WIFI_AP);
        WiFi.softAP(ACCESS_POINT_NAME, "");
        return;
      }
      delay(500);
      Serial.println("Waiting for connection to access point...");
    }
  }
}

long lastChecked = 0;
void logFreeHeap()
{
  if (millis() > lastChecked + 5000)
  {
    Serial.println(String("Free heap: ") + ESP.getFreeHeap() + String(" bytes. Timestamp: ") + String(millis() / 1000));
    lastChecked = millis();
  }  
}
#ifndef Configuration_h_
#define Configuration_h_

#include "EepromHandler.h"

class Configuration
{
  public:
    Configuration();
    ~Configuration();

    char* wifiSsid = 0;
    char* wifiPassword = 0;
    char* locationKey = 0;

    void save();
    void load();
  private:
    EepromHandler eepromHandler;

};

#endif
#include "Configuration.h"

Configuration::Configuration()
{

}

Configuration::~Configuration()
{
    delete wifiSsid;
    delete wifiPassword;
    delete locationKey;
}

void Configuration::save()
{
    eepromHandler.reset();

    if (wifiSsid && wifiPassword && locationKey)
    {
        char* temp = new char[strlen(wifiSsid) + 1];
        strcpy(temp, wifiSsid);
        eepromHandler.addValue(temp);

        temp = new char[strlen(wifiPassword) + 1];
        strcpy(temp, wifiPassword);
        eepromHandler.addValue(temp);

        temp = new char[strlen(locationKey) + 1];
        strcpy(temp, locationKey);
        eepromHandler.addValue(temp);

        eepromHandler.save();
    }
}

void Configuration::load()
{
    eepromHandler = EepromHandler();
    eepromHandler.load();

    char* temp = 0;

    if (eepromHandler.getValue(0))
    {
        wifiSsid = new char[strlen(eepromHandler.getValue(0)) + 1];
        strcpy(wifiSsid, eepromHandler.getValue(0));
    }

    if (eepromHandler.getValue(1))
    {
        wifiPassword = new char[strlen(eepromHandler.getValue(1)) + 1];
        strcpy(wifiPassword, eepromHandler.getValue(1));
    }

    if (eepromHandler.getValue(2))
    {
        locationKey = new char[strlen(eepromHandler.getValue(2)) + 1];
        strcpy(locationKey, eepromHandler.getValue(2));
    }
}
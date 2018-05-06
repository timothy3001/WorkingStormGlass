#ifndef EepromHandler_h_
#define EepromHandler_h_
#define DEBUG_EEPROMHANDLER

#include <EEPROM.h>
#include <string.h>
#include <Arduino.h>


class EepromHandler
{
  private:
    char** values = 0;
    int values_size = 0;

    void clearValues();

  public:
    EepromHandler();
    ~EepromHandler();
    void addValue(char* newValue);
    char* getValue(int index);
    void load();
    void save();
    void reset();
    int getCountFields();
};

#endif

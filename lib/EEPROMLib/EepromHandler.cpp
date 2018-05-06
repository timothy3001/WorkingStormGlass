#include "EepromHandler.h"

#define EEPROM_SIZE 2000

EepromHandler::EepromHandler()
{
  values_size = 0;
}

EepromHandler::~EepromHandler()
{
  delete[] values;
}

void EepromHandler::load()
{
  EEPROM.begin(EEPROM_SIZE);
  char* temp = new char[100];
  int counterTemp = 0;
  char c = 't';
  for (int i = 0; i < EEPROM_SIZE && c != 0; i++)
  {
    c = EEPROM.read(i);

    if (c == '|' || counterTemp == 99)
    {
      temp[counterTemp] = 0;
      char* singleEntry = new char[counterTemp + 1];
      strcpy(singleEntry, temp);

      this->addValue(singleEntry);

      delete temp;
      temp = new char[100];
      counterTemp = 0;
    }
    else
    {
      temp[counterTemp] = c;
      counterTemp++;
    }
  }
  EEPROM.end();
  
  delete temp;
}

void EepromHandler::addValue(char* newValue)
{
  if (!values)
  {
    values = new char*[1];
    values_size = 1;
  }
  else
  {
    char** tempValues = new char*[values_size + 1];

    for (int i = 0; i < values_size; i++)
    {
      tempValues[i] = values[i];
    }
     delete values;
     values = tempValues;
     values_size++;
  }

  int newPosition = values_size - 1;
  values[newPosition] = newValue;
}

char* EepromHandler::getValue(int index)
{
  if (values && index < values_size)
    return values[index];
  return 0;
}

int EepromHandler::getCountFields()
{
  return values_size;
}

void EepromHandler::reset()
{
  delete[] values;
  values = 0;
  values_size = 0;
}

void EepromHandler::save()
{
  EEPROM.begin(EEPROM_SIZE);
  
  if (values && values_size > 0)
  {
    int index = 0;
    for (int i = 0; i < values_size; i++)
    {
      char* currString = values[i];
      for (int j = 0; j < strlen(currString); j++)
      {
        EEPROM.write(index, currString[j]);
        index++;
        delay(10);
      }
      EEPROM.write(index, '|');
      index++;
      delay(10);
    }

    EEPROM.write(index, 0);
    EEPROM.commit();
    delay(100);
    EEPROM.end();
  }
}
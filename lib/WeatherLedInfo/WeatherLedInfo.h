#ifndef WeatherLedInfo_h_
#define WeatherLedInfo_h_

enum TemperatureChange
{
    MuchWarmer,
    LittleWarmer,
    MuchColder,
    LittleColder
};

enum WeatherType
{
    Unknown,
    ClearSky,
    FewClouds,
    Cloudy,
    ShowerRain,
    Rain,
    Thunderstorm,
    Snow,
    Fog
};

struct WeatherLedInfo
{
    WeatherType weatherType;
    TemperatureChange temperatureChange;
};

#endif
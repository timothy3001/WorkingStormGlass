#ifndef WebserverConfiguration_h_
#define WebserverConfiguration_h_

#include <ESP8266WebServer.h>
#include "Configuration.h"

class WebserverConfiguration
{
    public:
        WebserverConfiguration(ESP8266WebServer* ws, Configuration* config);
        ~WebserverConfiguration();
    private:
        ESP8266WebServer* webServer;
        Configuration* config;

        void handleRoot();
        void handleUpdateConfig();
        void handleGetStyle();
};

extern const char* html;
extern const char* bundledCss;

#endif
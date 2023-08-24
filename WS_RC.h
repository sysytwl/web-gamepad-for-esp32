#include <WiFi.h>
#include <ESPmDNS.h>
#include <WiFiUdp.h>
#include <ArduinoOTA.h>
#include <AsyncTCP.h>
#include <ESPAsyncWebSrv.h>
#include <iostream>
#include <sstream>
#include "ESP32MX1508.h"
#include "web.h"


AsyncWebServer server(80);
AsyncWebSocket wsCarInput("/CarInput");

MX1508 motorA(16, 17, 0, 1);
MX1508 motorB(18, 19, 2, 3);
int left;
int right;
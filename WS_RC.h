#include <WiFi.h>
//#include <ESPmDNS.h>
#include <WiFiUdp.h>
#include <ArduinoOTA.h>
#include <AsyncTCP.h>
#include <ESPAsyncWebSrv.h>
//#include <iostream>
//#include <sstream>
#include "ESP32MX1508.h"
#include "web.h"


AsyncWebServer server(80);
AsyncWebSocket wsCarInput("/CarInput");

MX1508 motorB(25, 26, 0, 1);
MX1508 motorA(18, 19, 2, 3);
int left;
int right;

// ===========================
// Enter your WiFi credentials
// ===========================
const char* host = "JackShenYt_Sumo";
const char* password = "ubrobots1";
const int channel = 3;

#include <WiFi.h>
#include <WiFiUdp.h>
#include <ArduinoOTA.h>
#include "ESP32MX1508.h"



MX1508 motorB(25, 26, 0, 1);
MX1508 motorA(18, 19, 2, 3);
int left;
int right;

// ===========================
// Enter your WiFi credentials
// ===========================
const char* host = "esp32_UBRobotics";
const char* password = "ubrobotics";
const int channel = 9;

WiFiUDP Udp; 
uint8_t data[8];
unsigned int localPort = 9999;

#include <Arduino.h>
#include "WS_RC.h"



void handleRoot(AsyncWebServerRequest *request) {
  request->send_P(200, "text/html", htmlHomePage);
}

void handleNotFound(AsyncWebServerRequest *request) {
  request->send(404, "text/plain", "File Not Found");
}

void onCarInputWebSocketEvent(AsyncWebSocket *server, AsyncWebSocketClient *client, AwsEventType type,void *arg, uint8_t *data, size_t len){
  switch (type) {
    case WS_EVT_CONNECT:
      Serial.printf("WebSocket client #%u connected from %s\n", client->id(), client->remoteIP().toString().c_str());
      motorA.motorBrake();
      motorB.motorBrake();
      break;
    case WS_EVT_DISCONNECT:
      Serial.printf("WebSocket client #%u disconnected\n", client->id());
      motorA.motorStop();
      motorB.motorStop();
      break;
    case WS_EVT_DATA:
      AwsFrameInfo *info;
      info = (AwsFrameInfo*)arg;
      if (info->final && info->index == 0 && info->len == len && info->opcode == WS_TEXT) {
        std::string myData = "";
        myData.assign((char *)data, len);
        std::istringstream ss(myData);
        std::string x, y;
        std::getline(ss, x, ',');
        std::getline(ss, y, ',');

        left = atoi(x.c_str()) + atoi(y.c_str());
        right = atoi(x.c_str()) - atoi(y.c_str());

        Serial.printf("L:{%d} R:{%d}\n", left, right); 

        motorA.motorGo(left);
        motorB.motorGo(right);

/*
      if((info->index + len) == info->len){
        Serial.printf("ws[%s][%u] frame[%u] end[%llu]\n", server->url(), client->id(), info->num, info->len);
        if(info->final){
          Serial.printf("ws[%s][%u] %s-message end\n", server->url(), client->id(), (info->message_opcode == WS_TEXT)?"text":"binary");
          if(info->message_opcode == WS_TEXT)
            client->text("I got your text message");
          else
            client->binary("I got your binary message");
*/
      } else {
        motorA.motorBrake();
        motorB.motorBrake();
      }
      break;
    case WS_EVT_PONG:
      Serial.printf("ws[%s][%u] pong[%u]: %s\n", server->url(), client->id(), len, (len)?(char*)data:"");
      break;
    case WS_EVT_ERROR:
      Serial.printf("ws[%s][%u] error(%u): %s\n", server->url(), client->id(), *((uint16_t*)arg), (char*)data);
      break;
    default:
      break;  
  }
}

void setup(void) {
  Serial.begin(115200);
  Serial.println("Booting");
  
  // AP name,passwd
  WiFi.softAP("UBRobotics", "UBRobotics");
  
  /*  Available ESP32 RF power parameters:
    WIFI_POWER_19_5dBm    // 19.5dBm (For 19.5dBm of output, highest. Supply current ~150mA)
    WIFI_POWER_19dBm      // 19dBm
    WIFI_POWER_18_5dBm    // 18.5dBm
    WIFI_POWER_17dBm      // 17dBm
    WIFI_POWER_15dBm      // 15dBm
    WIFI_POWER_13dBm      // 13dBm
    WIFI_POWER_11dBm      // 11dBm
    WIFI_POWER_8_5dBm     //  8dBm
    WIFI_POWER_7dBm       //  7dBm
    WIFI_POWER_5dBm       //  5dBm
    WIFI_POWER_2dBm       //  2dBm
    WIFI_POWER_MINUS_1dBm // -1dBm( For -1dBm of output, lowest. Supply current ~120mA) */
  WiFi.setTxPower(WIFI_POWER_MINUS_1dBm);
  
  IPAddress IP = WiFi.softAPIP();
  Serial.print("AP IP address: ");
  Serial.println(IP);

  // OTA
  ArduinoOTA
    .onStart([]() {
      String type;
      if (ArduinoOTA.getCommand() == U_FLASH)
        type = "sketch";
      else // U_SPIFFS
        type = "filesystem";

      // NOTE: if updating SPIFFS this would be the place to unmount SPIFFS using SPIFFS.end()
      Serial.println("Start updating " + type);
    })
    .onEnd([]() {
      Serial.println("\nEnd");
    })
    .onProgress([](unsigned int progress, unsigned int total) {
      Serial.printf("Progress: %u%%\r", (progress / (total / 100)));
    })
    .onError([](ota_error_t error) {
      Serial.printf("Error[%u]: ", error);
      if (error == OTA_AUTH_ERROR) Serial.println("Auth Failed");
      else if (error == OTA_BEGIN_ERROR) Serial.println("Begin Failed");
      else if (error == OTA_CONNECT_ERROR) Serial.println("Connect Failed");
      else if (error == OTA_RECEIVE_ERROR) Serial.println("Receive Failed");
      else if (error == OTA_END_ERROR) Serial.println("End Failed");
    });

  ArduinoOTA.begin();

  server.on("/", HTTP_GET, handleRoot);
  server.onNotFound(handleNotFound);
      
  wsCarInput.onEvent(onCarInputWebSocketEvent);
  server.addHandler(&wsCarInput);

  server.begin();
  Serial.println("HTTP server started");
}

void loop(){
  wsCarInput.cleanupClients();
  ArduinoOTA.handle();
}

#include <Arduino.h>
#include "WS_RC.h"



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
  
      if (info->final && info->index == 0 && info->opcode == WS_BINARY) {
        // Extract binary data into an array of integers
        int x = data[0];
        int y = data[1];
        int a = data[2];
        int b = data[3];
        int ch1 = data[4];
        int ch2 = 0; //data[5];
        int ch3 = 0; //data[6];
        int ch4 = 0; //data[7];

        Serial.printf("Received x: %d, y: %d, a: %d, b: %d, ch1: %d, ch2: %d, ch3: %d, ch4: %d\n", x, y, a, b, ch1, ch2, ch3, ch4);
      
        int left = y + x - 256;
        int right = y - x;
        Serial.printf("L:{%d} R:{%d}\n", left, right); 
        motorA.motorGo(left);
        motorB.motorGo(right);
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
  
  // AP name,passwd
  WiFi.softAP(host, password, channel);
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

  server.on("/", HTTP_GET, [](AsyncWebServerRequest *request) {
    request->send_P(200, "text/html", htmlHomePage);
  });
  server.on("/gamepad", HTTP_GET, [](AsyncWebServerRequest *request) {
    request->send_P(200, "text/html", gamepad);
  });
  server.on("/jquery.min.js", HTTP_GET, [](AsyncWebServerRequest *request) {
    request->send_P(200, "text/html", _js);
  });
  server.onNotFound([](AsyncWebServerRequest *request){
    request->send(404, "text/plain", "File Not Found");
  });
      
  wsCarInput.onEvent(onCarInputWebSocketEvent);
  server.addHandler(&wsCarInput);

  server.begin();
  Serial.println("HTTP server started");
}

void loop(){
  wsCarInput.cleanupClients();
  ArduinoOTA.handle();
}

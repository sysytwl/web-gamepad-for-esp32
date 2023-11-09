#include "Robot_UDP_Server.h"



void setup() {
  Serial.begin(115200);

  WiFi.softAP(ssid, password);
  Udp.begin(localPort);
  delay(200);
  Serial.println(WiFi.softAPIP());
}

void loop() {
  int packetSize = Udp.parsePacket();
  if (packetSize) {
    int len = Udp.read(data, 8);
    // Extract binary data into an array of integers
    int x = data[0];
    int y = data[1];
    int a = data[2];
    int b = data[3];
    int ch1 = data[4];
    int ch2 = 0; //data[5];
    int ch3 = 0; //data[6];
    int ch4 = 0; //data[7];

    Serial.printf("Received x: %d, y: %d, a: %d, b: %d, ch1: %d, ch2: %d, ch3: %d, ch4: %d", x, y, a, b, ch1, ch2, ch3, ch4);
      
    int left = y + x - 256;
    int right = y - x;
    Serial.printf("    L:{%d} R:{%d}\n", left, right); 
    motorA.motorGo(left);
    motorB.motorGo(right);
  } else {
    motorA.motorStop();
    motorB.motorStop();
  }
}

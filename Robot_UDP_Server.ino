#include "Robot_UDP_Server.h"

WiFiUDP Udp; 

char packetBuffer[255];

unsigned int localPort = 9999;

const char *ssid = "SmallReubenWagon";  
const char *password = "smallWagon";

class simpleMotor{
  public:
  int speed, PWMChannelF, PWMChannelB;

  void setup(int _pinForward, int _pinBackward, int _PWMChannelF, int _PWMChannelB){
    PWMChannelF = _PWMChannelF;
    PWMChannelB = _PWMChannelB;

    ledcSetup(_PWMChannelF, 60, 10);
    ledcSetup(_PWMChannelB, 60, 10);

    ledcAttachPin(_pinForward, _PWMChannelF);
    ledcAttachPin(_pinBackward, _PWMChannelB);
  }

  void setSpeed(int speed){
    if (speed < 0){
      speed*=-1;
      ledcWrite(PWMChannelF, 0);
      ledcWrite(PWMChannelB, speed);
    }
    else{
      ledcWrite(PWMChannelB, 0);
      ledcWrite(PWMChannelF, speed);
    }
  }
};

simpleMotor left;
simpleMotor right;

void setup() {
  Serial.begin(115200);
  WiFi.softAP(ssid, password);
  Udp.begin(localPort);
  delay(200);
  Serial.println(WiFi.softAPIP());

  left.setup(22, 23, 0, 1);
  right.setup(19, 18, 2, 3);
  }

void loop() {
  int packetSize = Udp.parsePacket();
  if (packetSize) {
    int len = Udp.read(packetBuffer, 255);
    if (len > 0) packetBuffer[len] = 0;
    Serial.print(String(packetBuffer[0]));
    Serial.println();
  }
  
  if (String(packetBuffer[0]) == "w"){
  left.setSpeed(450);
  right.setSpeed(450);
  Serial.println("f");
  }
  else if(String(packetBuffer[0]) == "s"){
  left.setSpeed(-400);
  right.setSpeed(-400);
  //Serial.println("b");
  }
  else if (String(packetBuffer[0]) == "a"){
  left.setSpeed(-450);
  right.setSpeed(450);
  Serial.println("f");
  }
  else if(String(packetBuffer[0]) == "d"){
  left.setSpeed(450);
  right.setSpeed(-450);
  //Serial.println("b");
  }
  else if(String(packetBuffer[0]) == "n"){
    left.setSpeed(0);
    right.setSpeed(0);
    Serial.println("s");
    
  }
  delay(50);
}

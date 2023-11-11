# UoBRobotics_sumo_robots WebSocket

## V3.2.8:
- web:
  - gamepad bug fixed
  - rotation bug fixed

## V3.2.7 changes:
- web:
  - better logic, reduced resource usage 

## V3.2.6 changes:
- Web:
  - add full-screen function
  - scale fixed
- find the bug: the joystick and button can't work simultaneously
- find the bug: Joystick offset error

## V3.2.5 changes:
- web:
  - better logic, reduced resource usage 
  - add debug mode
  - add rotation function
- fix the bug: won't stop sometime
- find the new bug: multi gamepads are not working

## Device:
* ESP32

## Functions:
* OTA
* Analogue output
* input:
  * touch screen
  * mouse
  * gamepad

## Library:
* ESPAsyncWebServer @1.2.6
* ESPAsyncTCP @1.2.4
* AsyncTCP @1.1.4
* <del>SPIFFS
* <del>mx1508

## <del>Ref: (SPIFFS)
* <del>https://blog.csdn.net/m0_50114967/article/details/126908896 ArduinoIDE < 2
* <del>https://docs.espressif.com/projects/esp-idf/en/latest/esp32/api-reference/storage/spiffs.html

## TODO:
- [x] support for gamepad
- [ ] voltage feedback
- [ ] signal feedback e.g. wifi db ...
- [x] make the webpage look better
- [x] more channels


#include <Arduino.h>
#include <WiFi.h>

#include <AsyncTCP.h>
#include <ESPAsyncWebSrv.h>

#include <iostream>
#include <sstream>

#include <ESP32MX1508.h>

AsyncWebServer server(80);
AsyncWebSocket wsCarInput("/CarInput");

const char* htmlHomePage PROGMEM = R"HTMLHOMEPAGE(
<!DOCTYPE html>
<html lang="en">
<head>
    <meta charset="UTF-8">
    <title>gamepad</title>
</head>
<body>
<canvas id="canvas" width="600" height="600" style="border: 1px solid #888"></canvas>

<form action="#">
    <input id='url' type="text" value="ws://localhost:8008">
    <input id='connect' type="button" value="connect">
    <input id='disconnect' type="button" value="disconnect">
    <sapn id="xvalue"></sapn>
</form>
</body>

<script src="jquery.min.js"></script>
<script type="application/javascript">
    $(function () {

        function init() {
            joystickdraw(BIG_CIRCUIT_R + POS_X, BIG_CIRCUIT_R + POS_Y);
        }

        function getXY(x, y) {

            let mouse2centerlength = Math.sqrt(x * x + y * y);
            let newx = x;
            let newy = y;
            if (mouse2centerlength > BIG_CIRCUIT_R) {
                let proportion = mouse2centerlength / BIG_CIRCUIT_R;
                newx = x / proportion;
                newy = y / proportion;
            }
            return {x: newx, y: newy};
        }

        function joystickdraw(posx, posy) {
            //ctx.fillStyle = "rgba(0, 0, 0, 0)";
            ctx.clearRect(0, 0, canvas.width, canvas.height);
            //保存转态
            ctx.save();
            //平移，根据鼠标位置
            let pos = getXY(posx - POS_X - BIG_CIRCUIT_R, posy - POS_Y - BIG_CIRCUIT_R);  //这里去掉偏移值
            output.x = parseInt(pos.x / BIG_CIRCUIT_R * 255);
            output.y = parseInt(pos.y / BIG_CIRCUIT_R * 255);
            sendwebsocket(output);
            $('#xvalue').text(output.x+","+output.y);

            ctx.translate(pos.x + POS_X + BIG_CIRCUIT_R - SAMLL_CIRCUIT_R, pos.y + POS_Y + BIG_CIRCUIT_R - SAMLL_CIRCUIT_R);
            ctx.beginPath();
            ctx.arc(SAMLL_CIRCUIT_R, SAMLL_CIRCUIT_R, SAMLL_CIRCUIT_R, 0, 2 * Math.PI);
            ctx.lineWidth = 10;
            ctx.fillStyle = 'black';
            ctx.fill();
            ctx.stroke();
            //ctx.drawImage(img1, 0, 0, SAMLL_CIRCUIT_R * 2, SAMLL_CIRCUIT_R * 2);
            ctx.restore();
        }

        function sendwebsocket(pos) {
            if (ws) {
                if (ws.readyState === WebSocket.OPEN)
                    ws.send(JSON.stringify(pos))
            }
        }

        function dragstart_event(e) {
            mousedown = true;
            joystickdraw(e.offsetX, e.offsetY)
        }

        function dragging_event(e) {
            if (mousedown) {
                joystickdraw(e.offsetX, e.offsetY)
            }
        }

        function dragstop_event() {
            mousedown = false;
            init();
        }

        function touchstart_event(e) {
            mousedown = true;
            joystickdraw(e.touches[0].clientX, e.touches[0].clientY)
        }

        function touching_event(e) {
            if (mousedown) {
                joystickdraw(e.touches[0].clientX, e.touches[0].clientY)
            }
        }

        function touchstop_event() {
            mousedown = false;
            init();
        }

        let mousedown = false;
        let ws;
        let texturl = document.getElementById('url');
        let btnconnect = document.getElementById('connect');
        let btndisconnect = document.getElementById('disconnect');
        let mycanvas = document.getElementById('canvas');
        let ctx = mycanvas.getContext('2d');

        let POS_X = 100;
        let POS_Y = 100;
        let SAMLL_CIRCUIT_R = 50; //小圆半径
        let BIG_CIRCUIT_R = 200;//大圆半径
        let output = {};

        mycanvas.onmousedown = dragstart_event;
        mycanvas.onmousemove = dragging_event;
        mycanvas.onmouseup = dragstop_event;

        mycanvas.ontouchstart = touchstart_event;
        mycanvas.ontouchmove = touching_event;
        mycanvas.ontouchend = touchstop_event;

        init();
    });

</script>
</html>

)HTMLHOMEPAGE";

MX1508 motorA(16, 17, 0, 1);
MX1508 motorB(18, 19, 2, 3);

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
      break;
    case WS_EVT_DISCONNECT:
      Serial.printf("WebSocket client #%u disconnected\n", client->id());
      break;
    case WS_EVT_DATA:
      AwsFrameInfo *info;
      info = (AwsFrameInfo*)arg;
      if (info->final && info->index == 0 && info->len == len && info->opcode == WS_TEXT) {
        //std::string myData = "";
        //myData.assign((char *)data, len);
        //std::istringstream ss(myData);
        //std::string key, value;
        //std::getline(ss, key, ',');
        //std::getline(ss, value, ',');
        //Serial.printf("Key [%s] Value[%s]\n", key.c_str(), value.c_str()); 
        //int valueInt = atoi(value.c_str());     
        Serial.printf("ws[%s][%u] %s-message[%llu]: ", server->url(), client->id(), (info->opcode == WS_TEXT)?"text":"binary", info->len);
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

  server.on("/", HTTP_GET, handleRoot);
  server.onNotFound(handleNotFound);
      
  wsCarInput.onEvent(onCarInputWebSocketEvent);
  server.addHandler(&wsCarInput);

  server.begin();
  Serial.println("HTTP server started");
}

void loop() 
{
  wsCarInput.cleanupClients(); 
}
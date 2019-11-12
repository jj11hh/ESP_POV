#include <ESP8266WiFi.h>
#include <WiFiUdp.h>
//#include <Hash.h>
#include <ESPAsyncTCP.h>
#include <ESPAsyncWebServer.h>
#include <ESPAsyncUDP.h>
#include <FixedPoints.h>
#include <FixedPointsCommon.h>
#include <math.h>

#include "display.h"
#include "common.h"
#include "EspTLC5947.h"
#include "msgQueue.h"
#include "keyEvent.h"
#include "game.h"

IPAddress ip(192,168,1,1);
IPAddress gateway(192,168,1,0);
IPAddress subnet(255,255,255,0);
AsyncWebServer server(80);
AsyncWebSocket ws("/ws");
uint8_t keyState = 0;
MsgQueue keyQueue;
AsyncUDP udpServer;

const unsigned int localUdpPort = 4210;
char incomingPacket[255];

void setupWiFi(){
  Serial.println("connecting wifi");
  //WiFi.mode(WIFI_STA);
  WiFi.softAPConfig(ip, gateway, subnet);
  WiFi.softAP("POV Display");
  ip = WiFi.softAPIP();
  //WiFi.begin("HTC U11", "wp980425");
  delay(500);
  yield();
  /*
  Serial.println("Connecting ...");
  int cnt = 0;
  while(WiFi.status() != WL_CONNECTED){
    delay(500);
    if (cnt < 10){
      tlc_clear(data_array, TLC_NUM);
      tlc_setpwm(data_array, TLC_NUM, led_map[3 * (cnt % LEDNUM)], 100);
      tlc_write(data_array, TLC_NUM, LATCH_PIN, -1);
    }
    if (cnt == 10){
      WiFi.beginSmartConfig();
      Serial.println("Starting SmartConfig.");
    }
    if (cnt >= 10){
      tlc_clear(data_array, TLC_NUM);
      tlc_setpwm(data_array, TLC_NUM, led_map[3 * (cnt % LEDNUM) + 2], 100);
      tlc_write(data_array, TLC_NUM, LATCH_PIN, -1);
      if(WiFi.smartConfigDone()){
        Serial.println("SmartConfig Success");
        break;
      }  
      Serial.print(".");
    }
    ++ cnt;
  }
  Serial.println(" connected"); 
  Serial.print("Connected to ");
  Serial.println(WiFi.SSID());              // Tell us what network we're connected to
  */
  Serial.print("IP address:\t");
  Serial.println(ip);           // Send the IP address of the ESP8266 to the computer
  //ip = WiFi.localIP();
}

void onWsEvent(AsyncWebSocket * server, AsyncWebSocketClient * client, AwsEventType type, void * arg, uint8_t *data, size_t len){
  if(type == WS_EVT_CONNECT){
    //Serial.printf("ws[%s][%u] connect\n", server->url(), client->id());
    client->ping();
  } else if(type == WS_EVT_DISCONNECT){
    //Serial.printf("ws[%s][%u] disconnect: %u\n", server->url(), client->id());
  } else if(type == WS_EVT_ERROR){
    //Serial.printf("ws[%s][%u] error(%u): %s\n", server->url(), client->id(), *((uint16_t*)arg), (char*)data);
  } else if(type == WS_EVT_PONG){
    //Serial.printf("ws[%s][%u] pong[%u]: %s\n", server->url(), client->id(), len, (len)?(char*)data:"");
  } else if(type == WS_EVT_DATA){
    AwsFrameInfo * info = (AwsFrameInfo*)arg;
    if(info->final && info->index == 0 && info->len == len){
      //the whole message is in a single frame and we got all of it's data
      //Serial.printf("ws[%s][%u] %s-message[%llu]: ", server->url(), client->id(), (info->opcode == WS_TEXT)?"text":"binary", info->len);
      if (len){
        keyState = data[len - 1];
        keyQueue.enQueue(keyState);
        //Serial.println((int)keyState);
      }
    }
  }
}


void setupServer(){
  SPIFFS.begin();
  ws.onEvent(onWsEvent);
  server.addHandler(&ws);
  server.on("/", HTTP_GET, [](AsyncWebServerRequest *request){
    //Serial.println("Serving /");
    request->send(SPIFFS, "/index.html");
  });  
  server.on("/upload_image", HTTP_POST, [](AsyncWebServerRequest *request){
    //Serial.println("Uploading Image...");
    request->send(200);
  }, NULL,
  [](AsyncWebServerRequest *request, uint8_t *data, size_t len, size_t index, size_t total){
    //Serial.printf("Reading %zu, index %zu\n", len, index);
    for(size_t i = index; i < index + len; i++){
      if (i < SCANLINE * SUBPIXEL_NUM)
        pov_buf_bg[i] = data[i - index];
        pov_buf[i] = data[i - index];
    }
  });
  server.on("/upload_gamma", HTTP_POST, [](AsyncWebServerRequest *request){
    //Serial.println("Uploading Gamma...");
    request->send(200);
  }, NULL,
  [](AsyncWebServerRequest *request, uint8_t *data, size_t len, size_t index, size_t total){
    //Serial.printf("Reading %zu, index %zu\n", len, index);
    for(size_t i = index; i < index + len; i++){
      if(i < 256)
        gamma8[i] = data[i - index];
    }
  });
  server.on("/upload_brightness", HTTP_POST, [](AsyncWebServerRequest *request){
    //Serial.println("Uploading Brightness...");
    request->send(200);
  }, NULL,
  [](AsyncWebServerRequest *request, uint8_t *data, size_t len, size_t index, size_t total){
    //Serial.printf("Reading %zu, index %zu\n", len, index);
    if (index == 0)
      brightness = data[0];
  });
  server.on("/upload_rotate", HTTP_POST, [](AsyncWebServerRequest *request){
    //Serial.println("Uploading Rotate...");
    request->send(200);
  }, NULL,
  [](AsyncWebServerRequest *request, uint8_t *data, size_t len, size_t index, size_t total){
    //Serial.printf("Reading %zu, index %zu\n", len, index);
    if (index == 0)
      scanRotate = data[0];
  });

  server.begin(); 

  if (udpServer.listen(4210)){
    //Serial.print("UDP Listening on IP: ");
    //Serial.println(WiFi.localIP());
    udpServer.onPacket([](AsyncUDPPacket packet) {
      char pack[16];
      int i;
      for (i = 0; i < 15 && i < packet.length(); i ++){
        pack[i] = packet.data()[i];
      }
      pack[4] = 0;
      //Serial.print("UDP received ");
      //Serial.println(pack);
      if (String("PING") == pack){
        packet.printf("PONG");
      }
    });    
  }
}

void setup()
{
  Serial.begin(74880);
  Serial.println("HELLO");
  setupDisplay();
  setupWiFi();
  setupServer();

  drawText5x7(8, SCANLINE / 2 - 5, ip.toString().c_str(), Color(255, 0, 255));
  commitBg();
}

void reRender() {
  static SQ15x16 lastTheta = 0;
  static SQ15x16 theta = 0;
  static bool dirty = true;
  static bool draw = false;
  static bool inGame = false;
  static GameState game;
  static KeyEvent keyEvent; 

  if (inGame){
    if (game.update()){
      return;  
    }
    else{
      game.exit();  
      keyEvent.lastKey = keyState; 
      inGame = false;
      dirty = false;
    }
  }

  if (dirty){
    clearCircle(11, lastTheta, 3);
    if (draw)
       fillCircle(11, theta, 3, Color(0, 255, 255));
    dirty = false;
  }

  keyEvent.onKeyDown([](uint8_t key, uint32_t keyState){
    switch (key){
      case KEY_UP:
        lastTheta = theta;
        theta += 0.1;
        dirty = true;
        break;
      case KEY_DOWN:
        lastTheta = theta;
        theta -= 0.1;
        dirty = true;
        break;
      case KEY_LEFT:
        scanRotate += 1;
        break;
      case KEY_RIGHT:
        scanRotate -= 1;
        break;
    }  
  });

  keyEvent.onKeyDownEdge([](uint8_t key, uint32_t keyState){
    switch (key){
      case KEY_A:
        draw = !draw;
        if (draw)
          dirty = true;
    }  
  });
  keyEvent.onKeyUpEdge([](uint8_t key, uint32_t keyState){
    switch (key){
      case KEY_B:
        game.enter();
        keyEvent.brk();
        inGame = true;
    }  
  });
  keyEvent.update(&keyQueue);
}

void loop(){
  static uint32_t tick = 0;
  pov_update();
  if (millis() - tick > 33){
    tick = millis();
    reRender();
  }
}

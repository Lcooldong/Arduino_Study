#define BUILTIN_BTN 3

#include <Arduino.h>
#include "LittleFS.h"
#include <DNSServer.h>
#ifdef ESP32
#include "WiFi.h"
#include <AsyncTCP.h>
#elif defined(ESP8266)
#include <ESP8266WiFi.h>
#include <ESPAsyncTCP.h>
#endif

// #include "ESPAsyncWebServer.h"

#include "neopixel.h"
#include  <WiFiManager.h>
#include "adafruitio_mqtt.h"

const char* ssid = "KT_GiGA_2G_Wave2_1205";
const char* pass = "8ec4hkx000";

const char* ssid_AP = "ESP32C3-WiFiManager";
const char* pass_AP = "12345678"; // 최소 8자리

IPAddress _ap_static_ip;

WiFiManager wm;
bool res = false;
MyNeopixel* myNeopixel = new MyNeopixel();

WiFiClient client;

Adafruit_MQTT_Client mqttClient(&client, MQTT_BROKER, MQTT_PORT, MQTT_USERNAME, AIO_KEY);
Adafruit_MQTT_Publish pub(&mqttClient, "CoolDong/f/home.led", MQTT_QOS_0);
Adafruit_MQTT_Subscribe sub(&mqttClient, "CoolDong/f/zig.vector", MQTT_QOS_0);
void subtestcallback(char *str, uint16_t len);
//WiFiServer myServer(80);
// AsyncWebServer myServer(80);
// AsyncEventSource events("/events");

MQTT mqtt;
//StepMotor stepmotor;

unsigned long previousMillis = 0;
const long interval = 10000;
unsigned long lastTime = 0;
unsigned long timerDelay = 5000;
static int32_t count = 0;


void setup() {
  Serial.begin(115200);
  Serial.println("Start ESP32");
  pinMode(BUILTIN_BTN, INPUT_PULLUP);
  myNeopixel->InitNeopixel();
  myNeopixel->pickOneLED(0, myNeopixel->strip->Color(255, 0, 0), 50, 50);
  if(!LittleFS.begin(true))
  {      
    Serial.println("LittleFS Mount Failed");
    return;
  }
  Serial.println("LittleFS Mounted");





  // // // KT 공유기 172.30.1.254
  // IPAddress ip (172, 30, 1, 40);  // M5stamp -> 40, LCD 0.42-> 41
  // IPAddress gateway (172, 30, 1, 254);
  // IPAddress subnet(255, 255, 255, 0);
  // WiFi.config(ip, gateway, subnet);

  WiFi.mode(WIFI_AP_STA);
  // WiFi.softAP(ssid_AP, pass_AP);


  // // 192.168.4.1 얻어오기
  // Serial.println(WiFi.softAPIP()); // 받은 서버를 여기로 연결함

  // //WiFi.softAP(ssid_AP);
  WiFi.begin(ssid, pass);
  
  long last_Time = millis();
  while(WiFi.status() != WL_CONNECTED)
  {
    delay(500);
    Serial.print(F("."));
    long current_Time = millis();
    if (current_Time - last_Time > 10000)
    {
        myNeopixel->pickOneLED(0, myNeopixel->strip->Color(0, 0, 255), 50, 50);
        wm.resetSettings();

        res = wm.autoConnect("PC_ON_OFF");
        if(!res)
        {
          Serial.println("Failed to connect");
        }
        else
        {
          Serial.println("connected...yeey :)");
          break;
        }  
    }

  }
  Serial.println();
  Serial.print(F("WiFi Connected -> IP : "));
  Serial.println(WiFi.localIP());

  // Subscribe 설정, Connect 이전에 해야함
  if( mqttClient.subscribe(&sub))
  {
    Serial.println("subscribed");
    // Callback 함수
    sub.setCallback(subtestcallback);
  }
  else 
  {
    Serial.println("subscriber not connected");
  }
    
  mqtt.MQTT_connect(mqttClient);
  Serial.println("MQTT Start");

  myNeopixel->pickOneLED(0, myNeopixel->strip->Color(0, 255, 0), 50, 50);
  // myServer.on("/", HTTP_GET, [](AsyncWebServerRequest * request)
  // {
  //   request->send(LittleFS, "/index.html", "text/html");
  // });

  // // css, js 파일 사용
  // myServer.serveStatic("/", LittleFS, "/");
  
  
  // myServer.begin();






}

void loop() 
{
  if (WiFi.status ()== WL_CONNECTED)
  {
    mqtt.MQTT_reconnect(mqttClient);

    if(millis() - lastTime > timerDelay)
    {
      //Serial.printf("%lf, %lf, %lf \r\n", angleX, angleY, angleZ);
      pub.publish(++count); // heartbeat
      Serial.println(count);

      
      lastTime = millis();
    }
  }
  else
  {
    // WiFi reconnect
    if(millis() - lastTime > 10000)
    {
      Serial.println("WiFi Disconnected");
      WiFi.reconnect();
      lastTime = millis();
    }
  }


  // if( digitalRead(BUILTIN_BTN))
  // {
  //   Serial.println("0");
  // }
  // else
  // {
  //   Serial.println("1");  // pressed
  // }  
}

// #include <memory>
// std::unique_ptr<DNSServer> dnsServer;
// std::unique_ptr<AsyncWebServer> server;

// void SetConfigPortal()
// {
//   dnsServer.reset(new DNSServer());
//   server.reset(new AsyncWebServer(80));
// }


void subtestcallback(char *str, uint16_t len)
{

    char buffer[len];
    String data;

    Serial.println("------------Message from ROBOT------------");
    
    for (size_t i = 0; i < len; i++)
    {
        //Serial.print(str[i]);
        buffer[i] = str[i];
        data += str[i];
    }
    Serial.println();
    //Serial.println(data);
    //Serial.printf("length : %d\r\n", len);

}
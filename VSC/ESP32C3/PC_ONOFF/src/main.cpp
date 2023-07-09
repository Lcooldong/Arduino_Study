// Library
#include <Arduino.h>
#include <WiFi.h>

#include <U8g2lib.h>
#include <Wire.h>

#include <DNSServer.h>
#include <AsyncTCP.h>
#include "ESPAsyncWebServer.h"

#include "neopixel.h"
#include "MyLittleFS.h"
// Define
#define BUILTIN_LED 2
#define LEDC_CHANNEL_0 0
#define LEDC_BASE_FREQ 5000
#define LEDC_TIMER_13_BIT 13


#define SDA_PIN 5
#define SCL_PIN 6
#define RGB_LED
//#define ANALOG_LED
//#define DEGITAL_LED

// Value
bool isOn = false;
int brightness = 0;
int fadeAmount = 5;

const char* ssid = "KT_GiGA_2G_Wave2_1205";
const char* pass = "8ec4hkx000";

const char* ssid_AP = "ESP32C3-WiFiManager";
const char* pass_AP = "12345678"; // 최소 8자리

// Function below
void ledcAnalogWrite(uint8_t channel, uint32_t value, uint32_t valueMax);

void onStatusUpdate(AsyncWebServerRequest *request);

// Instance
AsyncWebServer myServer(80);
// WiFiServer server(80);
MyNeopixel* myNeopixel = new MyNeopixel();
MyLittleFS* myLittleFS = new MyLittleFS();
U8G2_SSD1306_72X40_ER_F_HW_I2C u8g2(U8G2_R0, /* reset=*/ U8X8_PIN_NONE);

void setup() {
  Serial.begin(115200);
  Wire.begin(SDA_PIN, SCL_PIN);
  u8g2.begin();
  u8g2.enableUTF8Print();
  myLittleFS->InitLitteFS();
  myLittleFS->listDir(LittleFS, "/", 0);
  myLittleFS->writeFile(LittleFS, "/config.txt", "Hello C3");
  myLittleFS->readFile(LittleFS, "/config.txt");
  myLittleFS->listDir(LittleFS, "/", 0);

  IPAddress ip (172, 30, 1, 40);  // M5stamp -> 40, LCD 0.42-> 41
  IPAddress gateway (172, 30, 1, 254);
  IPAddress subnet(255, 255, 255, 0);
  WiFi.config(ip, gateway, subnet);
  WiFi.mode(WIFI_AP_STA);
  WiFi.softAP(ssid_AP, pass_AP);
  //WiFi.softAP(ssid_AP);
  WiFi.begin(ssid, pass);
  while(WiFi.status() != WL_CONNECTED)
  {
    delay(500);
    Serial.print(F("."));
  }
  Serial.println();
  Serial.print(F("WiFi Connected -> IP : "));
  Serial.println(WiFi.localIP());

  myServer.on("/", HTTP_GET, [](AsyncWebServerRequest * request)
  {
    request->send(LittleFS, "/index.html", "text/html");
  });

  myServer.on("/update", onStatusUpdate);

  // css, js 파일 사용
  myServer.serveStatic("/", LittleFS, "/");
  myServer.begin();




#ifdef ANALOG_LED
  ledcSetup(LEDC_CHANNEL_0, LEDC_BASE_FREQ, LEDC_TIMER_13_BIT);
  ledcAttachPin(BUILTIN_LED, LEDC_CHANNEL_0);
#endif

#ifdef DEGITAL_LED
  pinMode(BUILTIN_LED, OUTPUT);
#endif


}

void loop() {
  
  u8g2.clearBuffer();  
  u8g2.setFontMode(1);
  
  u8g2.setFont(u8g2_font_unifont_t_korean2);
  // u8g2.setFont(u8g2_font_cu12_tr);		  
  u8g2.setCursor(0,15);
  u8g2.print(F("안녕"));
  u8g2.sendBuffer();
  // delay(3000);
  // Serial.println("Loop");
  delay(2000);

// #ifdef RGB_LED
//   myNeopixel->pickOneLED(0, myNeopixel->strip->Color(255, 0, 0), 50, 50);
//   delay(1000);
//   myNeopixel->pickOneLED(0, myNeopixel->strip->Color(0, 255, 0), 50, 50);
//   delay(1000);
// #endif

#ifdef DIGITAL_LED
  if(isOn)
  {
    digitalWrite(BUILTIN_LED, HIGH);
    isOn = false;
    delay(1000);
  }
  else
  {
    digitalWrite(BUILTIN_LED, LOW);
    isOn = true;
    delay(1000);
  }
#endif

#ifdef ANALOG_LED
  ledcAnalogWrite(LEDC_CHANNEL_0, brightness, 255);
  brightness += fadeAmount;
  if(brightness <= 0 || brightness >= 255)
  {
    fadeAmount = -fadeAmount;
  }
  delay(50);
#endif

}


void onStatusUpdate(AsyncWebServerRequest *request)
{
  String inmsg;

  if(request->hasParam("state"))
  {
    inmsg = request->getParam("state")->value();
    myNeopixel->pickOneLED(0, myNeopixel->strip->Color(0, 255, 0), 50 * inmsg.toInt(), 50);
  }
  else if(request->hasParam("state2"))
  {
    inmsg = request->getParam("state2")->value();
    myNeopixel->pickOneLED(0, myNeopixel->strip->Color(0, 0, 255), 50 * inmsg.toInt(), 50);
  }
  else
  {
    inmsg = "No message sent";
  }

  Serial.println(inmsg);
  request->send(200, "text/plain", "OK");

}


void ledcAnalogWrite(uint8_t channel, uint32_t value, uint32_t valueMax = 255)
{
  uint32_t duty = (8191/ valueMax) * min(value, valueMax);
  ledcWrite(channel, duty);
}




// void InitWebServer()
// {
//   myServer.on("/", onRootRequest);


// }




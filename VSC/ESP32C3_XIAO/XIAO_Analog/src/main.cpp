#include <Arduino.h>
#include "neopixel.h"
#if defined(ESP8266)
  #include <ESP8266WiFi.h>
  #include <ESPAsyncTCP.h>
#elif defined(ESP32)
  #include <WiFi.h>
  #include <AsyncTCP.h>
#endif

#include <ESPAsyncWebServer.h>
#include <ElegantOTA.h>
#include <WebSerial.h>
#include <ESPAsyncWiFiManager.h>
#include <ArduinoOTA.h>
#include "MyLittleFS.h"

//#define XIAO
//#define M5STAMP
//#define LOLIN32
#define D1_MINI
//#define FIXED_IP

#ifdef M5STAMP
#define HALL_SENSOR_PIN 4
#define LED             5
// #define HALL_SENSOR_CUTOFF 3000 // 5V
#define HALL_SENSOR_CUTOFF 1900 // 3.3V
#endif

#ifdef XIAO
#define HALL_SENSOR_PIN 4   // GPIO4 = D2 (A2)
#define LED             5   // GPIO5 = D3 (A3)
#define HALL_SENSOR_CUTOFF 1700
#endif

#ifdef LOLIN32
#define HALL_SENSOR_PIN 4   // GPIO4 = D2 (A2)
#define LED             5   // GPIO5 = D3 (A3)
#define HALL_SENSOR_CUTOFF 2400
#endif

#ifdef D1_MINI
#define HALL_SENSOR_PIN 34
#define LED             5   // GPIO5 = D3 (A3)
#define HALL_SENSOR_CUTOFF 2500
#endif

#define HALL_SENSOR_INTERVAL 100
#define RESET_DEADLINE 400000000

#define WIFI_CONNECTION_INTERVAL 10000


uint64_t hallLastTime = 0;
unsigned long ota_progress_millis = 0;

AsyncWebServer server(80);
DNSServer dns;
MyLittleFS* mySPIFFS = new MyLittleFS();
MyNeopixel* myNeopixel = new MyNeopixel();

void resetBoardValue();


void setUpWiFi();
void onOTAStart();
void onOTAProgress(size_t current, size_t final);
void onOTAEnd(bool success);
void recvMsg(uint8_t *data, size_t len);
void arduinoOTAProgress();
void setupOTA();

void setup() {
  Serial.begin(115200);
  myNeopixel->InitNeopixel();
  pinMode(LED, OUTPUT);
  myNeopixel->pickOneLED(0, myNeopixel->strip->Color(255, 0, 0), 50, 1);

  setUpWiFi();
  setupOTA();


  delay(1000);
}

void loop() {
  ArduinoOTA.handle();
  ElegantOTA.loop();

  if (millis() - hallLastTime > HALL_SENSOR_INTERVAL)
  {
    int value = analogRead(HALL_SENSOR_PIN);
    Serial.printf("Value = %d\r\n", value);
    hallLastTime = millis();

    if(value < HALL_SENSOR_CUTOFF)
    {
      digitalWrite(LED, HIGH);
      myNeopixel->pickOneLED(0, myNeopixel->strip->Color(0, 255, 100), 50, 1);
    }
    else
    {
      digitalWrite(LED, LOW);
      myNeopixel->pickOneLED(0, myNeopixel->strip->Color(0, 0, 0), 0, 1);
    }

    
  }
  
  resetBoardValue();
}



void resetBoardValue()
{
  if(hallLastTime > RESET_DEADLINE)
  {
    ESP.restart();
  }

}

void setUpWiFi()
{
  AsyncWiFiManager wifiManager(&server,&dns);

  mySPIFFS->InitLitteFS();

  if(mySPIFFS->loadConfig(LittleFS))
  {
//    Serial.println(mySPIFFS->ssid);
//    Serial.println(mySPIFFS->pass);
#ifdef FIXED_IP
    IPAddress ip (192, 168, 1, 48);
    IPAddress gateway (192, 168, 1, 1);
    IPAddress subnet(255, 255, 255, 0);
    WiFi.config(ip, gateway, subnet);
#endif

    WiFi.mode(WIFI_STA);
    WiFi.begin(mySPIFFS->ssid, mySPIFFS->pass);
    Serial.print("Connect to Flash Memory!");
  }
  else
  {
    Serial.println("Saved file doesn't exist => Move to WiFiManager");
  }

  unsigned long connectionLastTime = millis();
  while (WiFi.status() != WL_CONNECTED) 
  {
      delay(500);
      Serial.print(".");

      if (millis() - connectionLastTime > WIFI_CONNECTION_INTERVAL)
      {
        Serial.println("Start WiFiManager => 192.168.4.1");
        wifiManager.resetSettings();
        bool wmRes = wifiManager.autoConnect("OTA_ANALOG");
        if(!wmRes)
        {
          Serial.println("Failed to connect");
        }
        else
        {
          Serial.printf("\nSuccess\n");

          mySPIFFS->saveConfig(LittleFS, wifiManager.getConfiguredSTASSID(), wifiManager.getConfiguredSTAPassword());
          delay(100);
          ESP.restart();

          break;  // Temp
        }
      }

  }
  Serial.print("\nIP : ");
  Serial.println(WiFi.localIP());

}

void onOTAStart() {
  // Log when OTA has started
  Serial.println("OTA update started!");
  // <Add your own code here>
}

void onOTAProgress(size_t current, size_t final) {
  // Log every 1 second
  if (millis() - ota_progress_millis > 1000) {
    ota_progress_millis = millis();
    Serial.printf("OTA Progress Current: %u bytes, Final: %u bytes\n", current, final);
  }
}

void onOTAEnd(bool success) {
  // Log when OTA has finished
  if (success) {
    Serial.println("OTA update finished successfully!");
  } else {
    Serial.println("There was an error during OTA update!");
  }
  // <Add your own code here>
}

void recvMsg(uint8_t *data, size_t len){
  WebSerial.println("Received Data...");
  String d = "";
  for(int i=0; i < len; i++){
    d += char(data[i]);
  }
  WebSerial.println(d);
  // if (d == "ON"){
  // digitalWrite(LED, HIGH);
  // }
  // if (d=="OFF"){
  //   digitalWrite(LED, LOW);
  // }
}


void arduinoOTAProgress()
{
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
}

void setupOTA()
{
  server.on("/", HTTP_GET, [](AsyncWebServerRequest *request) {
    request->send(200, "text/plain", "For Update -> /update \r\nFor Debug  -> /webserial");
  });

  ElegantOTA.begin(&server);    // Start ElegantOTA
  // ElegantOTA callbacks
  ElegantOTA.onStart(onOTAStart);
  ElegantOTA.onProgress(onOTAProgress);
  ElegantOTA.onEnd(onOTAEnd);

  WebSerial.begin(&server);
  WebSerial.msgCallback(recvMsg);

  arduinoOTAProgress();

  server.begin();
  Serial.println("HTTP server started");

}
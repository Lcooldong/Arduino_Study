#include <Arduino.h>
#include "M5AtomS3.h"

#include "WiFi.h"
#include <PubSubClient.h>
#include <ESPAsyncWebServer.h>
#include "ESPAsyncWiFiManager.h"

#include "MyLittleFS.h"

MyLittleFS* myLittleFS = new MyLittleFS();
AsyncWebServer server(80);
DNSServer dns;


void setup() {
  M5.begin(true, true, true, true);
  M5.IMU.begin();
  
  myLittleFS->InitLitteFS();
  AsyncWiFiManager wifiManager(&server,&dns);
  wifiManager.autoConnect("AutoConnectAP");
  Serial.println("connected...yeey :)");

  
  USBSerial.printf("0x%02x\n", M5.IMU.whoAmI());
}

float ax, ay, az, gx, gy, gz, t;

void loop() {
  M5.Lcd.setCursor(0, 40);
  M5.Lcd.clear();
  M5.IMU.getAccel(&ax, &ay, &az);
  M5.IMU.getGyro(&gx, &gy, &gz);
  M5.IMU.getTemp(&t);

  USBSerial.printf("%f | %f | %f | %f | %f | %f | %f\n", ax, ay, az, gx, gy, gz, t);

  M5.Lcd.printf("IMU:\r\n");
  M5.Lcd.printf("%0.2f %0.2f %0.2f\r\n", ax, ay, az);
  M5.Lcd.printf("%0.2f %0.2f %0.2f\r\n", gx, gy, gz);
  delay(500);
}
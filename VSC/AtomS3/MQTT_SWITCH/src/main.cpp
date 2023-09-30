#define RELAY_PIN 8
#define WIFI_CONNECTION_INTERVAL 10000

#include <Arduino.h>

#include <WiFi.h>
#include <PubSubClient.h>
#include <ESPAsyncWebServer.h>
#include "ESPAsyncWiFiManager.h"

#include "ImageViewer.hpp"
#include "MyLittleFS.h"

MyLittleFS* mySPIFFS = new MyLittleFS();
AsyncWebServer server(80);
DNSServer dns;

ImageViewer viewer;

WiFiClient espClient;
PubSubClient client(espClient);


const char* mqtt_server = "mqtt.m5stack.com";

unsigned long lastMsg = 0;
#define MSG_BUFFER_SIZE (50)
char msg[MSG_BUFFER_SIZE];
int value = 0;

void forever();
void callback(char* topic, byte* payload, unsigned int length);
void reConnect();


bool btnToggle = false;



void setup() {

 // myLittleFS->InitSPIFFS();
  
  AsyncWiFiManager wifiManager(&server,&dns);
  USBSerial.begin(115200);
  USBSerial.flush();
  delay(1000);

  pinMode(RELAY_PIN, OUTPUT);
  digitalWrite(RELAY_PIN, LOW);
  // SPIFFS 적용됨
  if (!viewer.begin()) {
        forever();
  }
  
//  myLittleFS->listDir(SPIFFS, "/", 0);
  USBSerial.println("Connecting to WiFi...");

  if(mySPIFFS->loadConfig(SPIFFS))
  {
    USBSerial.println(mySPIFFS->ssid);
    USBSerial.println(mySPIFFS->pass);
    M5.update();

    WiFi.mode(WIFI_AP_STA);
    WiFi.begin(mySPIFFS->ssid, mySPIFFS->pass);
    USBSerial.println("Connect to Flash Memory");
    M5.update();
  }
  else
  {
    const char* ssid        = "Cooldong";
    const char* password    = "8ec4hkx000";
    WiFi.begin(ssid, password);
    USBSerial.println("Connect to HOME");
    M5.update();
  }

  unsigned long connectionLastTime = millis();
  while (WiFi.status() != WL_CONNECTED) {
      delay(500);
      USBSerial.print(".");
      M5.update();

      digitalWrite(RELAY_PIN, HIGH);
      if (millis() - connectionLastTime > WIFI_CONNECTION_INTERVAL)
      {
        USBSerial.println("Start WiFiManager => 192.168.4.1");
        M5.update();
        wifiManager.resetSettings();
        bool wmRes = wifiManager.autoConnect("PC_SWITCH");
        if(!wmRes)
        {
          USBSerial.println("Failed to connect");
        }
        else
        {
          USBSerial.printf("\nSuccess\n");
          M5.update();
          M5.Lcd.clear();
          M5.Lcd.println("Success");
          
          mySPIFFS->saveConfig(SPIFFS, wifiManager.getConfiguredSTASSID(), wifiManager.getConfiguredSTAPassword());
          delay(100);
          ESP.restart();

          break;  // Temp
        }
      }

  }
  
  
  
  USBSerial.printf("\n%s\r\n", "Start MQTT Setup");
  M5.update();

  client.setServer(mqtt_server, 1883);  // Sets the server details. 
  client.setCallback(callback);  // Sets the message callback function.  

  
  // M5.IMU.begin();
  // USBSerial.printf("0x%02x\n", M5.IMU.whoAmI());




}

float ax, ay, az, gx, gy, gz, t;
uint8_t num[3] = {0x00, 0x00, 0x00};

int count = 0;
unsigned long lastTime = 0;
unsigned int interval = 100;
unsigned long restartTime = 4000000000;


void loop() {

  if (!client.connected()) {
        reConnect();
  }
  client.loop();

  // IMAGE
  if (millis() - lastTime > interval)
  {
    viewer.update();
    lastTime = millis();

    if(lastTime > restartTime)
    {
      ESP.restart();
    }
  }



  if(M5.BtnA.isPressed())
  {    
    USBSerial.printf("Count : %d\r\n", count++);
    ++value;
    snprintf(msg, MSG_BUFFER_SIZE, "hello world #%ld", value);  // Format to the specified string and store it in MSG.

    USBSerial.print("Publish message: ");
    USBSerial.println(msg);
    client.publish("M5Stack", msg);  // Publishes a message to the specified    
    
    digitalWrite(RELAY_PIN, LOW);
    delay(100);
    digitalWrite(RELAY_PIN, HIGH);
    delay(100);
    digitalWrite(RELAY_PIN, LOW);
  }

  // unsigned long now = millis();  // Obtain the host startup duration.  获取主机开机时长
  //   if (now - lastMsg > 2000) {
  //       lastMsg = now;
  //       ++value;
  //       snprintf(msg, MSG_BUFFER_SIZE, "hello world #%ld", value);  // Format to the specified string and store it in MSG.

  //       USBSerial.print("Publish message: ");
  //       USBSerial.println(msg);
  //       client.publish("M5Stack", msg);  // Publishes a message to the specified
                                  
  //   }

}





void forever(void) {
    while (true) {
        delay(1);
    }    
}

void callback(char* topic, byte* payload, unsigned int length) {
    USBSerial.print("Message arrived [");
    USBSerial.print(topic);
    USBSerial.print("] -> ");

  //  byte* p = (byte*)malloc(length);
   // memcpy(p, payload, length);

    for (int i = 0; i < length; i++) {
        USBSerial.print((char)payload[i]);
    }
    USBSerial.println();
 //   USBSerial.println((char *)p);

  //  free(p);
    //if(){}
}

void reConnect() {
    while (!client.connected()) {
        USBSerial.print("Attempting MQTT connection...");
        // Create a random client ID.  创建一个随机的客户端ID
        String clientId = "M5Stack-";
        clientId += String(random(0xffff), HEX);
        // Attempt to connect.  尝试重新连接
        if (client.connect(clientId.c_str())) {
            USBSerial.println("connected");
            // Once connected, publish an announcement to the topic.
            // 一旦连接，发送一条消息至指定话题
            client.publish("M5Stack", "hello world");
            // ... and resubscribe.  重新订阅话题
            client.subscribe("M5Stack");
        } else {
            USBSerial.print("failed, rc=");
            USBSerial.print(client.state());
            USBSerial.println("try again in 5 seconds");
            delay(5000);
        }
    }
}
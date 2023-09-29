#define RELAY_PIN 1

#include <Arduino.h>

#include <WiFi.h>
#include <PubSubClient.h>
#include <ESPAsyncWebServer.h>
#include "ESPAsyncWiFiManager.h"

#include "ImageViewer.hpp"
#include "MyLittleFS.h"

MyLittleFS* myLittleFS = new MyLittleFS();
AsyncWebServer server(80);
DNSServer dns;

ImageViewer viewer;

WiFiClient espClient;
PubSubClient client(espClient);

const char* ssid        = "Cooldong";
const char* password    = "8ec4hkx000";
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

 // myLittleFS->InitLitteFS();
  
  AsyncWiFiManager wifiManager(&server,&dns);
  USBSerial.begin(115200);
  USBSerial.flush();
  delay(1000);
  // SPIFFS 적용됨
  if (!viewer.begin()) {
        forever();
  }
  
//  myLittleFS->listDir(SPIFFS, "/", 0);
  USBSerial.printf("Connecting to %s", ssid);
  WiFi.mode(WIFI_STA);  // Set the mode to WiFi station mode.  设置模式为WIFI站模式
  WiFi.begin(ssid, password);  // Start Wifi connection.  开始wifi连接

  while (WiFi.status() != WL_CONNECTED) {
      delay(500);
      USBSerial.print(".");
  }
  USBSerial.printf("\nSuccess\n");
  M5.Lcd.println("Success");
  
  wifiManager.autoConnect("PC_SWITCH");
  USBSerial.printf("%s\n", "WiFi");
  M5.update();

  client.setServer(mqtt_server, 1883);  // Sets the server details. 
  client.setCallback(callback);  // Sets the message callback function.  

  pinMode(RELAY_PIN, OUTPUT);
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
    delay(500);
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
    USBSerial.print("] ->");

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
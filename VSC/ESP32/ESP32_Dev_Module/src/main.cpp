#define BUILTIN_BTN 3
#define ON_OFF_PIN 4
#define RESET_PIN 6
#define STATUS_PIN 5

#include <Arduino.h>
//#include "LittleFS.h"
#include "MyLittleFS.h"
#include <DNSServer.h>
#include "WiFi.h"
#include <AsyncTCP.h>


#include "neopixel.h"
#include  <WiFiManager.h>
#include "adafruitio_mqtt.h"

//const char* _ssid = "KT_GiGA_2G_Wave2_1205";
const char* _pass = "8ec4hkx000";

//const char* ssid = "UARobotics_212_2.4G";
//const char* pass = "uarobotics";

IPAddress _ap_static_ip;

WiFiManager wm;
bool res = false;
MyNeopixel* myNeopixel = new MyNeopixel();
MyLittleFS* myLittleFS = new MyLittleFS();
WiFiClient client;

Adafruit_MQTT_Client mqttClient(&client, MQTT_BROKER, MQTT_PORT, MQTT_USERNAME, AIO_KEY);
Adafruit_MQTT_Publish pub(&mqttClient, "CoolDong/f/home.led", MQTT_QOS_0);
Adafruit_MQTT_Publish pub_on_off(&mqttClient, "CoolDong/f/pc-lcd.on-off", MQTT_QOS_0);
Adafruit_MQTT_Publish pub_volt(&mqttClient, "CoolDong/f/pc-lcd.connection-volt", MQTT_QOS_0);

Adafruit_MQTT_Subscribe sub_on_off(&mqttClient, "CoolDong/feeds/pc-lcd.on-off", MQTT_QOS_0);
Adafruit_MQTT_Subscribe sub_reset(&mqttClient, "CoolDong/feeds/pc-lcd.reset", MQTT_QOS_0);

void subOnOffcallback(char *str, uint16_t len);
void subResetcallback(char *str, uint16_t len);

MQTT mqtt;

unsigned long previousMillis = 0;
const long interval = 10000;
unsigned long lastTime = 0;
unsigned long timerDelay = 5000;
static int32_t count = 0;
long current_Time = 0;
bool status = false;
bool setupFlag = false;
float volt = 0;


void setup() {
  Serial.begin(115200);
  Serial.println("Start ESP32");
  pinMode(BUILTIN_BTN, INPUT_PULLUP);
  pinMode(STATUS_PIN, INPUT_PULLUP);
  pinMode(ON_OFF_PIN, OUTPUT);
  pinMode(RESET_PIN, OUTPUT);
  myNeopixel->InitNeopixel();
  myNeopixel->pickOneLED(0, myNeopixel->strip->Color(255, 0, 0), 50, 50);
  myLittleFS->InitLitteFS();
  //  myLittleFS->listDir(LittleFS, "/", 0);
  // SSID, PASS 불러오기
  
//  myLittleFS->readFile(LittleFS, "/config.txt");
  if(myLittleFS->loadConfig())
  {
    Serial.println(myLittleFS->ssid);  
    Serial.println(myLittleFS->pass);

    WiFi.mode(WIFI_AP_STA);
    WiFi.begin(myLittleFS->ssid, myLittleFS->pass);
  }
  


  //WiFi.begin(_ssid, _pass);

  long last_Time = millis();
  while(WiFi.status() != WL_CONNECTED)
  {
    delay(500);
    Serial.print(F("."));
    current_Time = millis();
    if (current_Time - last_Time > 10000)
    {

        WiFi.disconnect();
        myNeopixel->pickOneLED(0, myNeopixel->strip->Color(0, 0, 255), 50, 50);
        wm.resetSettings();

        res = wm.autoConnect("ESP32C3_WiFiManager");
        if(!res)
        {
          Serial.println("Failed to connect");
        }
        else
        {
          Serial.println("connected...yeey :)");
          // 새로운 SSID, PASS 쓰기

          myLittleFS->saveConfig(wm.getWiFiSSID(), wm.getWiFiPass());
          // myLittleFS->writeFile(LittleFS, "/config.txt", "Hello C3");
          break;
        }  
    }

  }
  Serial.println();
  Serial.print(F("WiFi Connected -> IP : "));
  Serial.println(WiFi.localIP());
  
  


  // Subscribe 설정, Connect 이전에 해야함
  if( mqttClient.subscribe(&sub_on_off) && mqttClient.subscribe(&sub_reset))
  {
    Serial.println("subscribed");
    // Callback 함수
    sub_on_off.setCallback(subOnOffcallback);
    sub_reset.setCallback(subResetcallback);   
  }
  else 
  {
    Serial.println("subscriber not connected");
  }


  // MQTT 시작  
  mqtt.MQTT_connect(mqttClient);
  Serial.println("MQTT Start");

  myNeopixel->pickOneLED(0, myNeopixel->strip->Color(0, 255, 0), 50, 50);
  
  volt = analogRead(STATUS_PIN);
  Serial.print("Current State : ");
  Serial.println(volt);

  if (volt)
  {
    pub_on_off.publish("Off");    // 아래랑 바꾸기 테스트중
  }
  else
  {
    pub_on_off.publish("on");
  }

  delay(500);
  setupFlag = true;
}

void loop() 
{
  if (WiFi.status ()== WL_CONNECTED)
  {
    myNeopixel->pickOneLED(0, myNeopixel->strip->Color(0, 255, 0), 50, 50);
    mqtt.MQTT_reconnect(mqttClient);

    if(millis() - lastTime > timerDelay)
    {
      //Serial.printf("%lf, %lf, %lf \r\n", angleX, angleY, angleZ);
      pub.publish(++count); // heartbeat
      Serial.println(count);

      volt = analogRead(STATUS_PIN);
      pub_volt.publish(volt);
      if (volt > 1024)
      {
        status = true;
      }
      else
      {
        status = false;
      }
      lastTime = millis();
    }
  }
  else
  {
    // WiFi reconnect
    if(millis() - lastTime > 10000)
    {
      Serial.println("WiFi Disconnected");
      myNeopixel->pickOneLED(0, myNeopixel->strip->Color(255, 0, 0), 50, 50);
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


void subOnOffcallback(char *str, uint16_t len)
{

    char buffer[len];
    String data;

    if (setupFlag)
    {
      Serial.println("------------On Off Message------------");
    
      for (size_t i = 0; i < len; i++)
      {
          //Serial.print(str[i]);
          buffer[i] = str[i];
          data += str[i];
      }
      Serial.println();
      Serial.println(data);
      Serial.printf("length : %d\r\n", len);

      if (data == "On")
      {
        Serial.println("Turn On PC");
        digitalWrite(ON_OFF_PIN, HIGH);
        delay(100);
        digitalWrite(ON_OFF_PIN, LOW);
        delay(100);
        digitalWrite(ON_OFF_PIN, HIGH);
      }
      else if (data == "Off")
      {
        if (status == true)
        {
          Serial.println("Turn Off PC");
          digitalWrite(ON_OFF_PIN, LOW);
          delay(5000);
          digitalWrite(ON_OFF_PIN, HIGH);
        }

      }


    }

    

}

void subResetcallback(char *str, uint16_t len)
{

    char buffer[len];
    String data;

    Serial.println("------------Reset Message------------");
    
    for (size_t i = 0; i < len; i++)
    {
        //Serial.print(str[i]);
        buffer[i] = str[i];
        data += str[i];
    }
    Serial.println();
    Serial.println(data);
    Serial.printf("length : %d\r\n", len);

    if (data == "Reset")
    {
      Serial.println("Reset PC");
      digitalWrite(RESET_PIN, LOW);
    }
    else
    {
      digitalWrite(RESET_PIN, HIGH);
    }

}
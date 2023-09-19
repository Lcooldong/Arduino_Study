#define I2C_SCL 7
#define I2C_SDA 8
//#define SWITCH 21
#define BTN 3

#include <Arduino.h>
#include "neopixel.h"
#include <Wire.h>
#include <BH1750.h>

unsigned long lastTime = 0;
unsigned long currentTime = 0;
unsigned int interval = 50;

MyNeopixel* myNeopixel = new MyNeopixel();
bool toggleFlag = false;
BH1750 lightMeter;

void setup() {
  Serial.setTimeout(500);
  Serial.begin(115200);
  myNeopixel->InitNeopixel();
  myNeopixel->pickOneLED(0, myNeopixel->strip->Color(0, 0, 255), 50, 50);

  //pinMode(SWITCH, OUTPUT);
  
  Wire.begin(I2C_SDA, I2C_SCL);
  
  if (lightMeter.begin(BH1750::CONTINUOUS_HIGH_RES_MODE)) {
    Serial.println(F("BH1750 Advanced begin"));
  } else {
    Serial.println(F("Error initialising BH1750"));
  }
}

void loop() {


  if (Serial.available())
  {
    String text = Serial.readStringUntil('\n');
    //Serial.printf("Input Text :  %s  (%d)\r\n", text, text.length());
    if(text == "on")
    {
      myNeopixel->pickOneLED(0, myNeopixel->strip->Color(0, 255, 0), 50, 50);
     // digitalWrite(SWITCH, HIGH);
     while (true)
     {
      if (millis() - lastTime > interval)
      {
          lastTime = millis();
          if (lightMeter.measurementReady()) {
          float lux = lightMeter.readLightLevel();
          Serial.printf("%.2f\n", lux);
        }  
      }
      else
      {
        String cancelText = Serial.readString();
        if(cancelText == "off\n")
        {
          myNeopixel->pickOneLED(0, myNeopixel->strip->Color(255, 0, 0), 50, 50);
          break;
        }
      }
     }
    }

  }

}


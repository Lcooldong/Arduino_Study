#include <Arduino.h>
#include "neopixel.h"


//#define XIAO
#define M5STAMP

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

#define HALL_SENSOR_INTERVAL 100
#define RESET_DEADLINE 400000000

uint64_t hallLastTime = 0;
MyNeopixel* myNeopixel = new MyNeopixel();


void resetBoardValue();

void setup() {
  Serial.begin(115200);
  myNeopixel->InitNeopixel();
  pinMode(LED, OUTPUT);
  myNeopixel->pickOneLED(0, myNeopixel->strip->Color(255, 0, 100), 50, 1);
}

void loop() {
  if (millis() - hallLastTime > HALL_SENSOR_INTERVAL)
  {
    int value = analogRead(HALL_SENSOR_PIN);
    Serial.printf("Value = %d\r\n", value);
    hallLastTime = millis();

    if(value < HALL_SENSOR_CUTOFF)
    {
      digitalWrite(LED, HIGH);
    }
    else
    {
      digitalWrite(LED, LOW);
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


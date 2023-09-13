#include <Arduino.h>
#include "neopixel.h"

#define SWITCH 21

MyNeopixel* myNeopixel = new MyNeopixel();
bool toggleFlag = false;

void setup() {
  Serial.setTimeout(500);
  Serial.begin(115200);
  myNeopixel->InitNeopixel();
  myNeopixel->pickOneLED(0, myNeopixel->strip->Color(0, 0, 255), 50, 50);

  pinMode(SWITCH, OUTPUT);
}

void loop() {
  if(Serial.available())
  {
    String text = Serial.readStringUntil('\n');
    Serial.printf("Input Text :  %s  (%d)\r\n", text, text.length());
    if(text == "on")
    {
      myNeopixel->pickOneLED(0, myNeopixel->strip->Color(0, 255, 0), 50, 50);
      digitalWrite(SWITCH, HIGH);
    }
    else if (text == "off")
    {
      myNeopixel->pickOneLED(0, myNeopixel->strip->Color(255, 0, 0), 50, 50);
      digitalWrite(SWITCH, LOW);
    }
  }

}


#include "mainFunc.h"

const char* apName = "AP_1";

void setup() {
  Serial.begin(115200);
  myNeopixel->InitNeopixel();
  pinMode(LED, OUTPUT);
  myNeopixel->pickOneLED(0, myNeopixel->strip->Color(255, 0, 0), 50, 1);

  setUpWiFi();
  setupOTA();
  setupESPNOW();

#ifdef MASTER
  setupESPNOWPair();
#endif

  delay(1000);
}

void loop() {
  ArduinoOTA.handle();
  ElegantOTA.loop();

  if (millis() - hallLastTime > HALL_SENSOR_INTERVAL)
  {
    int value = analogRead(HALL_SENSOR_PIN);
//    Serial.printf("Value = %d\r\n", value);
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

#ifdef MASTER
  
  sendData();
#endif

  resetBoardValue();
}


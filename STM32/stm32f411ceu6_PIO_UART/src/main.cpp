#include <Arduino.h>

uint32_t lastMillis = 0;
uint32_t count = 0;

void setup() {
  Serial.begin(115200);
  pinMode(PC13, OUTPUT);
}

void loop() {
  uint32_t curMillis = millis();

  if(curMillis - lastMillis >= 100)
  {
    lastMillis = curMillis;
    digitalToggle(PC13);
    count++;
    Serial.printf("%d\r\n", count);
  }
}


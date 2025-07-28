#include <Arduino.h>
#define LED_PIN PB_2

bool ledState = false;

void setup() {
  Serial.begin(115200);
  SerialUSB.begin(115200);
  pinMode(LED_PIN, OUTPUT);
  
}

void loop() {
  if(ledState) {
    digitalWrite(LED_PIN, HIGH);
  } else {
    digitalWrite(LED_PIN, LOW);
  }
  ledState = !ledState; // Toggle the state
  delay(500); // Wait for 500 milliseconds
}



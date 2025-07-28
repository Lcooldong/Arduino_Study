#include <Arduino.h>
#include "OneButton.h"

#define LED_PIN PB2
#define BUTTON_PIN PC13

bool ledState = false;
uint32_t currentMillis = 0;
uint32_t previousMillis[2] = {0, 0}; // For LED and button
OneButton button;

void setup() {
  Serial.begin(115200);

  pinMode(LED_PIN, OUTPUT);
  button.setup(BUTTON_PIN, INPUT_PULLDOWN, false);
  button.attachClick([]() {
    Serial.println("Button Clicked!");
  });
  
}

void loop() {
  currentMillis = millis();
  if(currentMillis - previousMillis[0] >= 500) {
    previousMillis[0] = currentMillis;
    Serial.println("Toggling LED");
    digitalWrite(LED_PIN, ledState ? HIGH : LOW);
    ledState = !ledState;
  }
  else if(currentMillis - previousMillis[1] >= 10) {
    previousMillis[1] = currentMillis;
    button.tick(); // Check button state
  }
}



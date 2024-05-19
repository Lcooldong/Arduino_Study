#include <Arduino.h>


void setup() {
  Serial.begin(115200);
  pinMode(13, OUTPUT);
  pinMode(2, OUTPUT);

}

void loop() {
  Serial.println("H");
  digitalWrite(2, LOW);
  delay(1000);
  digitalWrite(2, HIGH);
  delay(1000);
}


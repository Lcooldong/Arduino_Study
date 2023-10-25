#include <Arduino.h>
#include <Servo.h>

#define MG996R_PIN 28

Servo myServo;

void setup() {
  myServo.attach(MG996R_PIN);
 //myServo.setPeriodHertz(500);
 //myServo.attach(MG996R_PIN, 1000, 2000);
}

void loop() {
 for (int pos = 0; pos <= 180; pos += 1) {
    myServo.write(pos);
    delay(10);
  }
 // Rotation from 180° to 0.
  for (int pos = 180; pos >= 0; pos -= 1) {
    myServo.write(pos);
    delay(10);
  }
  
}


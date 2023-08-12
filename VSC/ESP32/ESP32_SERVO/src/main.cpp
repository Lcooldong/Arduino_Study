#include <Arduino.h>
#include <Servo.h>

<<<<<<< Updated upstream
//#define FULL_DEGREE

=======
#include <SPI.h>
#include <Wire.h>
#include <WiFi.h>
>>>>>>> Stashed changes

#define LIMIT_SENSOR_PIN 4
#define SERVO_PIN 23
#define BTN_PIN 39


Servo myservo;

int pos = 0;

void setup() {
  Serial.begin(115200);
  pinMode(LIMIT_SENSOR_PIN, INPUT_PULLUP);
  pinMode(BTN_PIN, INPUT_PULLUP);
  myservo.attach(SERVO_PIN);

}

void loop() {
<<<<<<< Updated upstream

#ifdef FULL_DEGREE
=======
  if(digitalRead(BTN_PIN))
  {
    myservo.write(90);
    delay(100);
  }
  else
  {
    myservo.write(0);
    delay(100);
  }
  
>>>>>>> Stashed changes
  if(!digitalRead(LIMIT_SENSOR_PIN)) // Pressed -> 0,  Release -> 1
  {
    myservo.write(90);  // Stop
  }
  else
  {
    myservo.write(180); // ccw
    // myservo.write(0);   // cw 
  }
 // 0 ~ 1499  CW, 1501 ~ 2500 CCW, 1500 정지 1500에 가까울 수록 느림 (좌우로 1470 ~ 1530 정도는 멈춤)
  // for (int i = 1500; i < 2500; i++)
  // {
  //   myservo.writeMicroseconds(i);
  //   Serial.printf("Pulse Width :  %d\n", i);
  //   delay(100);
    
  // }
#endif
myservo.write(90);


// for (pos = 0; pos < 120; pos++)
// {
//   myservo.write(pos);
//   Serial.printf("Pos : %d\n", pos);
//   delay(10);
// }

// for (pos = 119; pos >= 0; pos--)
// {
//   myservo.write(pos);
//   Serial.printf("Pos : %d\n", pos);
//   delay(10);
// }


 
  
}


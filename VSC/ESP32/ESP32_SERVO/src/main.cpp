#include <Arduino.h>
#include <Servo.h>

#define LIMIT_SENSOR_PIN 4
#define SERVO_PIN 5


Servo myservo;

int pos = 0;

void setup() {
  Serial.begin(115200);
  pinMode(LIMIT_SENSOR_PIN, INPUT_PULLUP);
  myservo.attach(SERVO_PIN);

}

void loop() {

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
  
}


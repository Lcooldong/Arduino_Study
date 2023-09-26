#include <Arduino.h>
#include "M5Atom.h"
#include "neopixel.h"
#include <ESP32Servo.h>

#define SERVO_PIN 33

long count = 0;
int pos = 0;
int targetPos = 135;
MyNeopixel* myNeopixel = new MyNeopixel();
Servo gripperServo;


void setup() {

  Serial.setTimeout(500);
  M5.begin(true, true, false);
  myNeopixel->InitNeopixel();


 if(!gripperServo.attached())
 {
  gripperServo.setPeriodHertz(50);
  gripperServo.attach(SERVO_PIN, 1000, 2000);
 }
 gripperServo.write(0);


  delay(50);
  Serial.println("Start Atom");
  
  myNeopixel->pickOneLED(0, myNeopixel->strip->Color(255, 0, 255), 50, 50);
}

void loop() {
  if(M5.Btn.wasPressed())
  {
    Serial.printf("Count : %d\r\n", count++);
    delay(50);
  }
  
  M5.update();  // M5.Btn.read();


  if (Serial.available())
  {
    String text = Serial.readStringUntil('n');

    if(text == "1")
    {
      

      if (pos == 0)
      {
        myNeopixel->pickOneLED(0, myNeopixel->strip->Color(255, 0, 0), 50, 50);
        for (int i = 0; i <= targetPos; i++)
        {
          gripperServo.write(i);
          pos = i;
          printf("Degree : %d\r\n", i);
          delay(2);
        }

      }
      
    }
    else if (text = "2")
    {
      
      if (pos == targetPos)
      {
        myNeopixel->pickOneLED(0, myNeopixel->strip->Color(0, 255, 0), 50, 50);
        for (int i = targetPos; i >= 0; i--)
        {
          gripperServo.write(i);
          pos = i;
          printf("Degree : %d\r\n", i);
          delay(2);
        }

      }

    }




  }

    

  



  
}




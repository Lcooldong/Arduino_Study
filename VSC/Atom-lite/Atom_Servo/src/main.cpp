#include <Arduino.h>
#include "M5Atom.h"
#include "neopixel.h"
#include <ESP32Servo.h>

#include <Adafruit_DotStar.h>
#include <Wire.h>
#include <SPI.h> 

#define SERVO_PIN 33

#define NUMPIXELS 8
#define DATAPIN    25
#define CLOCKPIN   21



long count = 0;
int pos = 0;
int targetPos = 135;
MyNeopixel* myNeopixel = new MyNeopixel();
Servo gripperServo;
Adafruit_DotStar strip(NUMPIXELS, DATAPIN, CLOCKPIN, DOTSTAR_BRG);

void setup() {

  Serial.setTimeout(500);
  M5.begin(true, true, false);
  myNeopixel->InitNeopixel();
  strip.begin(); // Initialize pins for output
  strip.show();  // Turn all LEDs off ASAP

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

int      head  = 0, tail = -10; // Index of first 'on' and 'off' pixels
uint32_t color = 0xFFFFFF;      // 'On' color (starts red)

void loop() {
  if(M5.Btn.wasPressed())
  {
    Serial.printf("Count : %d\r\n", count++);

                     // Pause 20 milliseconds (~50 FPS)

   
              
    
  }
  
  M5.update();  // M5.Btn.read();

  for (int i = 0; i < NUMPIXELS; i++)
  {
    strip.setPixelColor(i, color); // 'On' pixel at head
    strip.show();                     // Refresh strip
    delay(20);    
  }
  
    

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




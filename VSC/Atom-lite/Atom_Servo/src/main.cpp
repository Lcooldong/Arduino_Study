#include <Arduino.h>
#include "M5Atom.h"
#include "neopixel.h"
#include <ESP32Servo.h>

#include <Adafruit_DotStar.h>
#include <Wire.h>
#include <SPI.h> 
#include <BH1750.h>

#define SERVO_PIN 33

#define NUMPIXELS 8
//#define DATAPIN    25
//#define CLOCKPIN   21

#define DATA_PIN    19
#define CLOCK_PIN   22

#define I2C_SCL 21
#define I2C_SDA 25


long count = 0;
int pos = 0;
int targetPos = 150;
MyNeopixel* myNeopixel = new MyNeopixel();
Servo gripperServo;
BH1750 lightMeter;
Adafruit_DotStar strip(NUMPIXELS, DATA_PIN, CLOCK_PIN, DOTSTAR_BRG);



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

  if (lightMeter.begin(BH1750::CONTINUOUS_HIGH_RES_MODE)) {
    Serial.println(F("BH1750 Advanced begin"));
  } else {
    Serial.println(F("Error initialising BH1750"));
  }

}

int      head  = 0, tail = -10; // Index of first 'on' and 'off' pixels
uint32_t color = 0x010101;      // 'On' color (starts red)

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
  
  if(Serial.available())
  {
    char charText = Serial.read();
    if( charText == 'i')
    {
      myNeopixel->pickOneLED(0, myNeopixel->strip->Color(0, 255, 0), 50, 50);
      if (lightMeter.measurementReady()) 
      {
        float lux = lightMeter.readLightLevel();
        //Serial.printf("%.2f\n", lux);
        if(lux > 0)
        {
          Serial.println(lux);
          delay(1);
        }
      }        
    }
    else if(charText == 'o')
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
    else if(charText == 'c')
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
  




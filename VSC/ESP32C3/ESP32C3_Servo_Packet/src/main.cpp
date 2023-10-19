#include <Arduino.h>
#include <ESP32Servo.h>
//#include <esp_now.h>
#include <Wire.h>
#include "neopixel.h"

//#define M5STAMP_C3
#define ATOM_LITE

#define INTERVAL 50

#ifdef M5STAMP_C3
#define SERVO_PIN 1
#define HALL_SENSOR_PIN 4
#endif
#ifdef ATOM_LITE
#define SERVO_PIN 19
#define HALL_SENSOR_PIN 33
#endif

#define SERVO_INITIAL_POS 0
#define SERVO_TARGET_POS 150

#ifdef ATOM_LITE
#define HALL_MID_VALUE 2400
#define HALL_TARGET_VALUE 960
#endif 

#ifdef M5STAMP_C3
#define HALL_MID_VALUE 2900
#define HALL_TARGET_VALUE 1280
#endif

#define HALL_FAR     0x00
#define HALL_NEARBY  0x04
#define HALL_ARRIVED 0x05
#define SERVO_CLOSED 0x06
#define SERVO_OPENED 0x00


typedef struct __attribute__((packed)) packet
{
  uint8_t stx;
  uint8_t servoState;
  uint8_t hallState;
  uint8_t etx;
}PACKET;

PACKET dataToSend = {0,};
PACKET buf;

long count = 0;
int hallCount = 0;
int pos = 0;
uint64_t lastTime = 0;
uint16_t hallValue;

MyNeopixel* myNeopixel = new MyNeopixel();
Servo gripperServo;

void initPacket(PACKET* _packet);
bool sendPacket(uint8_t* _data, size_t len);
void initServo();
void rotateServo(int targetPos);

void setup() {
  Serial.begin(115200);
  myNeopixel->InitNeopixel();
  initPacket(&dataToSend);
  initServo();
}

void loop() {
  if(millis() - lastTime > INTERVAL)
  {
    lastTime = millis();
    
    hallValue = analogRead(HALL_SENSOR_PIN);
    //Serial.printf("Value : %d\r\n", hallValue);
    if (hallValue <= HALL_TARGET_VALUE)
    {
      hallCount++;
      if(hallCount > 10)
      {
        //Serial.println("Arrived at Target Height");
        dataToSend.hallState = HALL_ARRIVED;
      }   
    }
    else if(hallValue <= HALL_MID_VALUE)
    {
      dataToSend.hallState = HALL_NEARBY;
    }
    else    
    {
      hallCount = 0;
      dataToSend.hallState = HALL_FAR;
    }
    

    //Serial.printf("%d %d | HALL : %d\r\n", dataToSend.servoState, dataToSend.hallState, hallValue);
  }


  if(Serial.available())
  {
    char c = Serial.read();
    switch (c)
    {
    case 'i':      
      //Serial.write('a');
      //initPacket(&dataToSend);
      sendPacket((uint8_t*)&dataToSend, sizeof(dataToSend));
      //Serial.write(dataToSend);
      break;
    case 'o':
      if(dataToSend.hallState == HALL_ARRIVED)
      {
        rotateServo(SERVO_INITIAL_POS);
        dataToSend.servoState = SERVO_OPENED;
        sendPacket((uint8_t*)&dataToSend, sizeof(dataToSend));
        delay(100);
      }

      break;
    case 'c':
      if(dataToSend.hallState == HALL_ARRIVED)
      {
        rotateServo(SERVO_TARGET_POS);
        dataToSend.servoState = SERVO_CLOSED;
        sendPacket((uint8_t*)&dataToSend, sizeof(dataToSend));
        delay(100);
      }
      break;
    
    default:
      break;
    }
    delay(1);
  }
}



void initPacket(PACKET* _packet)
{
  _packet->stx = 0x02;
  _packet->etx = 0x03;
}


bool sendPacket(uint8_t* _data, size_t len)
{

  for (int i = 0; i < sizeof(buf); i++)
  {
    //Serial.printf("0x%x \r\n", _data[i]);
    Serial.write(_data[i]);
  }
  

  return true;
}


void initServo()
{
  if(!gripperServo.attached())
  {
    gripperServo.setPeriodHertz(50);
    gripperServo.attach(SERVO_PIN, 1000, 2000);
  }
  gripperServo.write(0);

}

void rotateServo(int targetPos)
{
      if (pos != targetPos)
      {
        //Serial.print("Servo Rotate Start\r\n");

        if(pos < targetPos)
        {
          myNeopixel->pickOneLED(0, myNeopixel->strip->Color(0, 255, 0), 50, 50);
          for (int i = 0; i <= targetPos; i++)
          {
            gripperServo.write(i);
            pos = i;
            //Serial.printf("Degree : %d\r\n", i);
            delay(4);
          }  
        }
        else if (pos > targetPos)
        {
          myNeopixel->pickOneLED(0, myNeopixel->strip->Color(255, 0, 0), 50, 50);
          for (int i = pos; i >= targetPos; i--)
          {
            gripperServo.write(i);
            pos = i;
            //Serial.printf("Degree : %d\r\n", i);
            delay(4);
          }    
        }
              
        //Serial.printf("Servo Rotated\r\n");
      }
}
#include <Arduino.h>
#include <Wire.h>
#include <AS5600.h>


#define EVENT_COUNT 3
#define INVALID_EVENT -1


const int motorPinPhase_A = 2;
const int motorPinPhase_B = 3;
const int motorPinPhase_C = 4;

const int potentioMeter = 5;

const int motorPoles = 14;

int SPWM_A = 0;
int SPWM_B = 120;
int SPWM_C = 240;

int torque = 100;

int rPosition = 0;
int ePosition = 0;
int rotationDirection = 0;

float PID_P = 0;
float PID_D = 0;
float Kp = 25;
float kd = 32;
float PID_setpoint = 0;
float PID_error = 0;

float previousPID_error = 0;
float elapsedTime, currentTime, previousTime;
int PID_Torque = 0;



enum EventID
{
  EVENT_ONE,
  EVENT_TWO,
  EVENT_THREE
};

uint32_t interval = 1000;
uint32_t lastTime = 0;
bool idleSwitch = false;
typedef void (*EventHandler)(void);


void handleEventOne()
{
  Serial.println("Event One Handled");
  interval = 100;
}

void handleEventTwo()
{
  Serial.println("Event Two Handled");
  interval = 500;
}

void handleEventThree()
{
  Serial.println("Event Three Handled");
  interval = 2000;
}

void handleInvalidEvent()
{
  Serial.println("Invalid Event");
  interval = 10;
}

// 이벤트 처리
void processEvent(int eventID, EventHandler handlers[])
{
  if(eventID >= 0 && eventID < EVENT_COUNT && handlers[eventID])
  {
    handlers[eventID]();
  }
  else
  {
    handleInvalidEvent(); // 추가
  }
}



EventHandler handlers[EVENT_COUNT] = {handleEventOne, handleEventTwo, handleEventThree};
int myEvents[] = {EVENT_ONE, EVENT_TWO, EVENT_THREE, INVALID_EVENT};
int eventSize = sizeof(myEvents)/ sizeof(myEvents[0]);

AS5600 as5600;

void setup() {
  Serial.begin(115200);
  pinMode(LED_BUILTIN, OUTPUT);

  pinMode(potentioMeter, INPUT);
  pinMode(motorPinPhase_A, OUTPUT);
  pinMode(motorPinPhase_B, OUTPUT);
  pinMode(motorPinPhase_C, OUTPUT);
  

  delay(1000);
  Serial.println("Start FOC");
  Wire.begin();

  // if(as5600.begin(4))
  // {
    
  //   as5600.setDirection(AS5600_CLOCK_WISE);  //  default, just be explicit.
  //   // int b = as5600.isConnected();
  //   // Serial.print("Connect: ");
  //   // Serial.println(b);
  //   Serial.println("AS5600 Connected");
  // }
  // else
  // {
  //   Serial.println("AS5600 Not Connected");
  // }
  
  delay(1000);

}

void loop() {

  uint32_t currentTime = millis();

  // 이부분의 동작에 따라 바뀜
  if(Serial.available())
  {
    uint8_t ch = Serial.read();
    if(ch != 0)
    {
      switch (ch)
      {
      case '1':
        processEvent(myEvents[0], handlers);
        break;
      case '2':
        processEvent(myEvents[1], handlers);
        break;
      case '3':
        processEvent(myEvents[2], handlers);
        break;
      default:
        processEvent(myEvents[3], handlers);
        break;
      }

    }
  }

  if(currentTime - lastTime >= interval)
  {
    lastTime = currentTime;
    digitalWrite(LED_BUILTIN, !digitalRead(LED_BUILTIN));
  }


  PID_setpoint = analogRead(potentioMeter);
  PID_setpoint = map(PID_setpoint, 0, 1023, 0, 360);




}


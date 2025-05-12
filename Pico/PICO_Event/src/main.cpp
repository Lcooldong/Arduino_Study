#include <Arduino.h>

#define EVENT_COUNT 3
#define INVALID_EVENT -1

enum EventID
{
  EVENT_ONE,
  EVENT_TWO,
  EVENT_THREE
};

uint32_t interval = 1000;
uint32_t lastTime = 0;
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

void setup() {
  Serial.begin(115200);
  pinMode(LED_BUILTIN, OUTPUT);
  
  
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
        // handlers[0]();
        break;
      case '2':
        processEvent(myEvents[1], handlers);
        // handlers[1]();
        break;
      case '3':
        processEvent(myEvents[2], handlers);
        // handlers[2]();
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

}


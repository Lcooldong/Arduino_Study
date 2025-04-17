#include <MightyZap.h>
#include "HardwareSerial.h"


#define UART2_DE      PA1
#define UART2_TX      PA2
#define UART2_RX      PA3

#define SERIAL_TX_BUFFER_SIZE 1024
#define SERIAL_RX_BUFFER_SIZE 1024

#define DEVICE_ID     0
#define LINEAR_ID     0

#define LED_BUILTIN   PC6
#define LED_INTERVAL  100

#define LINEAR_INITIAL_POSITION 0
#define LINEAR_RELEASE_POSITION 300
#define LINEAR_PUSH_POSITION 2800

#define LINEAR_MAX_POSITION 3600
// #define LINEAR_MAX_SPEED 1023
#define LINEAR_MAX_SPEED 512


uint32_t lastMillis[2] = {0,};
bool led_state = false;
uint32_t count = 0;
bool while_recv = false;

HardwareSerial UART2(UART2_RX, UART2_TX);
Mightyzap linearServo(&UART2, UART2_DE);


void setup() {
  Serial.begin(115200);
  linearServo.begin(32);
  linearServo.GoalSpeed(LINEAR_ID, LINEAR_MAX_SPEED);
  linearServo.GoalPosition(LINEAR_ID, LINEAR_INITIAL_POSITION);

  pinMode(LED_BUILTIN, OUTPUT);

}

void loop() {
  uint32_t curMillis = millis();
  if(curMillis - lastMillis[0] >= LED_INTERVAL)
  {
    lastMillis[0] = curMillis;
    while_recv = false;
    digitalWrite(LED_BUILTIN, led_state);
    led_state = !led_state;
    count++;
    if(count % 2 == 0)
    {
      linearServo.ledOn(LINEAR_ID, RED);
    }
    else
    {
      linearServo.ledOn(LINEAR_ID, GREEN);
    }
    Serial.printf("[%d] : 0x%02X\r\n", count, linearServo.ping(LINEAR_ID));
    linearServo.ping(LINEAR_ID);
  }
  else
  {
    while_recv = true;
  }

  if(Serial.available() > 0)
  {
    int text = Serial.read();
    switch(text)
    {
      case '1':
        linearServo.GoalPosition(LINEAR_ID, LINEAR_PUSH_POSITION);
        Serial.printf("PUSH\r\n");
      break;
      case '2':
        linearServo.GoalPosition(LINEAR_ID, LINEAR_RELEASE_POSITION);
        Serial.printf("RELEASE\r\n");
      break;

    }
  }

}

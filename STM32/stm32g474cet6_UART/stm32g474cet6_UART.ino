#include "HardwareSerial.h"

#define UART2_TX      PA2
#define UART2_RX      PA3
#define LED_BUILTIN   PC6
#define LED_INTERVAL  100

uint32_t lastMillis[2] = {0,};
bool led_state = false;
uint32_t count = 0;

HardwareSerial RS485(UART2_RX, UART2_TX);

void setup() {
  Serial.begin(115200);
  RS485.begin(115200);
  pinMode(LED_BUILTIN, OUTPUT);

}

void loop() {
  uint32_t curMillis = millis();
  if(curMillis - lastMillis[0] >= LED_INTERVAL)
  {
    lastMillis[0] = curMillis;
    digitalWrite(LED_BUILTIN, led_state);
    led_state = !led_state;
    count++;
    RS485.printf("CNT : %d\n\r", count);
  }
}

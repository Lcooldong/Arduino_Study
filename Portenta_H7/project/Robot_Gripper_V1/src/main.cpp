#include <Arduino.h>
#include <RPC.h>
#include <Queue.h>

#define RS485          Serial1 
#define RS485_BAUDRATE 57600

uint32_t preMillis[2] = {0,};
uint8_t leds[] = {LEDR, LEDG, LEDB};

uint8_t ledrValue = 0;

void setup() {
  Serial.begin(115200);
  RS485.begin(RS485_BAUDRATE);

  for (int i = 0; i < sizeof(leds); i++)
  {
    pinMode(leds[i], OUTPUT);
  }
  

  RPC.begin();

#ifdef CORE_CM7
  ledrValue = 10;
  analogWrite(leds[1], ledrValue);  
#endif

#ifdef CORE_CM4
#endif
  

  for (uint32_t i = 0; i < sizeof(preMillis)/sizeof(preMillis[0]); i++)
  {
    preMillis[i] = millis();
  }
  
}

void loop() {

#ifdef CORE_CM7
 
  uint32_t curMillis = millis();
  if(curMillis - preMillis[0] >= 10)
  {
    preMillis[0] = curMillis;

    ledrValue++;
    analogWrite(leds[1], ledrValue);
    Serial.println(ledrValue);
  }
#endif

#ifdef CORE_CM4

#endif
  
}


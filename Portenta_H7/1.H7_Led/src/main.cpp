#include <Arduino.h>
#include <RPC.h>

int myLED;
uint64_t lastTime = 0;
bool ledState = false;
int count = 0;
uint64_t cntLastTime =0;

void setup() {
  // randomSeed(analogRead(0));
  Serial.begin(115200);
  RPC.begin();
  #ifdef CORE_CM7
    bootM4();
    myLED =LEDB;
    
  #endif

  #ifdef CORE_CM4
    myLED = LEDG;
  #endif
  pinMode(myLED, OUTPUT);

  Serial.println("---- Start PortentaH7 ----");
}

void loop() {
  if(millis() - lastTime > 500)
  {
    digitalWrite(myLED, ledState);
    ledState = !ledState;
    lastTime = millis();
  }
  if(millis() - cntLastTime > 1000)
  {
    count++;
  #ifdef CORE_CM7
    Serial.print("M7 : ");
    Serial.println(count);
  #endif

  #ifdef CORE_CM4
    RPC.print("M4 : ");
    RPC.println(count);
  #endif

    cntLastTime = millis();
  }
  #ifdef CORE_CM7
  if(RPC.available())
  {
    Serial.write(RPC.read());
  }
  #endif
}


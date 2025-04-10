#include <Dynamixel2Arduino.h>
#include <MightyZap.h>
#include <Wire.h>
#include <ADS1115_WE.h>
#include <HardwareSerial.h>
#include "can.h"

#define I2C_ADDRESS 0x48
#define I2C1_SDA    PB7
#define I2C1_SCL    PA15
#define LED_BUILTIN PC6

uint32_t curMillis = 0;
uint32_t lastMillis[4] = {0,};
uint32_t led_value =0;
uint8_t led_direction = 0;
float voltage = 0.0;

enum{
  CAN_MODE = 0b0000'0000,
  ADS_MODE = 0b0000'0010
}Mode;

uint8_t select_mode = 0x00; 

ADS1115_WE adc = ADS1115_WE(I2C_ADDRESS);

bool ads1115_init();

void setup() {
  Serial.begin(115200);
  pinMode(LED_BUILTIN, OUTPUT); // PC6
  
}

void loop() {
  uint32_t curMillis = millis();
  
  switch (select_mode) {
    case ADS_MODE:
    
    break;
  }


  if(curMillis - lastMillis[0] >= 100)
  {
    lastMillis[0] = curMillis;

  }
  if(curMillis - lastMillis[0] >= 100)
  {
    lastMillis[0] = curMillis;
    
  }
  if(curMillis - lastMillis[0] >= 100)
  {
    lastMillis[0] = curMillis;
    
  }
  if(curMillis - lastMillis[0] >= 100)
  {
    lastMillis[0] = curMillis;
    voltage = adc.getResult_V();

    Serial.printf("Result => %0.2f [%d]\r\n", voltage, adc.getRawResult());
  }

}


bool ads1115_init()
{
  Wire.setSDA(I2C1_SDA);
  Wire.setSCL(I2C1_SCL);
  Wire.begin();
  if(!adc.init()){
    Serial.println("ADS1115 not connected!");
    return false;
  }
  else
  {
    Serial.println("ADS1115 connected!");
    adc.setVoltageRange_mV(ADS1115_RANGE_6144);
    adc.setCompareChannels(ADS1115_COMP_0_GND); // select channel  0 ~ 3
    adc.setMeasureMode(ADS1115_CONTINUOUS);
    return true;
  }
}
#include<ADS1115_WE.h> 
#include<Wire.h>

#define I2C_ADDRESS 0x48
#define I2C1_SDA    PB7
#define I2C1_SCL    PA15
#define LED_BUILTIN PC6

uint32_t curMillis = 0;
uint32_t lastMillis[2] = {0,};
uint32_t led_value =0;
uint8_t led_direction = 0;
float voltage = 0.0;


ADS1115_WE adc = ADS1115_WE(I2C_ADDRESS);

void breate(uint32_t interval);

void setup() {
  Serial.begin(115200);
  delay(3000);
  pinMode(LED_BUILTIN, OUTPUT); // PC6
  // Wire.setPins()
  Wire.setSDA(I2C1_SDA);
  Wire.setSCL(I2C1_SCL);
  Wire.begin();
  if(!adc.init()){
    Serial.println("ADS1115 not connected!");
  }
  else
  {
    Serial.println("ADS1115 connected!");
    adc.setVoltageRange_mV(ADS1115_RANGE_6144);
    adc.setCompareChannels(ADS1115_COMP_0_GND); // select channel  0 ~ 3
    adc.setMeasureMode(ADS1115_CONTINUOUS);
  }

}

void loop() {
  curMillis = millis();
  if(curMillis - lastMillis[1] >= 100)
  {
    lastMillis[1] = curMillis;
    // adc.setCompareChannels(ADS1115_COMP_0_GND);
    voltage = adc.getResult_V();
     
    Serial.printf("Result => %0.2f [%d]\r\n", voltage, adc.getRawResult());
  }
  
  breate(5);
}




void breate(uint32_t interval)
{
  if(curMillis - lastMillis[0] >= interval)
  {
    lastMillis[0] = curMillis;
    analogWrite(LED_BUILTIN, led_value);
    
    if(led_value >= 255)
    {
      led_direction = 1;
      led_value = 255;
    }
    else if(led_value == 0)
    {
      led_direction = 0;
    }

    if(led_direction == 0)
    {
      led_value++;
    }
    else if (led_direction == 1)
    {
      led_value--;
    }
  }
}

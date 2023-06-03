#include <Arduino.h>
#define BUILTIN_LED 2
#define LEDC_CHANNEL_0 0
#define LEDC_BASE_FREQ 5000
#define LEDC_TIMER_13_BIT 13
#define ANALOG_LED
//#define DEGITAL_LED

bool isOn = false;
int brightness = 0;
int fadeAmount = 5;

void ledcAnalogWrite(uint8_t channel, uint32_t value, uint32_t valueMax);

void setup() {
  Serial.begin(115200);
  //pinMode(BUILTIN_LED, OUTPUT);
  ledcSetup(LEDC_CHANNEL_0, LEDC_BASE_FREQ, LEDC_TIMER_13_BIT);
  ledcAttachPin(BUILTIN_LED, LEDC_CHANNEL_0);


}

void loop() {
  Serial.println("Hello");

#ifdef DIGITAL_LED
  if(isOn)
  {
    digitalWrite(BUILTIN_LED, HIGH);
    isOn = false;
    delay(1000);
  }
  else
  {
    digitalWrite(BUILTIN_LED, LOW);
    isOn = true;
    delay(1000);
  }
#endif

#ifdef ANALOG_LED
  ledcAnalogWrite(LEDC_CHANNEL_0, brightness, 255);
  brightness += fadeAmount;
  if(brightness <= 0 || brightness >= 255)
  {
    fadeAmount = -fadeAmount;
  }
  delay(50);
#endif

}



void ledcAnalogWrite(uint8_t channel, uint32_t value, uint32_t valueMax = 255)
{
  uint32_t duty = (8191/ valueMax) * min(value, valueMax);
  ledcWrite(channel, duty);
}

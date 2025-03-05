#include <Arduino.h>

// DFROBOT Sonar SEN0208  분리형 35cm 이하만 잘잡아냄 일체형 40cm


#define MIN_DISTANCE 25
#define MAX_DISTANCE 250

const int ECHO_PIN = 4;
const int TRIG_PIN = 5;

enum
{
  SET_INIT = 0,
  SET_TRIG = 1,
  SET_WAIT = 2,
};

bool led_toggle = false;
int trig_toggle = SET_INIT;
uint32_t pre_millis[2] = {0,};

uint32_t cur_millis = 0;
uint32_t cur_micros = 0;
uint32_t sonar_micros = 0;

uint32_t distance = 0;
int count = 0;

void trigSonar(int pin);

void setup() {
  Serial.begin(115200);
  pinMode(LED_BUILTIN, OUTPUT);
  pinMode(ECHO_PIN, INPUT);
  pinMode(TRIG_PIN, OUTPUT);
  

  for (uint32_t i = 0; i < sizeof(pre_millis)/sizeof(pre_millis[0]); i++)
  {
    pre_millis[i] = millis();
    
  }
  sonar_micros = millis();
}

void loop() {
  cur_millis = millis();
  cur_micros = micros();
  if(cur_millis - pre_millis[0] >= 1000)
  {
    pre_millis[0] = cur_millis;
    digitalWrite(LED_BUILTIN, led_toggle);
    led_toggle = !led_toggle;
  }
  trigSonar(TRIG_PIN);
  
  // digitalWrite(TRIG_PIN, LOW); // Set the trigger pin to low for 2uS
  // delayMicroseconds(2);
  // digitalWrite(TRIG_PIN, HIGH); // Send a 10uS high to trigger ranging
  // delayMicroseconds(20);
  // digitalWrite(TRIG_PIN, LOW); // Send pin low again
  // distance = pulseIn(ECHO_PIN, HIGH)/58; // Read in times pulse
  // //Serial.println(distance);
  // Serial.print("[");
  // Serial.print(count);
  // Serial.print("] : ");
  // Serial.print(distance);
  // Serial.println("   cm");
  // count++;
  // delay(50);
}

void trigSonar(int pin)
{
  switch (trig_toggle) 
  {
    case SET_INIT:
      if (cur_micros - sonar_micros >= 2) // Ensure SET_INIT waits correctly
      {
        sonar_micros = cur_micros;
        trig_toggle = SET_TRIG;
        digitalWrite(pin, LOW);

      }
      break;

    case SET_TRIG:
      if (cur_micros - sonar_micros >= 10)
      {
        sonar_micros = cur_micros;
        trig_toggle = SET_WAIT;
        digitalWrite(pin, HIGH);

      }
      break;

    case SET_WAIT:
      if (cur_micros - sonar_micros >= 75000) 
      {
        sonar_micros = cur_micros;
        trig_toggle = SET_INIT;
        digitalWrite(pin, LOW);
        distance = pulseIn(ECHO_PIN, HIGH ) / 58;   // TIMEOUT

        if(distance >= MAX_DISTANCE)
        {
          distance = MAX_DISTANCE;
        }
        else if(distance <= MIN_DISTANCE)
        {
          distance = MIN_DISTANCE;
        }
        Serial.print("Distance: ");
        Serial.print(distance);
        Serial.println(" cm");
      }
      break;
  }
}


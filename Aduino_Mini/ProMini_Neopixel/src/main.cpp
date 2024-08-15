#include <Arduino.h>
#include <Adafruit_NeoPixel.h>

#define LED_NUM 20
#define LED_PIN 6
#define BUILTIN_LED 13

uint64_t breathingTime = 0;
int breathingValue = 0;
bool breathingDirection = true;

uint64_t lastTime = 0;
uint64_t blinkTime =0;
bool ledState = false;

Adafruit_NeoPixel* strip = new Adafruit_NeoPixel(LED_NUM, LED_PIN, NEO_GRB + NEO_KHZ800);

void pickOneLED(uint8_t ledNum, uint32_t color, uint8_t brightness, int wait);
void breathe(uint8_t _delay);
void blink(uint16_t _delay);

void setup() {
  Serial.begin(115200);
  pinMode(BUILTIN_LED, OUTPUT);

  strip->begin();

  for (int i = 0; i < LED_NUM; i++)
  {
    pickOneLED(i, strip->Color(0, 0, 0), 0, 1);
  }
  
}

void loop() {
  
  int var = analogRead(A1);
  int _brighness = map(var, 0, 1023, 0, 200);

  if(millis() - lastTime > 100)
  {
    
    Serial.println(_brighness);
    for (int i = 0; i < LED_NUM; i++)
    {
      pickOneLED(i, strip->Color(255, 255, 255), _brighness, 1);
    }
    lastTime = millis();
  }



  blink(2000);
  // breathe(5);
}


void pickOneLED(uint8_t ledNum, uint32_t color, uint8_t brightness, int wait){
    strip->setBrightness(brightness);
    strip->setPixelColor(ledNum, color);  
    strip->show();                                               
    delay(wait);
}

void breathe(uint8_t _delay)
{
  if(millis() - breathingTime > _delay)
  {
    breathingTime = millis();
    if(breathingDirection == true)
    {
      breathingValue++;
    }
    else
    {
      breathingValue--;
    }

    if(breathingValue >= 255)
    {
      breathingDirection = false;
    }
    else if(breathingValue <= 0) 
    {
      breathingDirection = true;
    }

    analogWrite(LED_BUILTIN, breathingValue);

  }
}

void blink(uint16_t _delay)
{
  if(millis() - blinkTime > _delay)
  {
    digitalWrite(LED_BUILTIN, ledState);
    ledState = !ledState;
    blinkTime = millis();
  }

  
  
}
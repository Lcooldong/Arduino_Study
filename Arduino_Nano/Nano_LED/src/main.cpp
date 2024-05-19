#include <Arduino.h>
#include <Adafruit_NeoPixel.h>

#define LED_NUM 20
#define LED_PIN 6

Adafruit_NeoPixel* strip = new Adafruit_NeoPixel(LED_NUM, LED_PIN, NEO_GRB + NEO_KHZ800);

void pickOneLED(uint8_t ledNum, uint32_t color, uint8_t brightness, int wait);

void setup() {
  Serial.begin(115200);
  strip->begin();
  for (int i = 0; i < LED_NUM; i++)
  {
    pickOneLED(i, strip->Color(0, 0, 0), 0, 1);
  }
  
}

void loop() {
  
  int var = analogRead(A1);
  int _brighness = map(var, 0, 1023, 0, 200);
  Serial.println(_brighness);
  for (int i = 0; i < LED_NUM; i++)
  {
    pickOneLED(i, strip->Color(255, 255, 255), _brighness, 1);
  }
  delay(100);
}


void pickOneLED(uint8_t ledNum, uint32_t color, uint8_t brightness, int wait){
    strip->setBrightness(brightness);
    strip->setPixelColor(ledNum, color);  
    strip->show();                                               
    delay(wait);
}
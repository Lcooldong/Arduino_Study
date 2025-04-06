#include <Adafruit_NeoPixel.h>

#define LED_INTERVAL 5
#define PIN          PB6
#define NUMPIXELS    23

uint32_t lastMillis[2] = {0,};
uint32_t led_value =0;
uint8_t led_direction = 0;
uint8_t neopixel_position = 0;   // start point
uint8_t neopixel_dir = 0;

Adafruit_NeoPixel pixels(NUMPIXELS, PIN, NEO_GRB + NEO_KHZ800);

void setup() {
  Serial.begin(115200);
  pinMode(LED_BUILTIN, OUTPUT); // PC6
  pixels.begin();
  pixels.clear();
  pixels.setBrightness(20);
  pixels.setPixelColor(0, pixels.Color(255, 0, 0));
  pixels.show();
}

void loop() {
  uint32_t curMillis = millis();

  if(curMillis - lastMillis[0] >= LED_INTERVAL)
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

  if(curMillis - lastMillis[1] >= 10)
  {
    lastMillis[1] = curMillis;
    
  

    Serial.printf("%d\r\n", neopixel_position);
    if(neopixel_dir == 1)
    {     
      pixels.setPixelColor(neopixel_position, pixels.Color(0, 0, 255));

      if(neopixel_position == 0)
      {
        neopixel_dir = 0;
      }
      else
      {
        neopixel_position--;
      }
    }
    else if(neopixel_dir == 0)
    {
      pixels.setPixelColor(neopixel_position, pixels.Color(0, 150, 0));
      
      if(neopixel_position >= NUMPIXELS - 1)
      {
        neopixel_dir = 1;
      }
      else
      {
        neopixel_position++;
      }
    }
    
    pixels.show();
  }


}

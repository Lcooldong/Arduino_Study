#include "BluetoothSerial.h"
#include <Adafruit_NeoPixel.h>

#ifdef __AVR__
 #include <avr/power.h> // Required for 16 MHz Adafruit Trinket
#endif

#if !defined(CONFIG_BT_ENABLED) || !defined(CONFIG_BLUEDROID_ENABLED)
 #error Bluetooth is not enabled! Please run `make menuconfig` to and enable it
#endif

#define LED_PIN 13
#define VCC 12
#define LED_COUNT 30
uint8_t current_state = 0;

BluetoothSerial SerialBT;
Adafruit_NeoPixel strip(LED_COUNT, LED_PIN, NEO_GRB + NEO_KHZ800);


////////////////////////////setup///////////////////////////////////

void setup() {
  pinMode(VCC,OUTPUT);
  digitalWrite(VCC,HIGH);
  Serial.begin(115200);
  SerialBT.begin("ESP32LED"); //Bluetooth device name
  Serial.println("The device started, now you can pair it with bluetooth!");
  
#if defined(__AVR_ATtiny85__) && (F_CPU == 16000000)
  clock_prescale_set(clock_div_1);
#endif

  strip.begin();           // INITIALIZE NeoPixel strip object (REQUIRED)
  strip.show();            // Turn OFF all pixels ASAP
  strip.setBrightness(50); // Set BRIGHTNESS to about 1/5 (max = 255)
  colorWipe(strip.Color(0, 0, 0), 0);
}


////////////////////////////loop/////////////////////////////////
void loop() {
  if (Serial.available()) {
    SerialBT.write(Serial.read());
  }
  while (SerialBT.available()) {
    uint8_t character = SerialBT.read();
    Serial.write(character);
    switch(character){
      case 'a':
      case 'b':
      case 'c':
      case 'd':
      case 'e':
      case 'f':
      case 'g':
      case 'h':
        current_state = character;
        break;  
    }
  }
  switch(current_state){
    case 'a':
      colorWipe(strip.Color(255,   0,   0), 20);    // Red
      break;
    case 'b':
      colorWipe(strip.Color(  0, 255,   0), 20);    // Green
      break;
    case 'c':
      colorWipe(strip.Color(  0,   0, 255), 20);    // Blue
      break;
    case 'd':
      theaterChase(strip.Color(255,   0,   0), 20); // Red
      break;
    case 'e':
      theaterChase(strip.Color(  0, 255,   0), 20); // Green
      break;
    case 'f':
      theaterChase(strip.Color(255,   0, 255), 20); // Cyan
      break;
    case 'g':
      rainbow(10);
      break;
    case 'h':
      theaterChaseRainbow(20);
      break;
  }
}

//////////////////////////////////function/////////////////////////////////

void colorWipe(uint32_t color, int wait) {
  for(int i=0; i<strip.numPixels(); i++) { // For each pixel in strip...
    strip.setPixelColor(i, color);         //  Set pixel's color (in RAM)
    strip.show();                          //  Update strip to match
    delay(wait);                           //  Pause for a moment
  }
}

void theaterChase(uint32_t color, int wait) {
  for(int a=0; a<10; a++) { 
    for(int b=0; b<3; b++) { 
      strip.clear();         
      for(int c=b; c<strip.numPixels(); c += 3) {
        strip.setPixelColor(c, color); 
      }
      strip.show(); 
      delay(wait);  
    }
  }
}

void rainbow(int wait) {
  for(long firstPixelHue = 0; firstPixelHue < 5*65536; firstPixelHue += 256) {
    for(int i=0; i<strip.numPixels(); i++) { 
      int pixelHue = firstPixelHue + (i * 65536L / strip.numPixels());
      strip.setPixelColor(i, strip.gamma32(strip.ColorHSV(pixelHue)));
    }
    strip.show(); 
    delay(wait);  
  }
}

void theaterChaseRainbow(int wait) {
  int firstPixelHue = 0;     
  for(int a=0; a<30; a++) {  
    for(int b=0; b<3; b++) { 
      strip.clear();        
      for(int c=b; c<strip.numPixels(); c += 3) {   
        int      hue   = firstPixelHue + c * 65536L / strip.numPixels();
        uint32_t color = strip.gamma32(strip.ColorHSV(hue)); 
        strip.setPixelColor(c, color); 
      }
      strip.show();               
      delay(wait);                 
      firstPixelHue += 65536 / 90; 
    }
  }
}

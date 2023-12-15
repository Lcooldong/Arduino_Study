#include <Arduino.h>


int count = 0;
void setup() {
  Serial.begin(115200);
  pinMode(LED_BUILTIN, OUTPUT);
}

void loop() {

  Serial.printf("%d : This is ATtiny85 !\r\n", count++);
  delay(1000);
  for (int i = 0; i < 255; i++)
  {
    analogWrite(LED_BUILTIN, i);
    delay(5);
  }
  
  

}


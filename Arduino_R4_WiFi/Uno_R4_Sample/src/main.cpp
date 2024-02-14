#include <Arduino.h>
// #include <WiFiS3.h>
#define LED 13

void setup() {
  Serial.begin(115200);
  pinMode(LED_BUILTIN, OUTPUT);

}
int count = 0;
void loop() {
  Serial.print("ESP32S3");
  Serial.println(count++);
  digitalWrite(LED_BUILTIN, HIGH);
  delay(500);
  digitalWrite(LED_BUILTIN, LOW);
  delay(500);
}





#include <dht11.h>

dht11 DHT11;
#define DHT11PIN 4

void setup() {
  Serial.begin(9600);

}

void loop() {
  int Save = DHT11.read(DHT11PIN);
  Serial.print(DHT11.temperature);
  Serial.print(",");
  Serial.print(DHT11.humidity);
  Serial.print(".");

  delay(1000);
}

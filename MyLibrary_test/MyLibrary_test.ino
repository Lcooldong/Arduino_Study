#include <MyLibrary.h>

void setup() {
  Serial.begin(9600);

}

void loop() {
  Serial.println(add(3, 4));
  delay(1000);
  Serial.println(subtract(5, 2));
  delay(1000);
}

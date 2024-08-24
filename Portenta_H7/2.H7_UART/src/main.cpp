#include <Arduino.h>
#include <Queue.h>

#define BAUDRATE 115200
#define INTERVAL 1000

uint64_t previousMillis = 0;
int count = 0;
char buffer[32];

char t1[] = "-[";
char t2[] = "]-";

uint8_t leds[] = {LEDR, LEDG, LEDB};




void setup() {
  Serial.begin(115200);
  // Serial1.begin(BAUDRATE);
  // Serial2.begin(BAUDRATE);
  // Serial3.begin(BAUDRATE);

  delay(3000);

  for (uint8_t i = 0; i < sizeof(leds); i++)
  {
    pinMode(leds[i], OUTPUT);
  }

  Serial.print("LED Size :");
  Serial.println(sizeof(leds));

  Serial.println("--UART TEST--");
}

void loop() {

  uint64_t currentMillis = millis();
  if(currentMillis - previousMillis > INTERVAL)
  {
    count++;
    digitalWrite(leds[count%3], HIGH);    // 0 1 2
    digitalWrite(leds[(count+1)%3], HIGH); // 1 2 0 
    digitalWrite(leds[(count+2)%3], LOW); // 2 0 1

    String text = "";

    text += "-[";
    text += (String)count;
    text += "]-";
    
    sprintf(buffer, "%s %3d %s", t1,  count, t2);

    Serial.print(text);
    Serial.print((String)"== [" + count + "] == ");
    Serial.println(buffer);
    // Serial.print(leds[count%3]);
    // Serial.print(" | ");
    // Serial.print(leds[(count+1)%3]);
    // Serial.print(" | ");
    // Serial.print(leds[(count+2)%3]);
    // Serial.print(" <= ");
    // Serial.println(count);


    // Serial1.print(count);
    // Serial2.print(count);
    // Serial3.print(count);

    previousMillis = currentMillis;
  }

}


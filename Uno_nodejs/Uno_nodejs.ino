#include<MsTimer2.h>

int LED = 13;
int sw = 7;
boolean bl = true;

void setup() {

  Serial.begin(9600);
  pinMode(LED, OUTPUT);
  pinMode(sw, INPUT_PULLUP);

  MsTimer2::set(2000, LED_blink);
  MsTimer2::start();
}

void loop() {
  if(digitalRead(sw)== 0){
    Serial.println("pushed");
    delay(200);  
  }
}

void LED_blink(){
    digitalWrite(LED, bl);
    if(bl == true){
      Serial.println("ON");
    }else{
      Serial.println("OFF");
    }
    bl = !bl;
}

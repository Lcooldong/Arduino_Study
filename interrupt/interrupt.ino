#include<MsTimer2.h>

#define LED 13
#define sw1 7
#define sw2 3 // 또는 2

boolean bl = true;

void setup() {
  Serial.begin(9600);
  pinMode(LED, OUTPUT);
  pinMode(sw1, INPUT_PULLUP);
  pinMode(sw2, INPUT_PULLUP);

  
  //0 -> digital 2번, 1-> digital 3번  고정임
  // 외부 하드웨어 쪽에 하면 좋음
  attachInterrupt(1, push_sw2, FALLING);
  
  //millisecond, function, 여러가지 동작할 때 좋음
  MsTimer2::set(500, led_blink);  //0.5초에 한번 작동
  MsTimer2::start();
  
}

void loop() {
  if(digitalRead(sw1)==0){
    Serial.println("sw1");
    delay(200);
  }
}



void led_blink(){
  digitalWrite(LED, bl);
  bl = !bl;
}


void push_sw2(){
  Serial.println("sw2");
}

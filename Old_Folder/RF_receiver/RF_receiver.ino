#include <SPI.h>
#include "RF24.h"

/*
  예제 248-1-1 2.4G RF모듈(nRF24L01)을 이용한 리시버예제!
 */


RF24 radio(9, 10); //CE, SS

uint8_t address[6] = "00001";

void setup() {
  // put your setup code here, to run once:
  Serial.begin(115200); //PC와 아두이노간 통신라인
  Serial.println("Receiver_Start");
  radio.begin(); //아두이노-RF모듈간 통신라인
  radio.setPALevel(RF24_PA_LOW); 
  radio.openReadingPipe(0, address);
  radio.startListening();
}

void loop() {
  // put your main code here, to run repeatedly:
  if (radio.available()){
    //RF무선모듈쪽으로 뭔가 수신된값이 존재한다면~
    char text[30];
    radio.read(text,sizeof(text));
    Serial.println(text);
   
  }
}

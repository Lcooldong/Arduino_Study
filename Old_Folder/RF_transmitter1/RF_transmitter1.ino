

/*
  예제 248-1-2 2.4G RF모듈(nRF24L01)을 이용한 트랜스미터 예제!
 */

#include <SPI.h>
#include "RF24.h"

RF24 radio(9, 10); //CE, SS

uint8_t address[6] = "00001";

void setup() {
  // put your setup code here, to run once:
  Serial.begin(115200); //PC와 아두이노간 통신라인
  radio.begin(); //아두이노-RF모듈간 통신라인
  radio.setPALevel(RF24_PA_LOW); 
  radio.openWritingPipe(address);
  radio.stopListening();
}

void loop() {
  // put your main code here, to run repeatedly:
  char text[] = "DEVICE1!!";
  radio.write(text,sizeof(text));
  delay(1000);
}

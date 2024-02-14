#include <SPI.h>
//#include <nRF24L01.h>
#include "RF24.h"
RF24 radio(9, 10); // SPI통신을 위한 (CE, CSN) 핀 선언
const byte address[6] = "00001"; // 송신기와 수신기가 동일한 값으로 주소 설정함(5자리)
void setup() {
  radio.begin();

  radio.setPALevel(RF24_PA_LOW); // 송신거리에 따른, 전원공급 파워레벨 설정
//(최소) RF24_PA_MIN / RF24_PA_LOW / RF24_PA_HIGH / RF24_PA_MAX (최대) 설정가능
//송신이 약하다고 판단될 경우 nRF24모듈의 GND와 3.3V 단자에 전해콘덴서(10uF이상:+를3.3V연결)사용권장
  radio.openWritingPipe(address); // 데이터를 보낼 수신 주소를 설정
  radio.stopListening();  // 모듈을 송신기로 설정
}
void loop() {
  const char text[] = "(From) nRF24 Tx : Hello World!"; // 송신할 문자
  radio.write(text, sizeof(text)); // 위 문자를 문자 길이 만큼 송출함
  delay(1000);
}

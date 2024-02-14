#include <SPI.h> 
//#include <nRF24L01.h>
#include "RF24.h"
RF24 radio(9, 10);     // SPI통신을 위한 (CE, CSN) 핀 선언
const byte address[6] = "00001"; // 송신기와 수신기가 동일한 값으로 주소 설정함(5자리)
//uint8_t adress2[6] = "10001";
void setup() {
  Serial.begin(115200);
  radio.begin();
  
  radio.setPALevel(RF24_PA_LOW); // 송신거리에 따른, 전원공급 파워레벨 설정
//(최소) RF24_PA_MIN / RF24_PA_LOW / RF24_PA_HIGH / RF24_PA_MAX (최대) 설정가능
//송신이 약하다고 판단될 경우 nRF24모듈의 GND와 3.3V 단자에 전해콘덴서(10uF이상:+를3.3V연결)사용권장
  radio.openReadingPipe(0, address);// 데이터를 받을 송신기 주소를 설정
  radio.startListening();   // 모듈을 수신기(상태)로 설정
}
void loop() {
  if (radio.available()) {
    char text[32] = "";   // 데이터를 수신 받을 변수 설정
    radio.read(text, sizeof(text)); // 수신되는 데이터 길이만큼 읽어 저장
    Serial.println(text);
  }
}

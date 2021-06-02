#include <SPI.h>
#include <nRF24L01.h>
#include <RF24.h>
RF24 radio(9, 10); 
const byte address[6] = "00001"; // 주소값을 부여합니다. 송신기와 수신기를 동일한 주소로 입력 해야합니다.
void setup() {
  radio.begin();
  radio.openWritingPipe(address); // 데이터를 보낼 수신의 주소를 설정합니다.
  radio.setPALevel(RF24_PA_MIN); // 전원공급에 관한 파워레벨을 설정합니다.
  radio.stopListening();  //모듈을 송신기로 설정합니다.
}
void loop() {
  const char text[] = "Hello World";
  radio.write(&text, sizeof(text)); //해당 메시지를 수신자에게 보냅니다.
  delay(1000);
}

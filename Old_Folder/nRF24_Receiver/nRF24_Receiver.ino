#include <SPI.h> 
#include <nRF24L01.h>
#include <RF24.h>
RF24 radio(9, 10);
int led_pin = 3;
const byte address[6] = "00001"; // 주소값을 부여합니다. 송신기과 수신기를 동일한 주소로 입력 해야합니다.
 
 
void setup() {
  pinMode(led_pin , OUTPUT);
  Serial.begin(9600);
  Serial.write("start");
  radio.begin();
  radio.openReadingPipe(0, address);
  radio.setPALevel(RF24_PA_MIN); //전원공급에 관한 파워레벨을 설정합니다.
  radio.startListening(); //모듈을 수신기로 설정합니다.
}
 
void loop() {
  if (radio.available()) {
    char text[32] = "";
    radio.read(&text, sizeof(text));
    Serial.println(text);
  digitalWrite(led_pin , HIGH);
  }
}

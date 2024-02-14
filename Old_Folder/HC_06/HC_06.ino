#include <SoftwareSerial.h> //시리얼통신 라이브러리 호출
 
int blueTx=2;   //Tx (보내는핀 설정)at
int blueRx=3;   //Rx (받는핀 설정)
SoftwareSerial mySerial(blueTx, blueRx);  //시리얼 통신을 위한 객체선언

void cmd(const char *msg) {
  mySerial.write(msg);
  unsigned long stime = millis();
  while((millis()-stime) < 1000) {
    if(mySerial.available()) {
      Serial.write(mySerial.read());
    }
  }
}


void setup() 
{
  Serial.begin(9600);   //시리얼모니터
  mySerial.begin(9600); //블루투스 시리얼
  Serial.println("Both NL & CR");
  Serial.println("Enter AT command : ");
  cmd("AT");
  cmd("AT+VERSION");
  cmd("AT+BAUD4");
  cmd("AT+NAMEbl_01");
  cmd("AT+PIN0000");
}

void loop()
{
  if (mySerial.available()) {       
    Serial.write(mySerial.read());  //블루투스측 내용을 시리얼모니터에 출력
  }
  if (Serial.available()) {         
    mySerial.write(Serial.read());  //시리얼 모니터 내용을 블루추스 측에 WRITE
  }
}

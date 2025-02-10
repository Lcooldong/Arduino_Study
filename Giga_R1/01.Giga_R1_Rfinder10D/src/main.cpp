#include <Arduino.h>
#include <RPC.h>
#include "Mecha_Rfinder10D.h"

#define RFINDER_SERIAL Serial3

uint32_t lastMillis = 0;
int colorState = 0;
enum
{
  RED = 0,
  GREEN =1,
  BLUE = 2,
  
};


Mecha_Rfinder10D rfinder;

void setup() {
  Serial.begin(115200);
  pinMode(LEDR, OUTPUT);
  pinMode(LEDG, OUTPUT);
  pinMode(LEDB, OUTPUT);

  // if(!rfinder.init(RFINDER_SERIAL))
  // {
  //   Serial.println("Init Error");
  //   while (true)
  //   {
  //     delay(100);
  //   }    
  // }
}

void loop() {

  uint32_t currentMillis = millis();
  if( currentMillis - lastMillis >= 500)
  {
    switch (colorState)
    {
    case RED:
      digitalWrite(LEDR, LOW);
      digitalWrite(LEDG, HIGH);
      digitalWrite(LEDB, HIGH);
      break;
    case GREEN:
      digitalWrite(LEDR, HIGH);
      digitalWrite(LEDG, LOW);
      digitalWrite(LEDB, HIGH);      
      break;
    case BLUE:
      digitalWrite(LEDR, HIGH);
      digitalWrite(LEDG, HIGH);
      digitalWrite(LEDB, LOW);        
      break;
    default:
      break;
    }

    lastMillis = currentMillis;
    colorState++;
    if(colorState >= 3)
    {
      colorState = 0;
    }
  }

  // rfinder.fetchNewData(); // Serial1의 데이터를 polling함으로, 항상 실행되어야 함.

  // if (rfinder.isNewData()) {                  // 마지막으로 .isNewData() 메소드 실행 후 새로운 데이터가 있다면,
  //   int rfinder_cnt = rfinder.getTargetCnt(); // 인식된 타겟 수 확인

  //   for (int i = 0; i < rfinder_cnt; i++) { // 타겟 수 만큼 출력
  //     Serial.print("Range : ");
  //     Serial.print(rfinder.getRange(i)); // .getRange() 메소드의 인자값으로 타겟 번호 전달
  //     Serial.print("\t\t");
  //   }
  //   Serial.println();
  // }  

}
  


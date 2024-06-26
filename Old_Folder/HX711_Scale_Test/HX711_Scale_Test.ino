//0.453592
#include "HX711.h"

// HX711 circuit wiring
// 디지털 2번 3번 핀 (pwm), 아날로그 A 로 2개
const int LOADCELL_DOUT_PIN = A2;  // esp32 아날로그 핀-> 0(GPIO 36), 3(39), 4(32), 5(33), 6(34), 7(35) 1(37)?2(38)?
const int LOADCELL_SCK_PIN = A3; //ADC2는 와이파이 사용시 사용X

HX711 scale;

void setup() {
  Serial.begin(115200);
  scale.begin(LOADCELL_DOUT_PIN, LOADCELL_SCK_PIN);
  Serial.println("start");
  scale.set_scale(700);    // 알고 있는 물건으로 무게를 보정하는 상수값
  scale.tare();             // 0점 조절, 초기화 (0 kg)
  
//  float factor = -7000;
//  float target_weight = 2000;
  
//  while(true){
//      //scale.set_scale control Calibration factor
//      scale.set_scale(factor);
//      float weight = scale.get_units()*(-0.453592);
//      Serial.println(weight);
//      if(weight <= target_weight){
//        break;  
//      }
//      else{
//        factor -= 100;  
//      }
//  }

//  Serial.print("보정값=");
//  Serial.println(factor);

  
}

void loop() {
  Serial.print("one reading:\t");
  Serial.println(scale.get_units()*(0.453592), 1);   //한번 읽기
  delay(500);

//  Serial.print("\t| average:\t");
//  Serial.println(scale.get_units(10)*0.453592, 1);   //  10개 평균 읽기
//  scale.power_down();             // put the ADC in sleep mode
//  delay(5000);
//  scale.power_up();
}

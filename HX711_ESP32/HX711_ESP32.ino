//0.453592
#include "HX711.h"


// HX711 circuit wiring
const int LOADCELL_DOUT_PIN = 2;
const int LOADCELL_SCK_PIN = 3;


HX711 scale;

void setup() {
  Serial.begin(115200);
  scale.begin(LOADCELL_DOUT_PIN, LOADCELL_SCK_PIN);
  scale.set_scale(-7000);    // 알고 있는 물건으로 무게를 보정하는 상수값
  scale.tare();                // 0점 조절, 초기화 (0 kg)

  float factor = -7000;

  float target_weight = 2000;
  
  while(true){
      //scale.set_scale control Calibration factor
      scale.set_scale(factor);
      float weight = scale.get_units()*(-0453592);
      Serial.println(weight);
      if(weight <= target_weight){
        break;  
      }
      else{
        factor -= 100;  
      }
  }

  Serial.print("보정값=");
  Serial.println(factor);

  
}

void loop() {
  Serial.print("one reading:\t");
  Serial.println(scale.get_units()*0.453592, 1);   //한번 읽기
  delay(500);

//  Serial.print("\t| average:\t");
//  Serial.println(scale.get_units(10)*0.453592, 1);   //  10개 평균 읽기
//  scale.power_down();             // put the ADC in sleep mode
//  delay(5000);
//  scale.power_up();
}

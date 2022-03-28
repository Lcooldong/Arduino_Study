//0.453592  1파운드 -> kg
// 0.45359237
// 1kg -> 453.59237 g
#include "HX711.h"


// HX711 circuit wiring
const int LOADCELL_DOUT_PIN = 2;
const int LOADCELL_SCK_PIN = 3;

// function
void initialize(double target_weight);

HX711 scale;

void setup() {
  Serial.begin(115200);
  scale.begin(LOADCELL_DOUT_PIN, LOADCELL_SCK_PIN);
  scale.set_scale(-7000);    // 알고 있는 물건으로 무게를 보정하는 상수값
  scale.tare();                // 0점 조절, 초기화 (0 kg)

  initialize(2000);

  
}

void loop() {
  Serial.print("one reading:\t");
  Serial.println(scale.get_units()*0.453592, 1);   //한자리 읽기
  delay(500);

//  Serial.print("\t| average:\t");
//  Serial.println(scale.get_units(10)*0.453592, 1);   //  10개 평균 읽기
//  scale.power_down();             // put the ADC in sleep mode
//  delay(5000);
//  scale.power_up();
}


void initialize(double target_weight=2000)
{
  double factor = -7000;
  
  while(true){
      //scale.set_scale control Calibration factor
      scale.set_scale(factor);
      double weight = scale.get_units()*(-0.453592);
      Serial.println(weight);
      if(weight <= target_weight){
        break;  
      }
      else{
        factor -= 10;
      }
  }

  Serial.print("보정값=");
  Serial.println(factor);
}

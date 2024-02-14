#include "NDelayFunc.h"

const int LED = 10;
unsigned int t_high = 0;


//안에 넣을 함수 정의
void fading(){
  t_high++;
  if(t_high > 255) t_high = 0;
  analogWrite(LED, t_high);    
}

NDelayFunc nDelayFading(4, fading); //사용할 함수 선언

void setup(){
  
}

void loop(){
  nDelayFading.run();  
}

// esp32 devkit Test
// GPIO34 ~ GPIO39 : 6 INPUT_PIN(NOT OUTPUT)  입력전용 내장 풀업저항 없음
// GPIO6 ~ GPIO11 : FLASH memory (no use) 입출력 핀으로 사용 X
// GPIO4(T0),0(T1),2,15,13,12,14,27,33,32 : Touch Sensor
// GPIO36,37,38,39,32,33,34,35 : ADC1
// GPIO0,2,15,13,12,14,27,25,26 : ADC2
// GPIO25, GPIO26 : DAC
// GPIO36,39,34,35,25,26,33,32,4,0,2,15,13,12,14,27 : RTC (real time clock) pin
// analogWrite 사용 X


//RIGHT
const int LED1 = 23;  //아무것도 안쓰면 GPIO
const int LED2 = 22;
const int LED3 = 1;
const int LED4 = 3;
const int LED5 = 21;
const int LED6 = 19;
const int LED7 = 18;
const int LED8 = 18;
const int LED9 = 5;
const int LED10 = 17;
const int LED11 = 16;
const int LED12 = 4;
const int LED13 = 0;
const int LED14 = 2;
const int LED15 = 15;

//LEFT
const int LED16 = 32;  
const int LED17 = 33;
const int LED18 = 25;
const int LED19 = 26;
const int LED20 = 27;
const int LED21 = 14;
const int LED22 = 12;
const int LED23 = 13;



void setup() {
  pinMode(LED1, OUTPUT);
  pinMode(LED2, OUTPUT);
  pinMode(LED3, OUTPUT);
  pinMode(LED4, OUTPUT);
  pinMode(LED5, OUTPUT);
  pinMode(LED6, OUTPUT);
  pinMode(LED7, OUTPUT);
  pinMode(LED8, OUTPUT);
  pinMode(LED9, OUTPUT);
  pinMode(LED10, OUTPUT);
  pinMode(LED11, OUTPUT);
  pinMode(LED12, OUTPUT);
  pinMode(LED13, OUTPUT);
  pinMode(LED14, OUTPUT);
  pinMode(LED15, OUTPUT);
  pinMode(LED16, OUTPUT);
  pinMode(LED17, OUTPUT);
  pinMode(LED18, OUTPUT);
  pinMode(LED19, OUTPUT);
  pinMode(LED20, OUTPUT);
  pinMode(LED21, OUTPUT);
  pinMode(LED22, OUTPUT);
  pinMode(LED23, OUTPUT);


  
  digitalWrite(LED1, LOW);
  digitalWrite(LED2, LOW);
  digitalWrite(LED3, LOW);
  digitalWrite(LED4, LOW);
  digitalWrite(LED5, LOW);
  digitalWrite(LED6, LOW);
  digitalWrite(LED7, LOW);
  digitalWrite(LED8, LOW);
  digitalWrite(LED9, LOW);
  digitalWrite(LED10, LOW);
  digitalWrite(LED11, LOW);
  digitalWrite(LED12, LOW);
  digitalWrite(LED13, LOW);
  digitalWrite(LED14, LOW);
  digitalWrite(LED15, LOW);
  digitalWrite(LED16, LOW);
  digitalWrite(LED17, LOW);
  digitalWrite(LED18, LOW);
  digitalWrite(LED19, LOW);
  digitalWrite(LED20, LOW);
  digitalWrite(LED21, LOW);
  digitalWrite(LED22, LOW);
  digitalWrite(LED23, LOW);
  
}



void loop() {
//  for(int i=0; i<4; i++){
//    
//  }
  digitalWrite(LED1, HIGH);
  digitalWrite(LED2, HIGH);
  digitalWrite(LED3, HIGH);
  digitalWrite(LED4, HIGH);
  digitalWrite(LED5, HIGH);
  digitalWrite(LED6, HIGH);
  digitalWrite(LED7, HIGH);
  digitalWrite(LED8, HIGH);
  digitalWrite(LED9, HIGH);
  digitalWrite(LED10, HIGH);
  digitalWrite(LED11, HIGH);
  digitalWrite(LED12, HIGH);
  digitalWrite(LED13, HIGH);
  digitalWrite(LED14, HIGH);
  digitalWrite(LED15, HIGH);
  digitalWrite(LED16, HIGH);
  digitalWrite(LED17, HIGH);
  digitalWrite(LED18, HIGH);
  digitalWrite(LED19, HIGH);
  digitalWrite(LED20, HIGH);
  digitalWrite(LED21, HIGH);
  digitalWrite(LED22, HIGH);
  digitalWrite(LED23, HIGH);
  delay(500);
  
  digitalWrite(LED1, LOW);
  digitalWrite(LED2, LOW);
  digitalWrite(LED3, LOW);
  digitalWrite(LED4, LOW);
  digitalWrite(LED5, LOW);
  digitalWrite(LED6, LOW);
  digitalWrite(LED7, LOW);
  digitalWrite(LED8, LOW);
  digitalWrite(LED9, LOW);
  digitalWrite(LED10, LOW);
  digitalWrite(LED11, LOW);
  digitalWrite(LED12, LOW);
  digitalWrite(LED13, LOW);
  digitalWrite(LED14, LOW);
  digitalWrite(LED15, LOW);
  digitalWrite(LED16, LOW);
  digitalWrite(LED17, LOW);
  digitalWrite(LED18, LOW);
  digitalWrite(LED19, LOW);
  digitalWrite(LED20, LOW);
  digitalWrite(LED21, LOW);
  digitalWrite(LED22, LOW);
  digitalWrite(LED23, LOW);
  delay(500);
}

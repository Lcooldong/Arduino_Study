//built-in LEd = D0
// 0 -> GPIO 0 의미

//- GPIO 0,2,15 번은 부팅모드를 설정하기 위해 내정되어 있어 사용시에 주의를 요한다. 
//- GPIO1,3 번은 시리얼 통신용이다.
//- GPIO 6~11번은 플래시메모리용으로 이미 사용중이므로 사용할 수 없다.
//- GPIO 16번 (D0) 은 Sleep mode에서 벗어나기 위한 Wake용으로 사용된다
//따라서 사용자가 조건 없이 범용으로 사용할 수 있는 핀은 GPIO 4번(D2), GPIO 5번 (D1),  GPIO 12~14번 (D6,D7,D5) 다섯개 밖에 되지 않는다.     
//내장LED는 GPIO 2번(D4) 에 연결되어 있다. ( 아두이노 나노는 디지털13번 )

//절전 sleeping mode 있음
//

//#define GPIO

#ifdef GPIO
//RIGHT
#define LED1 16
#define LED2 5
#define LED3 4
#define LED4 0
#define LED5 2
#define LED6 14
#define LED7 12
#define LED8 13
#define LED9 15
#define LED10 3
#define LED11 1
// LEFT
#define LED12 10


#else
//RIGHT
#define LED1 D0
#define LED2 D1 // 제한없음
#define LED3 D2 //
#define LED4 D3
#define LED5 D4
#define LED6 D5 //
#define LED7 D6 //
#define LED8 D7 //



#endif



void setup() {
  Serial.begin(9600);
  pinMode(LED6, OUTPUT);
  pinMode(LED7, OUTPUT);
  pinMode(LED8, OUTPUT);


}

void loop() {
  digitalWrite(LED6, LOW);
  digitalWrite(LED7, LOW);
  digitalWrite(LED8, LOW);
  delay(500);
  digitalWrite(LED6, HIGH);
  digitalWrite(LED7, HIGH);
  digitalWrite(LED8, HIGH);
  delay(500);

}

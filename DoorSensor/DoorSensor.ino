#define Reed 13 // D13, GND
#define LED  15

unsigned long closedTime = 0;
unsigned long openedTime = 0;
const long interval = 2000;
int doorFlag = 0;
bool dataState = false;

void setup() {
  Serial.begin(115200);
  pinMode(Reed, INPUT_PULLUP);  //GND
  pinMode(LED, OUTPUT);
}

void loop() {
  
  //Serial.println(digitalRead(Reed));
  if(digitalRead(Reed) == 0){
    //Serial.println("close");
    doorFlag = 1;
    closedTime = millis();
  }else{
    //Serial.println("open");
    doorBell();
  }
}


// 시간 측정 시작 flag -> time1 (1일 때 마다 갱신됨)
// interval 3초보다 현재시간 - 이전시간이 크면 작동
// 중간에 doorFlag 0->1 time1은 더 커짐 -> 무조건 발동되어버림
// 3초가 되었을 때 openFlag 발동-> doorFlag 0일때 초기화
// 간격을 딱 집어낼 수 있어야함-> 어떻게?  

// 열었을 때 2번 닫으면 1번 찍힘
// 닫을 때 doorFlag가 1되면서 찍힘
// 열었을 때 시간 차이 나서 1번 찍히고 그다음 간격 때 

// 시간측정 - 0 일때 한번
// 닫을 때 시간 측정 - 이전 시간 간격 커서 한번
// 다시 열 때 시간 측정 - 이전 시간 간격 커서 한번

// 타이머를 2개를 둬서 (닫힘 -> 열림) 시간 간격을 구함
// 문이 열렸을 때 시간, 처음에는 둘이 같지만 닫혔던 시간은 과거, 차이가 발생
void doorBell(){
  if(doorFlag){
    openedTime = millis() - closedTime;   
    Serial.println(openedTime);
    delay(100);
    if(openedTime >= interval){ //3초보다 커졌을때
      Serial.println("문열림");
      doorFlag = 0;
    }
  }
}

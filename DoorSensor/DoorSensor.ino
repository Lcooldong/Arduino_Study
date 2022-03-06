#define Reed 13 // D13, GND
#define LED  15

unsigned long currentMillis = 0;
unsigned long previousMillis = 0;
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
    doorFlag = 0;
  }else{
    //Serial.println("open");
    doorFlag = 1;
    doorBell();
  }
}


// 시간 측정 시작 flag -> time1 (1일 때 마다 갱신됨)
// interval 3초보다 현재시간 - 이전시간이 크면 작동
// 중간에 doorFlag 0->1 time1은 더 커짐 -> 무조건 발동되어버림
// 3초가 되었을 때 openFlag 발동-> doorFlag 0일때 초기화
// 간격을 딱 집어낼 수 있어야함-> 어떻게?  
void doorBell(){
  currentMillis = millis();
  if(currentMillis - previousMillis <= interval){
    if(!dataState){
    previousMillis = currentMillis;
    }
    dataState = true;
  }else if(currentMillis - previousMillis >= interval){
    Serial.println("메세지");
    dataState = false;
  }
}

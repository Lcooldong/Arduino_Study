// 아날로그 디지털 둘다 가능
#define DIGITAL
//#define ANALOG

#ifdef DIGITAL

#define irPin 0
int ir;
void setup(){
  Serial.begin(115200);
  pinMode(irPin, INPUT);
}
void loop(){
  ir = digitalRead(irPin);
  if (ir == LOW){
    Serial.println("detect");
  }
  else{
    Serial.println("no detect");
  }
  delay(500);
}

#elif ANALOG

int ir;
const int irPin = A0;
void setup(){
  Serial.begin(115200);
  pinMode(irPin, INPUT);
}
void loop(){
  ir = analogRead(A0);
  if (ir <= 300){
    Serial.print("detect");
  }
  else{
    Serial.print("no detect");
  }
  delay(100);
  Serial.print(',');
  Serial.println(ir);
}

#endif

void setup() {
  Serial.begin(115200);
  pinMode(PC13, OUTPUT);
  


}

void loop() {

  digitalWrite(PC13, HIGH);
  delay(1000);
  digitalWrite(PC13, LOW);
  delay(1000);

}

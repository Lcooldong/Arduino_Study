#define tilt_pin 7

void setup() {
  pinMode(tilt_pin, INPUT);
  Serial.begin(9600);
  
}

void loop() {
  int tilt = digitalRead(tilt_pin);
  Serial.println(tilt);
  delay(500);
}

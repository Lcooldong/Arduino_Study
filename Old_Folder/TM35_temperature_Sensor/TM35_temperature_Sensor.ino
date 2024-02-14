int value = 0;
int raw_data = A7;
float temperature;

void setup() {
  Serial.begin(9600);
  

}

void loop() {
  value = analogRead(raw_data);
  temperature = (5.0 * value * 100)/1024;
  Serial.println(temperature);
  delay(1000);
}

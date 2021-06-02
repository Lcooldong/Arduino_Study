int pb_input = 8;
char* state;
char buf[100];

void setup() {
  Serial.begin(9600);
  pinMode(pb_input, INPUT_PULLUP);

}

void loop() {
  int pb_data = digitalRead(pb_input);
  
  if(pb_data == 0) state = "ON";
  
  else state = "OFF";
  
  sprintf(buf, "data : %d | %s", pb_data, state);
  Serial.println(buf);
  delay(200);

}

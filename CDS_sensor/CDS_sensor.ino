#define CDS A0
int light = 0;
int percent = 0;
char buf[100];


void setup() {
  Serial.begin(9600);

}

void loop() {
  light = analogRead(CDS);
  percent = map(light, 1023, 0, 0, 100);  //PULL-UP 저항
//  percent = map(light, 1023, 0, 100, 0);  //PULL-DOWN 저항
  sprintf(buf, "data: %4d |밝기: %3d %%", light ,percent);
  Serial.println(buf);

  delay(100);

}

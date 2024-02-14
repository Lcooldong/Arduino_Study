#define Cds A0

int LED[4] = {2,3,4,5};
int darkness = 0;
int brightness = 0;
int percent = 0;
char buf[100];


void setup() {
  Serial.begin(9600);
  for(int i = 0; i < 4; i++)
  {
    pinMode(LED[i], OUTPUT);  
  }

}

void loop() {
  darkness = analogRead(Cds);

  percent = map(darkness, 1023, 0, 0, 100);  //PULL-UP 저항

  if(percent < 25) digitalWrite(LED[0], HIGH);  //white
  else digitalWrite(LED[0], LOW);
  if(percent < 40) digitalWrite(LED[1], HIGH);  //green
  else digitalWrite(LED[1], LOW);
  if(percent < 55) digitalWrite(LED[2], HIGH);  //yello
  else digitalWrite(LED[2], LOW);
  if(percent < 70) digitalWrite(LED[3], HIGH);  //red
  else digitalWrite(LED[3], LOW);
  
  sprintf(buf, "data: %4d |밝기: %3d %%", darkness ,percent);
  Serial.println(buf);

  delay(100);
  
}

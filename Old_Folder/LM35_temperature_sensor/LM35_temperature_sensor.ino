void setup()
{
  Serial.begin(115200);
}

void loop()
{
  char buffer1[32];
  char buffer2[32];
  char buffer3[32];
  float adc = analogRead(A0) * 5.0/1024.0;  // 측정 전압
  float tmp = (adc * 100.0);                // 온도 센서
  
  dtostrf(adc, 5, 2, buffer1);  //전체 5자리 소수점 2자리까지 더블-> 스트링
  dtostrf(tmp, 5, 1, buffer2);
  
  sprintf(buffer3, "adc(%s), tmp(%s)", buffer1, buffer2);
  Serial.println(buffer3);
  delay(100);
}

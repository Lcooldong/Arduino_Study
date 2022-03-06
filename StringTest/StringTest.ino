String text = "abcdef_!5000";
int index1 = text.indexOf('!'); // 없으면 -1 반환
int size_of_text = text.length();


void setup() {
  Serial.begin(115200);

}

void loop() {
  Serial.println(index1);   // 0부터해서 7
  Serial.println(size_of_text);   // 12
  int inString1 = text.substring(index1+1, size_of_text).toInt(); 없으면 0임
  Serial.println(inString1);
  delay(5000);

}

void setup(){

  Serial.begin(9600);

}
void loop(){



    //코드 없다~~

}

void serialEvent(){

  char text = Serial.read();

  char textbox[3];

  textbox = &text;
  
  Serial.print("data : ");

  Serial.println(text);

  Serial.print(textbox);

}

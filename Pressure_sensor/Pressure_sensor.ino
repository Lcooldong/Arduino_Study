//#include <Wire.h>     //I2C 통신인데 없어도 작동함
#include <LiquidCrystal_I2C.h>
LiquidCrystal_I2C lcd(0x27,16,2);

int FSR = A0;
int LED = 2;
int val = 0;

void setup() {

  Serial.begin(9600);
  pinMode(LED, OUTPUT);
  lcd.init();             //LCD 초기화
  lcd.backlight();
}

void loop() {
  Serial.print(analogRead(FSR));
  Serial.println("g");
  val = map(analogRead(FSR), 0, 1023, 0, 255);  //내부적 작동이라 lcd는 따로 계산
//  val = analogRead(FSR)*4;
  analogWrite(LED, val);
  lcd.setCursor(0,0);
  lcd.print("Pressure :");
  lcd.setCursor(11,0);
  lcd.print(val*4);
  lcd.setCursor(15,0);
  lcd.print("g");
  lcd.init();
  delay(50);

}

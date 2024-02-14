//#include <Wire.h>     //I2C 통신인데 없어도 작동함
#include <LiquidCrystal_I2C.h>
LiquidCrystal_I2C lcd(0x27,16,2);

int FSR = A0;
int LED = 2;
int val = 0;

unsigned pressure_prev = 0;
const unsigned long pressure_delay = 5;

unsigned lcd_prev = 0;
const unsigned long lcd_delay = 10;


void setup() {

  Serial.begin(9600);
  pinMode(LED, OUTPUT);
  lcd.init();             //LCD 초기화
  lcd.backlight();
}

void loop() {

  unsigned pressure_now = millis(); //현재 시간 milli 단위로 읽어 저장
  if(pressure_now - pressure_prev >= pressure_delay)
  {
    pressure_prev = pressure_now;
    
    Serial.print(analogRead(FSR));
    Serial.println("g");
    val = map(analogRead(FSR), 0, 1023, 0, 255);  //내부적 작동이라 lcd는 따로 계산
    analogWrite(LED, val);
  }

  unsigned lcd_now = millis();
  if(lcd_now - lcd_prev >= lcd_delay)
  {
    lcd_prev = lcd_now;
    
    lcd.setCursor(0,0);
    lcd.print("Pressure :");
    lcd.setCursor(11,0);
    lcd.print(val*4);
    lcd.setCursor(15,0);
    lcd.print("g");
    lcd.init();
  }

}

//#define MAX_TYPE(type) ((type)(pow(2, sizeof(type) * 8 - 1)-1))
//#define MAX_INT MAX_TYPE(int)
//#define MAX_LONG MAX_TYPE(long)

//#include <Wire.h>     //I2C 통신인데 없어도 작동함
#include <LiquidCrystal_I2C.h>
LiquidCrystal_I2C lcd(0x27,16,2);



int FSR = A0;
int LED_1 = 2;
int LED_2 = 5;
int val = 0;
char buf[80];

unsigned pressure_prev = 0;
const unsigned long pressure_delay = 50;

unsigned lcd_prev = 0;
const unsigned long lcd_delay = 100;


//unsigned int on_off1 = 0;
//unsigned LED_1_prev = 0;
//const unsigned long LED_1_delay = 3000;
//
//
//unsigned int on_off2 = 0;
//unsigned LED_2_prev = 0;
//const unsigned long LED_2_delay = 5000;


void setup() {


  Serial.begin(9600);
  pinMode(LED_1, OUTPUT);
  pinMode(LED_2, OUTPUT);
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
    analogWrite(LED_1, val);
  }

  unsigned lcd_now = millis();
  if(lcd_now - lcd_prev >= lcd_delay)
  {
    lcd_prev = lcd_now;

    lcd.setCursor(0,0);
    lcd.print("Pressure :");
    lcd.setCursor(11,0);
    sprintf(buf, "%3d", val*4); // 문자열 buf에 저장
    lcd.print(buf);
    lcd.setCursor(15,0);
    lcd.print("g");
  
  }



//  unsigned LED_1_now = millis();
//  if(LED_1_now - LED_1_prev >= LED_1_delay)
//  {
//    LED_1_prev = LED_1_now;
//
//    on_off1++;
//    if(on_off1>1) on_off1 = 0;
//    digitalWrite(LED_1, on_off1);
//
//  }
//
//
//  unsigned LED_2_now = millis();
//  if(LED_2_now - LED_2_prev >= LED_2_delay)
//  {
//    LED_2_prev = LED_2_now;
//    on_off2++;
//    if(on_off2>1) on_off2 = 0;
//    digitalWrite(LED_2, on_off2);
//
//  }


}

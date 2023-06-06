// Library
#include <Arduino.h>
#include <WiFi.h>

#include <U8g2lib.h>
#include <Wire.h>

#include <FS.h>
#include <LittleFS.h>

#include "neopixel.h"
#include "MyLittleFS.h"
// Define
#define BUILTIN_LED 2
#define LEDC_CHANNEL_0 0
#define LEDC_BASE_FREQ 5000
#define LEDC_TIMER_13_BIT 13
//#define RGB_LED

#define SDA_PIN 5
#define SCL_PIN 6
//#define ANALOG_LED
//#define DEGITAL_LED

// Value
bool isOn = false;
int brightness = 0;
int fadeAmount = 5;


// Function below
void ledcAnalogWrite(uint8_t channel, uint32_t value, uint32_t valueMax);
void listDir(fs::FS &fs, const char * dirname, uint8_t levels);

U8G2_SSD1306_72X40_ER_F_HW_I2C u8g2(U8G2_R0, /* reset=*/ U8X8_PIN_NONE);



// Instance
WiFiServer server(80);
MyNeopixel* myNeopixel = new MyNeopixel();
MyLittleFS* myLittleFS = new MyLittleFS();

void setup() {
  Serial.begin(115200);
  Wire.begin(SDA_PIN, SCL_PIN);
  u8g2.begin();
  u8g2.enableUTF8Print();
  myLittleFS->InitLitteFS();
  myLittleFS->listDir(LittleFS, "/", 0);
  myLittleFS->writeFile(LittleFS, "/config.txt", "Hello C3");
  myLittleFS->readFile(LittleFS, "/config.txt");
  myLittleFS->listDir(LittleFS, "/", 0);
  // listDir(LittleFS, "/", 3);



  // myNeopixel->strip.begin();
  // File _file = SPIFFS.open("/config.txt", "w");
  // if(!_file)
  // {
  //   Serial.println("Failed to open file");
  // }
  // else
  // {
  //   Serial.println("Success to open file");
  // }
  // _file.print("Hello\n");

  // _file.close();
  // delay(1000);
  // _file = SPIFFS.open("/config.txt", "r");
  // Serial.println(_file.readStringUntil('\n'));
  // _file.close();
  // listDir("/");


#ifdef ANALOG_LED
  ledcSetup(LEDC_CHANNEL_0, LEDC_BASE_FREQ, LEDC_TIMER_13_BIT);
  ledcAttachPin(BUILTIN_LED, LEDC_CHANNEL_0);
#endif

#ifdef DEGITAL_LED
  pinMode(BUILTIN_LED, OUTPUT);
#endif



}

void loop() {
  // Serial.println("Hello");
  u8g2.clearBuffer();  
  u8g2.setFontMode(1);
  
  u8g2.setFont(u8g2_font_unifont_t_korean2);
  // u8g2.setFont(u8g2_font_cu12_tr);		  
  u8g2.setCursor(0,15);
  u8g2.print(F("안녕"));
  u8g2.sendBuffer();
  // delay(3000);
  // Serial.println("Loop");
  delay(2000);

#ifdef RGB_LED
  myNeopixel->pickOneLED(0, myNeopixel->strip->Color(255, 0, 0), 50, 50);
  delay(1000);
  myNeopixel->pickOneLED(0, myNeopixel->strip->Color(0, 255, 0), 50, 50);
  delay(1000);
  // pickOneLED(0, strip.Color(255, 0, 0), 50, 50);
  // delay(1000);
  // pickOneLED(0, strip.Color(0, 255, 0), 50, 50);
  // delay(1000);
#endif

#ifdef DIGITAL_LED
  if(isOn)
  {
    digitalWrite(BUILTIN_LED, HIGH);
    isOn = false;
    delay(1000);
  }
  else
  {
    digitalWrite(BUILTIN_LED, LOW);
    isOn = true;
    delay(1000);
  }
#endif

#ifdef ANALOG_LED
  ledcAnalogWrite(LEDC_CHANNEL_0, brightness, 255);
  brightness += fadeAmount;
  if(brightness <= 0 || brightness >= 255)
  {
    fadeAmount = -fadeAmount;
  }
  delay(50);
#endif

}




void ledcAnalogWrite(uint8_t channel, uint32_t value, uint32_t valueMax = 255)
{
  uint32_t duty = (8191/ valueMax) * min(value, valueMax);
  ledcWrite(channel, duty);
}

void listDir(fs::FS &fs, const char * dirname, uint8_t levels){
    Serial.printf("Listing directory: %s\r\n", dirname);

    #ifdef ARDUINO_ARCH_ESP8266
    File root = fs.open(dirname, "r");
    #else
    File root = fs.open(dirname);
    #endif

    if(!root){
        Serial.println("- failed to open directory");
        return;
    }
    if(!root.isDirectory()){
        Serial.println(" - not a directory");
        return;
    }

    File file = root.openNextFile();
    while(file){
        if(file.isDirectory()){
            Serial.print("  DIR : ");
            Serial.println(file.name());
            if(levels){
                #ifdef ARDUINO_ARCH_ESP8266
                listDir(fs, file.fullName(), levels -1);
                #else
                listDir(fs, file.path(), levels -1);
                #endif
            }
        } else {
            Serial.print("  FILE: ");
            Serial.print(file.name());
            Serial.print("\tSIZE: ");
            Serial.println(file.size());
        }
        file = root.openNextFile();
    }
}

// void writeFile(const char* path, const char* message)
// {
//   File file = SPIFFS.open(path, "w");
//   if(!file)
//   {
//     Serial.println("Failed to open file for writing");
//     return;
//   }
//   if(file.print(message))
//   {
//     Serial.println("File written");
//   }
//   else
//   {
//     Serial.println("Write failed");
//   }

//   file.close();
// }

// void readFile(const char* path)
// {
//   File file = SPIFFS.open(path, "r");
//   if(!file || file.isDirectory())
//   {
//     Serial.println(" - failed to open file for reading");
//     return;
//   }
//   Serial.print("Read from file : ");
//   while(file.available())
//   {
//     Serial.write(file.read());
//   }
//   file.close();
// }


// void listDir(const char * dirname)
// { 
//   Serial.printf("Listing directory: %s\r\n", dirname); 
//   File root = SPIFFS.open(dirname); // ESP8266은 확장자 "Dir"과 "File"로 구분해서 사용, ESP32는 "File"로 통합 
//   File file = root.openNextFile(); 
//   while(file){ // 다음 파일이 있으면(디렉토리 또는 파일) 
//     if(file.isDirectory()){ // 다음 파일이 디렉토리 이면 
//       Serial.print("  DIR : "); Serial.println(file.name()); // 디렉토리 이름 출력 
//     } else {                // 파일이면 
//       Serial.print("  FILE: "); Serial.print(file.name());   // 파일이름 
//       Serial.print("\tSIZE: "); Serial.println(file.size()); // 파일 크기 
//     } 
//     file = root.openNextFile(); 
//   }
//   file.close();
// }


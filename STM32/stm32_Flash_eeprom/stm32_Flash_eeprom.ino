#include <FlashStorage_STM32.h>

#define LED_BUILTIN PC6
#define BUTTON      PC13

int address = 12;
byte value;

struct MyObject
{
  float field1;
  byte field2;
  char name[10];
};

void setup() {
  Serial.begin(115200);
  pinMode(LED_BUILTIN, OUTPUT);
  pinMode(BUTTON, INPUT);

  while(!Serial);
  delay(200);

  Serial.print(F("\nStart FlashStoreAndRetrieve on "));
  Serial.println(BOARD_NAME);
  Serial.println(FLASH_STORAGE_STM32_VERSION);

  Serial.printf("EEPROM length: %d\r\n", EEPROM.length());
  

  float f = 123.456f;
  int eeAddress = 31;

  MyObject customVar =
  {
    3.14159f,
    65,
    "Working!"
  };

  EEPROM.put(eeAddress, f);
  eeAddress += sizeof(float); // 31번 + float 크기 이후에 쓰기
  EEPROM.put(eeAddress, customVar);


  float getf = 0.00f;
  eeAddress = 31;
  EEPROM.get(eeAddress, getf);
  Serial.println();
  Serial.printf("[%d]Float: %0.3lf\r\n", eeAddress, getf);
  eeAddress += sizeof(float);
  MyObject recvCustomVar;
  EEPROM.get(eeAddress, recvCustomVar);

  Serial.println("\r\nRead custom object from EEPROM: ");
  Serial.println(recvCustomVar.field1, 5);
  Serial.println(recvCustomVar.field2);
  Serial.println(recvCustomVar.name);


  EEPROM.write(address, 0x0B);
  EEPROM.commit();

  byte value = EEPROM.read(address);
  
  Serial.printf("[%d] %d \r\n", address, value);
  if (++address == EEPROM.length())
  {
    address = 0;
  }

  eepromClear();
  delay(1000);

  byte byteValue = EEPROM.read(address);
  Serial.printf("\r\n=>[%d] %d \r\n", address, byteValue);
  float clearf = 0.00f;
  eeAddress = 31;
  EEPROM.get(eeAddress, clearf);
  Serial.printf("=>[%d]Float: %0.3lf\r\n", eeAddress, clearf);
  eeAddress += sizeof(float);
  EEPROM.get(eeAddress, recvCustomVar);
  Serial.println("\r\n=>Read custom object from EEPROM: ");
  Serial.println(recvCustomVar.field1, 5);
  Serial.println(recvCustomVar.field2);
  Serial.println(recvCustomVar.name);

}

void loop() {
  

}


bool eepromClear()
{
  unsigned long startMillis = millis();

  for(int i=0; i<EEPROM.length(); i++)
  {
    EEPROM.write(i, 0);

  }
  EEPROM.commit();

  Serial.print("Done clearing emulated EEPROM. Time spent (ms) = ");
  Serial.println(millis() - startMillis);
  return true;
}


bool eepromShow()
{
  

}

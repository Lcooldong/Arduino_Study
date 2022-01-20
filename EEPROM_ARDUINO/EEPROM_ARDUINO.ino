/* This program is for EEPROM in Arduino,
   EEPROM concept in ESP (ESP8266 or nodeMCU and ESP32) is different
   https://pijaeducation.com/eeprom-in-arduino-and-esp/
*/

// include EEPROM Library
#include<EEPROM.h>

/* Defining addresses for data to store
   We know size of int is 2 Bytes in Arduino,
   If we store int at index 0 then the next EEPROM address can be anything (2,3..10 etc.) after 2 byte address i.e. (0-1)
  You can use this statement to find size of int type data Serial.println(sizeof(int)) 
*/
int eepromAddr1 = 0, eepromAddr2 = 10;

// int and string data which we store in EEPROM of Arduino Uno or Arduino Nano etc.
int intData = 1024, getInt;
String stringData = "My String", getStr;

// put means writing to EEPROM and get means reading from EEPROM
void setup() {
  Serial.begin(9600);
  // Message for user
  Serial.println("To put int Send → 'int' \nTo put String Send → 'string' \nTo get data Send → 'read'");
}

void loop() {
  if (Serial.available()) {
    String x = Serial.readString();
    delay(10);
    Serial.print("\nInput → ");
    Serial.println(x);

    /* if user send 'read' (can be any string of your choice) as input
        then we will read from EEPROM using 'EEPROM get' function
    */
    if (x == "read") {
      EEPROM.get(eepromAddr1, getInt);
      EEPROM.get(eepromAddr2, getStr);
      Serial.print("getInt:");
      Serial.println(getInt);
      Serial.print("getStr:");
      Serial.println(getStr);
    }
    /* if user send 'int' (can be any string of your choice) as input
        then we will write to EEPROM using 'EEPROM put' function
    */    
    else if (x == "int") {
      EEPROM.put(eepromAddr1, intData);
      Serial.println("intData write in EEPROM is Successful");
    }
    /* if user send 'string' (can be any string of your choice) as input
        then we will write to EEPROM using 'EEPROM put' function
    */    
    else if (x == "string") {
      EEPROM.put(eepromAddr2, stringData);
      Serial.println("stringData write in EEPROM is Successful");
    }
    /* if user send 'clear' (can be any string of your choice) as input
        then we will clear data from EEPROM
       This will take some time
    */
    else if (x == "clear") {
      for (int i = 0 ; i < EEPROM.length() ; i++) {
        EEPROM.write(i, 0);
      }
      Serial.println("EEPROM Clear Done!");
    }
    /* if user send any other string as input
        then display a message to user
    */
    else {
      Serial.println("Wrong Input");
    }
  }
}

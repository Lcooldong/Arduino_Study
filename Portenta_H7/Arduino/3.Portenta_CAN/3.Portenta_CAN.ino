/**************************************************************************************
 * INCLUDE
 **************************************************************************************/

// #include <Arduino_CAN.h>
#include "mbed.h"

/**************************************************************************************
 * CONSTANTS
 **************************************************************************************/

static uint32_t const CAN_ID = 0x124;

uint32_t preMillis[2] = {0,};
uint8_t leds[] = {LEDR, LEDG, LEDB};
bool ledToggle = false;
int count = 0;

#define CAN_TX PH_13  // Adjust if needed
#define CAN_RX PB_8  // Adjust if needed

// Initialize CAN interface with baud rate
mbed::CAN can1(CAN_RX, CAN_TX, 500000);  // 500 kbps



// Initialize CAN interface
void CAN_Init() {
  Serial.println("Initializing CAN...");
  if (can1.frequency(500000)) {  // Set CAN speed to 500 kbps
      Serial.println("CAN Initialized!");
  } else {
      Serial.println("CAN Initialization Failed!");
  }
}


void onCANReceived();

void setup()
{
  Serial.begin(115200);



  while (!Serial) { }

  CAN_Init();
  for(int i=0; i< sizeof(leds); i++)
  {
    pinMode(leds[i], OUTPUT);
    digitalWrite(leds[i], HIGH);
  }
  // digitalWrite(leds[0], LOW);
  can1.filter(0x123, 0x7FF, CANStandard); 
  
  
}

static uint32_t msg_cnt = 0;


void loop()
{
  uint32_t curMillis = millis();
  if(curMillis - preMillis[0] >= 500)
  {
    preMillis[0] = curMillis;
    
    digitalWrite(leds[1], ledToggle);
    ledToggle = !ledToggle;
    
  }
  if(curMillis - preMillis[1] >= 1000)
  {
    preMillis[1] = curMillis;
    count++;
    Serial.println((String)"UART : " + count);
    char data[8] = {0x11, 0x22, 0x33, 0x44, 0x55, 0x66, 0x77, 0x88};
    mbed::CANMessage send_msg(0x123, data, 8);  // Standard ID 0x123, 8-byte data

    if (can1.write(send_msg)) {
        Serial.println("CAN Message Sent!");
    } else {
        Serial.println("CAN Transmission Failed!");
    }
  }
  // ---- Receive CAN Message ----
  onCANReceived();

  


}


void onCANReceived() {
    mbed::CANMessage msg;
    if (can1.read(msg)) {
        Serial.print("Interrupt: Received CAN Message - ID: 0x");
        Serial.print(msg.id, HEX);
        Serial.print(", Data: ");
        for (int i = 0; i < msg.len; i++) {
            Serial.print(msg.data[i], HEX);
            Serial.print(" ");
        }
        Serial.println();
    }

    
}

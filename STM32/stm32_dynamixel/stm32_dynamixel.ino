#include <Dynamixel2Arduino.h>
#include "HardwareSerial.h"

#define UART2_DE      PA1
#define UART2_TX      PA2
#define UART2_RX      PA3

#define SERIAL_TX_BUFFER_SIZE 1024
#define SERIAL_RX_BUFFER_SIZE 1024


#define DXL_ID           1
#define DXL_BAUDRATE 57600
#define DXL_PROTOCOL_VERSION 2.0

#define LED_BUILTIN   PC6
#define LED_INTERVAL  100


#define DXL_MAX_POSITION 4095
#define DXL_TARGET_POSITION 1350
#define DXL_SUB_POSITION 500
#define DXL_INITIAL_POSITION 0
#define DXL_INITIAL_VELOCITY 100


uint32_t lastMillis[3] = {0,};
bool led_state = false;
uint32_t count = 0;
uint32_t command_count = 0;
bool while_recv = false;

const int dxl_offset = 50;

bool dxlConnectState = false;
bool dxlConnectionLastState = false;
int dxlTargetPosition = 0;

HardwareSerial UART2(UART2_RX, UART2_TX);
Dynamixel2Arduino dxl(UART2, UART2_DE);
// Dynamixel2Arduino dxl(UART2);

using namespace ControlTableItem;

void serialCommand();
void dxlReconnect();

void setup() {
  Serial.begin(115200);
  pinMode(LED_BUILTIN, OUTPUT);

  dxl.begin(DXL_BAUDRATE);  
  dxl.setPortProtocolVersion(DXL_PROTOCOL_VERSION);
  dxl.ping(DXL_ID); 

  dxl.torqueOff(DXL_ID);
  dxl.setOperatingMode(DXL_ID, OP_POSITION);
  dxl.torqueOn(DXL_ID);

  dxl.writeControlTableItem(PROFILE_VELOCITY, DXL_ID, DXL_INITIAL_VELOCITY);
}

void loop() {
  uint32_t curMillis = millis();
  if(curMillis - lastMillis[0] >= LED_INTERVAL)
  {
    lastMillis[0] = curMillis;
    while_recv = false;
    digitalWrite(LED_BUILTIN, led_state);
    led_state = !led_state;
    count++;
    if(dxlConnectionLastState)
    {
      if(count % 2 == 0)
      {
        dxl.ledOn(DXL_ID);
      }
      else
      {
        dxl.ledOff(DXL_ID);
      }

      dxl.setGoalPosition(DXL_ID, dxlTargetPosition);  
    }
    
    // Serial.printf("[%d]\r\n", count);
  }
  else
  {
    while_recv = true;
  }

  if(curMillis -lastMillis[1] >= 1000)
  {
    lastMillis[1] = curMillis;
    command_count++;
    dxlReconnect();
  }

  serialCommand();

}

void serialCommand()
{
  if(millis() - lastMillis[2] >= 1)
  {
    if(Serial.available())
    {
      int ch = Serial.read();

      if(ch != -1)
      {
        switch (ch)
        {
          case '1':
            if(dxlTargetPosition >= DXL_TARGET_POSITION)
            {
              dxlTargetPosition = DXL_TARGET_POSITION;
            }
            else
            {
              dxlTargetPosition = dxlTargetPosition + dxl_offset;
            }
            break;
          case '2':
            if(dxlTargetPosition <= 0)
            {
              dxlTargetPosition = 0;
            }
            else
            {
              dxlTargetPosition = dxlTargetPosition - dxl_offset;
            }
            break;
          default:
            break;
        }
        Serial.printf("DXL Pos : %4d %5.2f\r\n", dxlTargetPosition, dxl.getPresentCurrent(DXL_ID));
      }
    }
    lastMillis[2] = millis();
  }
}


void dxlReconnect()
{
  dxlConnectState = dxl.ping(DXL_ID);
  if(dxlConnectState)
  {
    Serial.printf("Connected\r\n");

    if(dxlConnectionLastState != dxlConnectState)
    {

        dxl.setOperatingMode(DXL_ID, OP_POSITION);
        Serial.println("Reconnect TorqueOn");
        dxl.torqueOn(DXL_ID);
        dxl.writeControlTableItem(PROFILE_VELOCITY, DXL_ID, DXL_INITIAL_VELOCITY);
    }
  }
  else
  {
    
    Serial.printf("[%d] Not Connected\r\n", command_count);      
  }
  dxlConnectionLastState = dxlConnectState;
}

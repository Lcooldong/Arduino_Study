#include <MightyZap.h>

#include <Dynamixel2Arduino.h>
#include <MightyZap.h>
#include <Wire.h>
#include <ADS1115_WE.h>
#include <HardwareSerial.h>
#include "can.h"

#define I2C_ADDRESS   0x48
#define I2C1_SDA      PB7
#define I2C1_SCL      PA15
#define LED_BUILTIN   PC6

#define UART2_DE      PA1
#define UART2_TX      PA2
#define UART2_RX      PA3     


#define DXL_ID                1
#define DXL_BAUDRATE          57600
#define DXL_INITIAL_VELOCITY  100
#define DXL_MAX_POSITION      4095
#define DXL_TARGET_POSITION   1350
#define DXL_SUB_POSITION      600
#define DXL_INITIAL_POSITION  0

#define LSV_ID                  0
#define LINEAR_INITIAL_POSITION 0
#define LINEAR_RELEASE_POSITION 300
#define LINEAR_PUSH_POSITION    2800
#define LINEAR_MAX_POSITION     3600
#define LINEAR_MAX_SPEED        1023


const float DXL_PROTOCOL_VERSION = 2.0;


typedef struct{
  uint16_t dxl_position;
  uint16_t dxl_velocity;
  bool     dxl_connection;
  uint16_t lsv_position;
  uint16_t lsv_speed;
  bool     lsv_connection;
  
}gripper_t;

gripper_t myGripper;

uint32_t curMillis = 0;
uint32_t lastMillis[4] = {0,};
uint32_t led_value =0;
uint8_t led_direction = 0;
float voltage = 0.0;


enum{
  CAN_MODE = 0b0000'0000,
  ADS_MODE = 0b0000'0010
}Mode;

uint8_t select_mode = 0x00; 

HardwareSerial UART2(UART2_RX, UART2_TX);
Dynamixel2Arduino dxl(UART2, UART2_DE);
Mightyzap lsv(&UART2, UART2_DE);

ADS1115_WE adc = ADS1115_WE(I2C_ADDRESS);
// using namespace ControlTableItem;

bool ads1115_init();
bool dxl_init();
bool lsv_init();
bool motor_init();

void setup() {
  Serial.begin(115200);
  pinMode(LED_BUILTIN, OUTPUT); // PC6
  
  ads1115_init();
  motor_init();

}

void loop() {
  uint32_t curMillis = millis();
  
  switch (select_mode) {
    case ADS_MODE:
    
    break;
  }


  if(curMillis - lastMillis[0] >= 100)
  {
    lastMillis[0] = curMillis;

    if(myGripper.dxl_connection)
    {
      dxl.setGoalPosition(DXL_ID, myGripper.dxl_position);
    }
    if(myGripper.lsv_connection)
    {
      lsv.GoalPosition(LSV_ID, myGripper.lsv_position);
    }
  }

  if(curMillis - lastMillis[0] >= 100)
  {
    lastMillis[0] = curMillis;
    
  }

  if(curMillis - lastMillis[0] >= 100)
  {
    lastMillis[0] = curMillis;
  }

  if(curMillis - lastMillis[0] >= 100)
  {
    lastMillis[0] = curMillis;
    voltage = adc.getResult_V();

    Serial.printf("Result => %0.2f [%d]\r\n", voltage, adc.getRawResult());
  }

}





bool ads1115_init()
{
  Wire.setSDA(I2C1_SDA);
  Wire.setSCL(I2C1_SCL);
  Wire.begin();
  if(!adc.init()){
    Serial.println("ADS1115 not connected!");
    return false;
  }
  else
  {
    Serial.println("ADS1115 connected!");
    adc.setVoltageRange_mV(ADS1115_RANGE_6144);
    adc.setCompareChannels(ADS1115_COMP_0_GND); // select channel  0 ~ 3
    adc.setMeasureMode(ADS1115_CONTINUOUS);
    return true;
  }
}

bool motor_init()
{
  if(dxl_init() && lsv_init())
  {

  }
  else
  {
    return false;
  }

  return true;
}

bool dxl_init()
{
  dxl.begin(DXL_BAUDRATE);
  dxl.setPortProtocolVersion(DXL_PROTOCOL_VERSION);
  if(dxl.ping(DXL_ID))
  {
    dxl.torqueOff(DXL_ID);
    dxl.setOperatingMode(DXL_ID, OP_POSITION);
    dxl.torqueOn(DXL_ID);

    dxl.writeControlTableItem(ControlTableItem::PROFILE_VELOCITY, DXL_ID, DXL_INITIAL_VELOCITY);
  }
  else
  {
    return false;
  }

  return true;
}

bool lsv_init()
{
  lsv.begin(32);
  lsv.GoalSpeed(LSV_ID, LINEAR_MAX_SPEED);

  if(lsv.ping(LSV_ID) != 0xff)
  {
    lsv.GoalPosition(LSV_ID, LINEAR_INITIAL_POSITION);
  }
  else
  {
    return false;
  }

  return true;
}

bool motor_reconnect()
{
  bool dxl_state = dxl.ping(DXL_ID);


  if(dxl_state)
  {
    if(myGripper.dxl_connection != dxl_state)
    {
      dxl.setOperatingMode(DXL_ID, OP_POSITION);
      dxl.torqueOn(DXL_ID);
      dxl.writeControlTableItem(ControlTableItem::PROFILE_VELOCITY, DXL_ID, DXL_INITIAL_VELOCITY);
      Serial.printf("Reconnected\r\n");
    }
  }
  else
  {
    Serial.printf("Not Connected\r\n");
  }  
  myGripper.dxl_connection = dxl_state;

}
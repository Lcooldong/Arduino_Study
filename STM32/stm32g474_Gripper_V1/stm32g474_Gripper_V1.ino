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

typedef struct
{
  float voltage;
  int16_t raw;

}ads_t;




typedef struct __attribute__((packed)){
  uint16_t dxl_position;
  uint16_t dxl_velocity;
  bool     dxl_connection;

  uint16_t lsv_position;
  uint16_t lsv_speed;
  int      lsv_connection;

  bool     hall_toggle;
}gripper_t;

typedef union
{
  gripper_t gripper;
  uint8_t datas[sizeof(gripper_t)];

}gripper_packet_t;

gripper_packet_t packet;




gripper_t myGripper;
ads_t myADS;

uint32_t serialLastTime = 0;
uint32_t curMillis = 0;
uint32_t lastMillis[4] = {0,};
bool led_state = false;
uint32_t led_value =0;
uint8_t led_direction = 0;



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

void serialCommand();
bool ads1115_init();
bool dxl_init();
bool lsv_init();
bool motor_init();
bool motor_reconnect();

void setup() {
  Serial.begin(115200);
  pinMode(LED_BUILTIN, OUTPUT); // PC6
  
  delay(1000);

  ads1115_init();
  motor_init();
  myGripper.hall_toggle = false;

}

void loop() {
  curMillis = millis();

  serialCommand();  

  switch (select_mode) {
    case ADS_MODE:
    
    break;
  }

  // // MOTOR
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
  else  // Toque off
  {

  }




  // HALL SENSOR
  if((curMillis - lastMillis[1] >= 100) && myGripper.hall_toggle)
  {
    lastMillis[1] = curMillis;
    myADS.voltage = adc.getResult_V();
    myADS.raw = adc.getRawResult();

    Serial.printf("Result => %0.2f [%d]\r\n", myADS.voltage, myADS.raw);
  }

  // INDICATOR
  if(curMillis - lastMillis[2] >= 1000)
  {
    lastMillis[2] = curMillis;
    digitalWrite(LED_BUILTIN, led_state);
    led_state = !led_state;
    Serial.printf("DXL : %s | LSV : 0x%02X\r\n", myGripper.dxl_connection?"ON":"OFF", myGripper.lsv_connection);
  }


  // RECONNECT
  if(curMillis - lastMillis[3] >= 2000)
  {
    lastMillis[3] = curMillis;
    motor_reconnect();
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
    Serial.println("Motor Begin");
  }
  else
  {
    Serial.println("Motor Not Connected");
    return false;
  }

  return true;
}

bool dxl_init()
{
  dxl.begin(DXL_BAUDRATE);
  dxl.setPortProtocolVersion(DXL_PROTOCOL_VERSION);
  myGripper.dxl_connection = dxl.ping(DXL_ID);
  if(myGripper.dxl_connection)
  {
    dxl.torqueOff(DXL_ID);
    dxl.setOperatingMode(DXL_ID, OP_POSITION);
    dxl.torqueOn(DXL_ID);

    dxl.writeControlTableItem(ControlTableItem::PROFILE_VELOCITY, DXL_ID, DXL_INITIAL_VELOCITY);
  }
  else
  {
    return false;
    Serial.println("Fail to connect DXL");
  }

  return true;
}

bool lsv_init()
{
  lsv.begin(32);
  lsv.GoalSpeed(LSV_ID, LINEAR_MAX_SPEED);
  myGripper.lsv_connection = lsv.ping(LSV_ID);
  
  if(myGripper.lsv_connection != 0xff)
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
  bool ret = false;

  bool dxl_state = dxl.ping(DXL_ID);
  myGripper.dxl_connection = dxl_state;
  if(dxl_state)
  {
    if(myGripper.dxl_connection != dxl_state)
    {
      dxl.setOperatingMode(DXL_ID, OP_POSITION);
      dxl.torqueOn(DXL_ID);
      dxl.writeControlTableItem(ControlTableItem::PROFILE_VELOCITY, DXL_ID, DXL_INITIAL_VELOCITY);
      Serial.printf("DXL Reconnected\r\n");
      ret = true;
    }
  }
  else
  {
    // Serial.printf("DXL Not Connected\r\n");
    ret = false;
  }  

  

  int lsv_state = lsv.ping(LSV_ID);
  myGripper.lsv_connection = lsv_state;
  if(lsv_state != 0x00)
  { 
    Serial.printf("LSV Reconnected\r\n");
    ret = true;
  }
  else
  {
    // Serial.printf("LSV Not Connected\r\n");

    ret = false;
  }

  return ret;
}


void serialCommand()
{
  if(millis() - serialLastTime > 1)
  {
    if(Serial.available())
    {
      int ch = Serial.read();

      if(ch != -1)
      {
        switch (ch)
        {
        case '1':
          Serial.println("1");
          myGripper.hall_toggle = !myGripper.hall_toggle;
          break;
        case '2':
          Serial.println("2");

          break;
        case '3':
          Serial.println("3");
          break;
        case '4':
          Serial.println("4");
          break;
        case '5':
          Serial.println("5");
          break;
        default:
          break;
        }
      }
    }
    serialLastTime = millis();
  }
}


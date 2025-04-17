#include <Adafruit_NeoPixel.h>
#include <Dynamixel2Arduino.h>
#include <MightyZap.h>
#include <Wire.h>
#include <ADS1115_WE.h>
#include <HardwareSerial.h>

#include "can.h"

#define DEVICE_ADDRESS    0x48
#define I2C1_SDA          PB7
#define I2C1_SCL          PA15

#define LED_BUILTIN       PC6
#define CAN_CH            _DEF_CAN1         

#define LED_PIN           PB6
#define NUMPIXELS         23

#define UART2_DE          PA1
#define UART2_TX          PA2
#define UART2_RX          PA3     


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

enum
{
  DISCONNECTED  = 0x00,
  CONNECTED     = 0x01,
  DXL_INIT      = 0x02,
  DXL_SUB       = 0x03,
  DXL_TARGET    = 0x04,
  
}status_t;


typedef struct __attribute__((packed))
{
  uint8_t  id;            // 1
  uint16_t position;      // 2
  uint16_t velocity;      // 2
  bool     status;        // 1
}dxl_t;

typedef struct __attribute__((packed))
{
  uint8_t  id;            // 1
  uint16_t position;      // 2
  uint16_t speed;         // 2
  int      status;        // 4
}lsv_t;

typedef struct __attribute__((packed))
{
  bool    status;         // 1
  uint8_t toggleSwitch;   // 1
  float   voltage;        // 4
  int16_t raw;            // 2
}ads_t; 

typedef struct
{
  uint8_t red;
  uint8_t green;
  uint8_t blue;
  uint8_t brightness; 
}color_t;

typedef union
{
  color_t pixelColor;
  uint8_t pixelColorArray[4];
}led_color_t;

typedef struct __attribute__((packed))
{
  uint8_t      ledSwitch;      // 1
  led_color_t  colors;         // 4
}led_t;



typedef struct __attribute__((packed))
{
  dxl_t    dxl;             // 6
  lsv_t    lsv;             // 9
  ads_t    hallSensor;      // 8
  led_t    led;             // 5

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

bool can_ret = false;
uint32_t can_index = 0;
uint32_t tx_err_cnt = 0;
uint32_t rx_err_cnt = 0;


enum{
  CAN_MODE = 0b0000'0000,
  ADS_MODE = 0b0000'0010
}Mode;

uint8_t select_mode = 0x00; 

HardwareSerial UART2(UART2_RX, UART2_TX);
Dynamixel2Arduino dxl(UART2, UART2_DE);

Mightyzap lsv(&UART2, UART2_DE);
ADS1115_WE adc = ADS1115_WE(DEVICE_ADDRESS);
Adafruit_NeoPixel pixels(NUMPIXELS, LED_PIN, NEO_GRB + NEO_KHZ800);

using namespace ControlTableItem;

void serialCommand();
void canReceive(uint8_t ch);
void canSend(uint8_t ch, can_msg_t msg);

bool i2cReconnect(uint8_t address);
bool ads1115_init();
bool dxl_init();
bool lsv_init();
bool motor_init();
bool motor_reconnect();
void led_init();
void led_setColor(uint8_t color[3], uint8_t brightness);


void setup() {
  Serial.begin(115200);
  pinMode(LED_BUILTIN, OUTPUT); // PC6
  canInit();
  can_ret = canOpen(_DEF_CAN1, CAN_NORMAL, CAN_FD_NO_BRS, CAN_500K, CAN_2M);
  led_init();
  delay(1000);

  ads1115_init();
  motor_init();
  myGripper.hallSensor.toggleSwitch = false;
  myGripper.led.ledSwitch = false;

  uint8_t colorGreen[3] = {0, 255, 0};
  led_setColor(colorGreen, 5);
  
  delay(1000);
  
  pixels.clear();
  pixels.show();
}

void loop() {
  curMillis = millis();

  serialCommand();
  canReceive(CAN_CH);

  switch (select_mode) {
    case ADS_MODE:
    
    break;
  }

  // // MOTOR
  if(curMillis - lastMillis[0] >= 100)
  {
    lastMillis[0] = curMillis;

    if(myGripper.dxl.status >= CONNECTED)
    {
      dxl.setGoalPosition(DXL_ID, myGripper.dxl.position);
      // Serial.printf("0x%02X\r\n", myGripper.dxl.position);
    }
  
    if(myGripper.lsv.status >= CONNECTED)
    {
      lsv.GoalPosition(LSV_ID, myGripper.lsv.position);
      // Serial.printf("0x%02X\r\n", myGripper.lsv.position);
    }
  }
  else   // Toque off
  {
    // Serial.printf("Toque Off\r\n");
  }


  // HALL SENSOR
  if((curMillis - lastMillis[1] >= 100) && myGripper.hallSensor.toggleSwitch && myGripper.hallSensor.status)
  {
    lastMillis[1] = curMillis;
    myGripper.hallSensor.voltage = adc.getResult_V();
    myGripper.hallSensor.raw = adc.getRawResult();
    if(myGripper.hallSensor.raw != 0)
    {
      Serial.printf("Result => %0.2f [%d]\r\n", myGripper.hallSensor.voltage, myGripper.hallSensor.raw);
    }
  }

  // INDICATOR
  if(curMillis - lastMillis[2] >= 1000)
  {
    lastMillis[2] = curMillis;
    digitalWrite(LED_BUILTIN, led_state);
    led_state = !led_state;

    if(myGripper.led.ledSwitch == DISCONNECTED)
    {
      uint8_t lightOff[3] = {0, 0, 0};
      led_setColor(lightOff, 0);
    }

    Serial.printf("DXL:%d[%d]| LSV:0x%02X[%d]| HALL:%d| %02X,%02X,%02X\r\n", 
    myGripper.dxl.status, 
    myGripper.dxl.position,
    myGripper.lsv.status, 
 
    myGripper.lsv.position,
    myGripper.hallSensor.status, 
    myGripper.led.colors.pixelColor.red,
    myGripper.led.colors.pixelColor.green,
    myGripper.led.colors.pixelColor.blue );

    can_msg_t msg;
    
    msg.frame   = CAN_FD_NO_BRS;
    msg.id_type = CAN_STD;
    msg.dlc     = CAN_DLC_32;
    msg.id      = 0x314;
    msg.length  = 32;
    for (int i=0; i < msg.length; i++) {
      msg.data[i] = i;
    }
    // canSend(CAN_CH, msg);
  }

  if(myGripper.led.ledSwitch == CONNECTED)
  {
    uint8_t lightOn[3] = {255, 255, 255};
    led_setColor(lightOn, 20);
  }


  // RECONNECT
  if(curMillis - lastMillis[3] >= 2000)
  {
    lastMillis[3] = curMillis;
    motor_reconnect();
    myGripper.hallSensor.status = i2cReconnect(DEVICE_ADDRESS);
  }

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
          myGripper.hallSensor.toggleSwitch = !myGripper.hallSensor.toggleSwitch;
          break;
        case '2':
          Serial.println("2");
          myGripper.led.ledSwitch = !myGripper.led.ledSwitch;
          break;
        case '3':
          Serial.println("3");
          // if(myGripper.dxl.status == CONNECTED)
          // {
            switch (myGripper.dxl.position) 
            {
              case DXL_INITIAL_POSITION:
                myGripper.dxl.position = DXL_SUB_POSITION;
              break;

              case DXL_SUB_POSITION:
                myGripper.dxl.position = DXL_TARGET_POSITION;
                break;

              case DXL_TARGET_POSITION:
                myGripper.dxl.position = DXL_INITIAL_POSITION;
                break;
            }
          // }
          
          break;
        case '4':
          Serial.println("4");
          switch (myGripper.lsv.position) 
          {
            case LINEAR_INITIAL_POSITION:
              myGripper.lsv.position = LINEAR_PUSH_POSITION;
            break;

            case LINEAR_PUSH_POSITION:
              myGripper.lsv.position = LINEAR_RELEASE_POSITION;
              break;

            case LINEAR_RELEASE_POSITION:
              myGripper.lsv.position = LINEAR_INITIAL_POSITION;
              break;
          }
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

void canReceive(uint8_t ch)
{
  // Receive 
  //
  if (canMsgAvailable(ch))
  {
    can_msg_t msg;

    canMsgRead(ch, &msg);

    can_index %= 1000;
    Serial.printf("ch %d %03d(R) <- id ",ch, can_index++);
    if (msg.frame != CAN_CLASSIC)
    {
      Serial.printf("fd ");
    }
    else
    {
      Serial.printf("   ");
    }        
    if (msg.id_type == CAN_STD)
    {
      Serial.printf("std ");
    }
    else
    {
      Serial.printf("ext ");
    }
    Serial.printf(": 0x%08X, L:%02d, ", msg.id, msg.length);
    for (int i=0; i<msg.length; i++)
    {
      Serial.printf("0x%02X ", msg.data[i]);
    }
    Serial.println();
  }

  
  if (tx_err_cnt != canGetTxErrCount(ch) ||
      rx_err_cnt != canGetRxErrCount(ch))
  {
    Serial.printf("ch %d ErrCnt : R %d, T %d\n", ch, canGetRxErrCount(ch), canGetTxErrCount(ch));
  }
  tx_err_cnt = canGetTxErrCount(ch);
  rx_err_cnt = canGetRxErrCount(ch);


  if (canUpdate())
  {
    Serial.printf("ch %d BusOff Recovery\n", ch);
  }    
}

void canSend(uint8_t ch, can_msg_t msg)
{
  
  if (canMsgWrite(ch, &msg, 10) > 0)
  {
    // can_index %= 1000;
    // Serial.printf("ch %d %03d(T) -> id ", ch, can_index++);
    // if (msg.frame != CAN_CLASSIC)
    // {
    //   Serial.printf("fd ");
    // }
    // else
    // {
    //   Serial.printf("   ");
    // }

    // if (msg.id_type == CAN_STD)
    // {
    //   Serial.printf("std ");
    // }
    // else
    // {
    //   Serial.printf("ext ");
    // }
    // Serial.printf(": 0x%08X, L:%02d, ", msg.id, msg.length);
    // for (int i=0; i<msg.length; i++)
    // {
    //   Serial.printf("0x%02X ", msg.data[i]);
      
    // }
    // Serial.println();
  }
}


bool i2cReconnect(uint8_t address)
{
  Wire.beginTransmission(address);
  if(Wire.endTransmission()==0)
  {
    // Serial.println("I2C already connected");
    return true;
  }
  else
  {
    // Serial.println("Disconnected");
    ads1115_init();
    return false;
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
  myGripper.dxl.status = dxl_init();
  myGripper.lsv.status = lsv_init();
  Serial.printf("%d | %d\r\n", myGripper.dxl.status, myGripper.lsv.status);

  if(myGripper.dxl.status == CONNECTED && myGripper.lsv.status == CONNECTED)
  {
    Serial.println("Motor Begin");
    myGripper.dxl.position = DXL_INITIAL_POSITION;
    myGripper.lsv.position = LINEAR_INITIAL_POSITION;
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
  myGripper.dxl.status = dxl.ping(DXL_ID);
  if(myGripper.dxl.status)
  {
    dxl.torqueOff(DXL_ID);
    dxl.setOperatingMode(DXL_ID, OP_POSITION);
    dxl.torqueOn(DXL_ID);

    // dxl.writeControlTableItem(ControlTableItem::PROFILE_VELOCITY, DXL_ID, DXL_INITIAL_VELOCITY);
    dxl.writeControlTableItem(PROFILE_VELOCITY, DXL_ID, DXL_INITIAL_VELOCITY);
    Serial.println("DXL Connected");
  }
  else
  {
    Serial.println("Fail to connect DXL");
    return false;    
  }

  return true;
}

bool lsv_init()
{
  lsv.begin(32);
  lsv.GoalSpeed(LSV_ID, LINEAR_MAX_SPEED);
  lsv.GoalPosition(LSV_ID, LINEAR_INITIAL_POSITION);
  myGripper.lsv.status = lsv.ping(LSV_ID);
  
  Serial.printf("0x%02X LSV PING\r\n", lsv.getTxRxStatus());
  // if(myGripper.lsv.status == 0xff)
  // {
  //   Serial.printf("LinearServo Connected\r\n");
  //   lsv.GoalPosition(LSV_ID, LINEAR_INITIAL_POSITION);
  // }
  // else
  // {
  //   Serial.printf("LinearServo Not Connected\r\n");
  //   return false;
  // }

  return true;
}

bool motor_reconnect()
{
  bool ret = false;
  uint8_t dxl_state = 0; 
  uint8_t lsv_state = 0;


  dxl_state = dxl.ping(DXL_ID); // current
  if(dxl_state)
  {
    if(myGripper.dxl.status != dxl_state)   // state change
    {
      dxl.setOperatingMode(DXL_ID, OP_POSITION);
      dxl.torqueOn(DXL_ID);
      // dxl.writeControlTableItem(ControlTableItem::PROFILE_VELOCITY, DXL_ID, DXL_INITIAL_VELOCITY);
      dxl.writeControlTableItem(PROFILE_VELOCITY, DXL_ID, DXL_INITIAL_VELOCITY);
      Serial.printf("DXL Reconnected\r\n");
    }
    ret = true;
  }
  else
  {
    // Serial.printf("DXL Not Connected\r\n");
    ret = false;
  }  
  myGripper.dxl.status = dxl_state;
  
  lsv_state = lsv.getModelNumber(LSV_ID); // 한번 저장되어서 안변함

  if(lsv_state != 0x00)
  {
    Serial.printf("LSV Once Connected 0x%02X\r\n", lsv_state);
  }
  else
  {
    Serial.printf("LSV NOT Conneceted 0x%02X\r\n", lsv_state);
  }

  // lsv_state = lsv.ping(LSV_ID);
  // if(lsv_state != 0x00)
  // {
  //   if(myGripper.lsv.status != lsv_state)
  //   { 
  //     Serial.printf("LSV Reconnected\r\n");
  //   }
  //   ret = true;
  // }
  // else
  // {
  //   Serial.printf("LSV Not Connected 0x%02X\r\n", lsv_state);

  //   ret = false;
  // }
  // myGripper.lsv.status = lsv_state;

  return ret;
}






void led_init()
{
  pixels.begin();
  pixels.clear();
  pixels.setBrightness(1);
  for (int i=0; i<NUMPIXELS; i++) {
    pixels.setPixelColor(i, pixels.Color(255, 0, 0));
  }
  pixels.show();
}


void led_setColor(uint8_t color[3], uint8_t brightness)
{
  pixels.setBrightness(brightness);
  for (int i=0; i<NUMPIXELS; i++) {
    pixels.setPixelColor(i, pixels.Color(color[0], color[1], color[2]));
  }
  memcpy(myGripper.led.colors.pixelColorArray, color, sizeof(color));
  myGripper.led.colors.pixelColor.brightness  = brightness;

  pixels.show();
}
#include <Adafruit_NeoPixel.h>
#include <Dynamixel2Arduino.h>
#include <MightyZap.h>
#include <Wire.h>
#include <ADS1115_WE.h>
#include <HardwareSerial.h>

#include "can.h"
#include "button.h"

#define DEVICE_ADDRESS    0x48
#define I2C1_SDA          PB7
#define I2C1_SCL          PA15

// #define LED_BUILTIN       PC6  // stm32g474ceu6
#define LED_BUILTIN       PC13    // stm32g474cet6
#define CAN_CH            _DEF_CAN1

#define USER_KEY          PB2
#define LED_PIN           PB6
#define NUMPIXELS         23

#define UART2_DE          PA1
#define UART2_TX          PA2
#define UART2_RX          PA3     


#define DXL_ID                1
#define DXL_BAUDRATE          57600
#define DXL_INITIAL_VELOCITY  100
#define DXL_MAX_POSITION      4095
#define DXL_TARGET_POSITION   3397
#define DXL_SUB_POSITION      2600
#define DXL_INITIAL_POSITION  2000

#define LSV_ID                  0
#define LSV_BAUDRATE            32
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

enum
{
  REQUEST           = 0x01,
  RESPONSE          = 0x02,
  MOTOR_DXL_RUN     = 0x03,
  MOTOR_DXL_ARRIVED = 0x04,
  MOTOR_LSV_RUN     = 0x05,
  MOTOR_LSV_ARRIVED = 0x06,
  HALL_SENSOR_RUN   = 0x07,
  HALL_SENSOR_STOP  = 0x08,
  LED_ON            = 0x09,
  LED_OFF           = 0x0A,
  CAN_START         = 0x1F,
  CAN_STOP          = 0x0F,
  RESTART           = 0x2F
}cmd_state_t;

typedef struct __attribute__((packed))
{
  uint8_t count;
  uint8_t command;

}cmd_t; 

typedef struct __attribute__((packed))
{
  uint8_t  id;            // 1
  uint16_t position;      // 2
  uint16_t velocity;      // 2
  uint8_t  status;        // 1
}dxl_t;

typedef struct __attribute__((packed))
{
  uint8_t  id;            // 1
  uint16_t position;      // 2
  uint16_t speed;         // 2
  uint8_t  status;        // 1
}lsv_t;

typedef struct __attribute__((packed))
{
  uint8_t toggleSwitch;   // 1
  int16_t raw;            // 2
  uint8_t status;         // 1
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
  uint8_t      ledSwitch;      // 1 status
  led_color_t  colors;         // 4
}led_t;





typedef struct __attribute__((packed))
{
  cmd_t      cmd;             // 2   0~1
  dxl_t      dxl;             // 6   2~7
  lsv_t      lsv;             // 6   8~13
  ads_t      hall;            // 4   14~17
  led_t      led;             // 5   18~22
  uint8_t    crc;             // 1   23

}gripper_t;

typedef union
{
  gripper_t gripper;
  uint8_t datas[sizeof(gripper_t)];

}gripper_packet_t;

gripper_packet_t sendPacket;
gripper_packet_t recvPacket;
gripper_t *recvGripper = &recvPacket.gripper;
gripper_t *sendGripper = &sendPacket.gripper;
volatile bool recvInUse = false;

bool canFlag     = false;
bool requestFlag = false;
bool dxlFlag     = false;
bool lsvFlag     = false;
bool hallFlag    = false;
bool ledFlag     = false;
bool restart     = false;


uint32_t serialLastTime = 0;
uint32_t curMillis = 0;
uint32_t lastMillis[8] = {0,};
uint32_t canMillis = 0;
int count = 0;

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


bool myCanCallback(uint8_t ch, CanEvent_t evt, can_msg_t *msg)
{
  if (evt == CAN_EVT_MSG && msg != NULL)
  {
    // Process received message
    Serial.print("CAN RX: ID=0x");
    Serial.print(msg->id, HEX);
    Serial.print(" Len=");
    Serial.print(msg->length);
    Serial.print(" Data=");
    for (int i = 0; i < msg->length; i++)
    {
      Serial.printf(" %02X", msg->data[i]);
    }
    Serial.println();

    memcpy(recvPacket.datas, msg->data, sizeof(gripper_t));


    switch (recvGripper->cmd.command) {
      case CAN_START:
        canFlag = true;
        break;
      case CAN_STOP:
        canFlag = false;
        break;
      case REQUEST:
        requestFlag = true;
        break;
      case  MOTOR_DXL_RUN:
        dxlFlag = true;
        break;
      case MOTOR_LSV_RUN:
        lsvFlag = true;
        break;
      case HALL_SENSOR_RUN:
        hallFlag = true;
        break;
      case HALL_SENSOR_STOP:
        hallFlag = false;
        break;
      case LED_ON:
        ledFlag = true;
        break;
      case LED_OFF:
        ledFlag = false;
        break;
      case RESTART:
        restart = true;
        break;
    }
    
    
    // if (recvGripper->cmd.command == REQUEST)
    // {
    //   Serial.println("REQUEST CALLBACK");
    //   // memcpy(&sendPacket.gripper, &recvPacket.gripper, sizeof(gripper_t));
    //   // sendGripper->cmd.command = RESPONSE;
    // }
    // else if (recvGripper->cmd.command == REQUEST)
    // {
    //   Serial.println("MOTOR CALLBACK");
    // }

  }

  return true;  // Automatically remove from queue
}



void setup() {
  Serial.begin(115200);
  pinMode(LED_BUILTIN, OUTPUT);     // PC6
  pinMode(USER_KEY, INPUT_PULLUP);  // PB2
  // buttonInit();
  // recvGripper = &recvPacket.gripper;
  // sendGripper = &sendPacket.gripper;


  canInit();
  can_ret = canOpen(_DEF_CAN1, CAN_NORMAL, CAN_FD_NO_BRS, CAN_500K, CAN_2M);
  // can_ret = canOpen(_DEF_CAN1, CAN_NORMAL, CAN_FD_NO_BRS, CAN_250K, CAN_2M);
  led_init();

  
  delay(1000);

  ads1115_init();
  motor_init();

  // Init Packet
  sendGripper->cmd.command = 0xFF;
  sendGripper->hall.toggleSwitch = false;
  sendGripper->led.ledSwitch = false;


  uint16_t curPos = dxl.getPresentPosition(DXL_ID);
  sendGripper->crc = curPos & 0xFF;

  uint8_t colorGreen[3] = {0, 255, 0};
  led_setColor(colorGreen, 5);
  
  // canAttachRxInterrupt(CAN_CH, myCanCallback);   // Callback

  delay(1000);
  Serial.printf("Start Program\r\n");
  pixels.clear();
  pixels.show();

  // // CAN PING
  can_msg_t msg;
    
  msg.frame   = CAN_FD_NO_BRS;
  msg.id_type = CAN_STD;
  msg.dlc     = CAN_DLC_24;
  msg.id      = 0x0FF;
  msg.length  = 24;
  Serial.println("send CAN 5");
  memcpy(msg.data, sendPacket.datas, sizeof(gripper_t));
  canSend(CAN_CH, msg);
}



void loop() {
  curMillis = millis();
  canReceive(CAN_CH);


  if(requestFlag)
  {
    can_msg_t msg;
    
    msg.frame   = CAN_FD_NO_BRS;
    msg.id_type = CAN_STD;
    msg.id      = 0x1FF;
    msg.dlc     = CAN_DLC_24;
    msg.length  = 24;

    memset(sendPacket.datas, 0, sizeof(gripper_t));

    sendGripper->cmd.count = recvGripper->cmd.count;
    sendGripper->cmd.command = REQUEST + 0x01;
    sendGripper->lsv.position = lsv.presentPosition(LSV_ID);
    lsv.ledOn(LSV_ID, GREEN);
    sendGripper->crc = calcCRCSimple(recvGripper->cmd.count, sendGripper->cmd.command);
    memcpy(msg.data, sendPacket.datas, sizeof(gripper_t));

    if(sendGripper->cmd.command == RESPONSE) canSend(CAN_CH, msg);
    requestFlag = false;

    for(int i=0; i< sizeof(gripper_t); i++)
    {
      Serial.printf("0x%02X ", sendPacket.datas[i]);
    }
    Serial.println();
  }

  // MOTOR
  if(dxlFlag)
  {
    if(sendGripper->dxl.status >= CONNECTED)
    {
      dxl.setGoalPosition(DXL_ID, recvGripper->dxl.position);
      
      Serial.printf("Target DXL: %4d\r\n", recvGripper->dxl.position);
    }
    else
    {
      Serial.printf("DXL Not Connected \r\n");
    }
    dxlFlag = false;
  }
  else if(lsvFlag)
  {
    lsv.GoalPosition(LSV_ID, recvGripper->lsv.position);
    Serial.printf("Target LSV: %4d\r\n", recvGripper->lsv.position);
    lsvFlag = false;
  }
  else if(curMillis - lastMillis[0] >= 100)
  {
    lastMillis[0] = curMillis;
    sendGripper->dxl.position = dxl.getPresentPosition(DXL_ID);
    // sendGripper->lsv.position = lsv.presentPosition(LSV_ID);
  }

  // HALL SENSOR
  if((curMillis - lastMillis[1] >= 50) && hallFlag)
  {
    lastMillis[1] = curMillis;
    sendGripper->hall.raw = adc.getRawResult();
    if(sendGripper->hall.raw != 0)
    {
      Serial.printf("Result => %0.2f [%d]\r\n", adc.getResult_V(), sendGripper->hall.raw);
    }
  }
  else if(curMillis - lastMillis[2] >= 10 && hallFlag == false)
  {
    lastMillis[2] = curMillis;
    sendGripper->hall.raw = 0;
  }

  // // INDICATOR
  // if(curMillis - lastMillis[3] >= 1000)
  // {
  //   lastMillis[3] = curMillis;
  //   digitalWrite(LED_BUILTIN, !digitalRead(LED_BUILTIN));
  //   count++;
  //   if(sendGripper->led.ledSwitch == DISCONNECTED)
  //   {
  //     uint8_t lightOff[3] = {0, 0, 0};
  //     led_setColor(lightOff, 0);
  //   }
  //   Serial.printf("cur: %0.2f\r\n", dxl.getPresentPosition(DXL_ID));

  //   // Serial.printf("[%d] DXL:%d[%d]| LSV:0x%02X[%d]| HALL:%d[%d]| [%d] %02X,%02X,%02X \r\n", 
  //   // count,
  //   // myGripper->dxl.status, 
  //   // myGripper->dxl.position,
  //   // myGripper->lsv.status, 
  //   // myGripper->lsv.position,
  //   // myGripper->hall.status,
  //   // myGripper->hall.raw,
  //   // myGripper->led.ledSwitch,
  //   // myGripper->led.colors.pixelColor.red,
  //   // myGripper->led.colors.pixelColor.green,
  //   // myGripper->led.colors.pixelColor.blue
  //   // );
  //   // Serial.printf("CUR %5.2f\r\n", dxl.getPresentPosition(DXL_ID));
    
  //   // RCC_PeriphCLKInitTypeDef clkconf;
  //   // // RCC_PeriphCLKFreqTypeDef clkfreq;

  //   // HAL_RCCEx_GetPeriphCLKConfig(&clkconf);
  //   // clkconf.PeriphClockSelection = RCC_PERIPHCLK_FDCAN;

  //   // Serial.printf("[%d] %d | %d | %d | %d  %d\r\n", 
  //   // count++,
  //   // HAL_RCC_GetSysClockFreq(),
  //   // HAL_RCCEx_GetPeriphCLKFreq(RCC_PERIPHCLK_FDCAN),
  //   // // __HAL_RCC_GET_SYSCLK_SOURCE()==RCC_SYSCLKSOURCE_STATUS_PLLCLK?"PLL":"NOT",
  //   // RCC_SYSCLKSOURCE_STATUS_HSE,
  //   // GetFdcanBaudRate(CAN_CH),
  //   // GetFdcanDataPhaseBaudRate(CAN_CH)
  //   // );




  if(ledFlag && recvGripper->led.ledSwitch == CONNECTED)
  {
    sendGripper->led.ledSwitch = CONNECTED;
    // uint8_t lightOn[3] = {255, 255, 255};
    // led_setColor(lightOn, 20);
    led_setColorArray(recvGripper->led.colors.pixelColorArray);
    recvGripper->led.ledSwitch = DISCONNECTED;  // for only once
    Serial.printf("%d %d %d %d\r\n", recvGripper->led.colors.pixelColor.red, 
                                     recvGripper->led.colors.pixelColor.green, 
                                     recvGripper->led.colors.pixelColor.blue, 
                                     recvGripper->led.colors.pixelColor.brightness);
  }
  else if(ledFlag == false && sendGripper->led.ledSwitch == CONNECTED) // toggle when has led Connected state
  {
    sendGripper->led.ledSwitch = DISCONNECTED;
    uint8_t lightOff[3] = {0, 0, 0};
    led_setColor(lightOff, 0);
    Serial.printf("LED Turn OFF\r\n");
   
  }


  // Debug
  if(curMillis - lastMillis[3] >= 5)
  {
    serialCommand();
    lastMillis[3] = curMillis;
  }
  else
  {
    if(curMillis - lastMillis[4] >= 2000)
    {
      lastMillis[4] = curMillis;
      digitalWrite(LED_BUILTIN, !digitalRead(LED_BUILTIN));
      // Serial.printf("0x%02X [%s]\r\n", recvGripper->cmd.command, requestFlag?"TRUE":"FALSE");
      // requestFlag = false;
      motor_reconnect();
      sendGripper->hall.status = i2cReconnect(DEVICE_ADDRESS);

      uint8_t offset = 3;
      // float current = dxl.getPresentCurrent(DXL_ID);
      // if(current > 2000)
      // {
      //   dxl.torqueOff(DXL_ID);
      // }
      // else
      // {
      //   dxl.torqueOn(DXL_ID);
      // }
      // float currentDXLPosition = dxl.getPresentPosition(DXL_ID);
      if(sendGripper->dxl.position + offset >= recvGripper->dxl.position && sendGripper->dxl.position - offset <= recvGripper->dxl.position)
      {
        // Serial.printf("DXL Target Arrived\r\n");
        sendGripper->cmd.command = MOTOR_DXL_ARRIVED;
        // canSend(CAN_CH, msg);
      }
      if(!hallFlag)
      {
        // int currentLSVPosition = lsv.presentPosition(LSV_ID);
        if(sendGripper->lsv.position + offset >= recvGripper->lsv.position && sendGripper->lsv.position - offset <= recvGripper->lsv.position)
        {
          // Serial.printf("LSV Target Arrived\r\n");
        }
        else
        {
          sendGripper->lsv.position = lsv.presentPosition(LSV_ID);
          Serial.printf("LSV Target Moving\r\n");
        }
        // Serial.printf("DXL:%0.2f->%d | LSV:%d->%d\r\n",  sendGripper->dxl.position, recvGripper->dxl.position, sendGripper->lsv.position, recvGripper->lsv.position);

        // Serial.printf("CAN => %d %d \r\n ", GetFdcanBaudRate(CAN_CH), GetFdcanDataPhaseBaudRate(CAN_CH));
      }

      // can_msg_t msg;
      
      // msg.frame   = CAN_FD_NO_BRS;
      // msg.id_type = CAN_STD;
      // msg.id      = 0x567;
      // msg.dlc     = CAN_DLC_24;
      // msg.length  = 24;
      // canSend(CAN_CH, msg);
    }
  }



  if(canFlag)
  {
    if(curMillis - lastMillis[5]  >= 50)
    {
      lastMillis[5] = curMillis;

      can_msg_t msg;
      
      msg.frame   = CAN_FD_NO_BRS;
      msg.id_type = CAN_STD;
      msg.id      = 0x123;
      msg.dlc     = CAN_DLC_24;
      msg.length  = 24;

      memcpy(msg.data, sendPacket.datas, sizeof(gripper_t));
      canSend(CAN_CH, msg);
      // switch (recvGripper->cmd.command)
      // {
        
      //   case REQUEST:
      //   case MOTOR_DXL_RUN:
      //   case MOTOR_DXL_ARRIVED:
      //     memcpy(msg.data, sendPacket.datas, sizeof(gripper_t));
      //     canSend(CAN_CH, msg);
      //     break;

      //   case CAN_STOP:
      //     // Serial.printf("Not Send CAN Data\r\n");
      //     break;
      // }
    }
    canErrorCheck();
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
          recvGripper->hall.toggleSwitch = !recvGripper->hall.toggleSwitch;
          break;
        case '2':
          Serial.println("2");
          recvGripper->led.ledSwitch = !recvGripper->led.ledSwitch;
          break;
        case '3':
          Serial.println("3");
          // if(myGripper->dxl.status == CONNECTED)
          // {
            recvGripper->cmd.command = MOTOR_DXL_RUN;
            switch (recvGripper->dxl.position) 
            {
              case DXL_INITIAL_POSITION:
                recvGripper->dxl.position = DXL_SUB_POSITION;
              break;

              case DXL_SUB_POSITION:
                recvGripper->dxl.position = DXL_TARGET_POSITION;
                break;

              case DXL_TARGET_POSITION:
                recvGripper->dxl.position = DXL_INITIAL_POSITION;
                break;
              default:
                recvGripper->dxl.position = DXL_INITIAL_POSITION;
                break;
            }
          // }
          
          break;
        case '4':
          Serial.println("4");
          recvGripper->cmd.command = MOTOR_DXL_ARRIVED;
          switch (recvGripper->lsv.position) 
          {
            case LINEAR_INITIAL_POSITION:
              recvGripper->lsv.position = LINEAR_PUSH_POSITION;
            break;

            case LINEAR_PUSH_POSITION:
              recvGripper->lsv.position = LINEAR_RELEASE_POSITION;
              break;

            case LINEAR_RELEASE_POSITION:
              recvGripper->lsv.position = LINEAR_INITIAL_POSITION;
              break;
            default:
              recvGripper->lsv.position = LINEAR_INITIAL_POSITION;
              break;
          }
          break;
        case '5':
          Serial.println("5");
          can_msg_t msg;
    
          msg.frame   = CAN_FD_NO_BRS;
          msg.id_type = CAN_STD;
          msg.dlc     = CAN_DLC_24;
          msg.id      = 0x125;
          msg.length  = 24;
          Serial.println("send CAN 5");
          memcpy(msg.data, sendPacket.datas, sizeof(gripper_t));
          canSend(CAN_CH, msg);
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
  
    // if(recvGripper->cmd.command != msg.data[1])
    // {
    //   recvGripper->cmd.command = msg.data[1];      
    // }
    // else  // same command
    // {

    // }
    memcpy(recvPacket.datas, msg.data, sizeof(gripper_t));  // 
    
    switch (recvGripper->cmd.command) {
      case CAN_START:
        canFlag = true;
        break;
      case CAN_STOP:
        canFlag = false;
        break;
      case REQUEST:
        requestFlag = true;
        break;
      case  MOTOR_DXL_RUN:
        dxlFlag = true;
        break;
      case MOTOR_LSV_RUN:
        lsvFlag = true;
        break;
      case HALL_SENSOR_RUN:
        hallFlag = true;
        break;
      case HALL_SENSOR_STOP:
        hallFlag = false;
        break;
      case LED_ON:
        ledFlag = true;
        break;
      case LED_OFF:
        ledFlag = false;
        break;
      case RESTART:
        restart = true;
        break;
    }
    recvGripper->cmd.command = 0;

    // CRC 계산

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

  

}



void canSend(uint8_t ch, can_msg_t msg)
{
  
  if (canMsgWrite(ch, &msg, 10) > 0)
  {
#ifdef CAN_SEND_DEBUG
    can_index %= 1000;
    Serial.printf("ch %d %03d(T) -> id ", ch, can_index++);
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
#endif
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
  sendGripper->dxl.status = dxl_init();
  sendGripper->lsv.status = lsv_init();
  Serial.printf("%d | %d\r\n", sendGripper->dxl.status, sendGripper->lsv.status);

  if(sendGripper->dxl.status == CONNECTED && sendGripper->lsv.status == CONNECTED)
  {
    Serial.println("Motor Begin");
    sendGripper->dxl.position = DXL_INITIAL_POSITION;
    sendGripper->lsv.position = LINEAR_INITIAL_POSITION;

    dxl.setGoalPosition(DXL_ID, sendGripper->dxl.position);
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
  sendGripper->dxl.status = dxl.ping(DXL_ID);
  if(sendGripper->dxl.status)
  {
    dxl.torqueOff(DXL_ID);  // Toque off when configure
    dxl.setOperatingMode(DXL_ID, OP_POSITION);
    dxl.torqueOn(DXL_ID);
    // dxl.setOperatingMode(DXL_ID, OP_PWM)
    // dxl.writeControlTableItem(ControlTableItem::PROFILE_VELOCITY, DXL_ID, DXL_INITIAL_VELOCITY);
    dxl.writeControlTableItem(PROFILE_VELOCITY, DXL_ID, DXL_INITIAL_VELOCITY);
    // dxl.writeControlTableItem(DRIVE_MODE, DXL_ID, 0); // CCW
    // dxl.writeControlTableItem(DRIVE_MODE, DXL_ID, 1); // CW

    
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
  lsv.begin(LSV_BAUDRATE);
  lsv.GoalSpeed(LSV_ID, LINEAR_MAX_SPEED);
  lsv.GoalPosition(LSV_ID, LINEAR_INITIAL_POSITION);
  sendGripper->lsv.status = lsv.ping(LSV_ID);
  
  Serial.printf("0x%02X LSV PING\r\n", lsv.getTxRxStatus());
  // if(myGripper->lsv.status == 0xff)
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
    if(sendGripper->dxl.status != dxl_state)   // state change
    {
      // dxl.torqueOff(DXL_ID);
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
  sendGripper->dxl.status = dxl_state;
  // lsv.ledOn(LSV_ID, RED);
  // lsv_state = lsv.ledOn(LSV_ID);

  // Serial.printf("PING LSV %d %d\r\n", lsv_state, lsv.readint(LSV_ID, 0x81));
  // if(!hallFlag)
  // {
  //   if(lsv_state == 0)   // when Reconnect Turn off
  //   {
  //     if(sendGripper->lsv.status != lsv_state)
  //     {
        
  //       Serial.printf("LSV Reconnected\r\n");
  //       // lsv.ledOn(LSV_ID, RED); // Set to 1
  //     }
  //   }
  //   sendGripper->lsv.status = lsv_state;
  //   // Serial.printf("PING LSV %d\r\n", lsv_state);
  // }

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
  memcpy(sendGripper->led.colors.pixelColorArray, color, sizeof(color));
  sendGripper->led.colors.pixelColor.brightness  = brightness;

  pixels.show();
}


void led_setColorArray(uint8_t color[4])
{
  pixels.setBrightness(color[3]);
  for (int i=0; i<NUMPIXELS; i++) {
    pixels.setPixelColor(i, pixels.Color(color[0], color[1], color[2]));
  }
  memcpy(sendGripper->led.colors.pixelColorArray, color, sizeof(color));
  sendGripper->led.colors.pixelColor.brightness = color[3];

  pixels.show();
}


void canErrorCheck()
{
  if (tx_err_cnt != canGetTxErrCount(CAN_CH) ||
      rx_err_cnt != canGetRxErrCount(CAN_CH))
  {
    Serial.printf("ch %d ErrCnt : R %d, T %d\r\n", CAN_CH, canGetRxErrCount(CAN_CH), canGetTxErrCount(CAN_CH));
  }
  tx_err_cnt = canGetTxErrCount(CAN_CH);
  rx_err_cnt = canGetRxErrCount(CAN_CH);


  if (canUpdate())
  {
    Serial.printf("ch %d BusOff Recovery\r\n", CAN_CH);
  }
}

bool isCRCValid(gripper_t  *packet) {
  return calcCRCSimple(packet->cmd.count, packet->cmd.command) == packet->crc;
}

uint8_t calcCRCSimple(uint8_t count, uint8_t command) {
  return count ^ command;
}

uint8_t calcCRC(uint8_t* data, size_t len) {
  uint8_t crc = 0;
  for (size_t i = 0; i < len; i++) {
    crc ^= data[i];
  }
  return crc;
}
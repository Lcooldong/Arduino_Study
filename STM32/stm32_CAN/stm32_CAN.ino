#include "can.h"

#define LED_BUILTIN PC6
#define CAN_CH      _DEF_CAN1;

bool can_ret = false;
uint32_t can_index = 0;
uint32_t tx_err_cnt = 0;
uint32_t rx_err_cnt = 0;

void canReceive(uint8_t ch);
void canSend(uint8_t ch, can_msg_t msg);

void setup() 
{
  Serial.begin(115200);
  delay(1000);
  Serial.println("Start CAN Classic");
  canInit();
  can_ret = canOpen(_DEF_CAN1, CAN_NORMAL, CAN_CLASSIC, CAN_500K, CAN_2M);

  pinMode(LED_BUILTIN, OUTPUT);
}

void loop() 
{
  static uint32_t pre_time;

  bool    is_rx = false;
  uint8_t rx_data;
  uint8_t ch = CAN_CH;


  if (millis()-pre_time >= 500)
  {
    pre_time = millis();
    digitalWrite(LED_BUILTIN, !digitalRead(LED_BUILTIN)); 
  }

  if (Serial.available() > 0)
  {
    is_rx = true;
    rx_data = Serial.read();
  }

  if(is_rx)
  {
    switch (rx_data) {
    case '1':
      can_msg_t msg;
    
      msg.frame   = CAN_CLASSIC;
      msg.id_type = CAN_STD;
      msg.dlc     = CAN_DLC_8;
      msg.id      = 0x314;
      msg.length  = 8;
      for (int i=0; i<CAN_DLC_8; i++) {
        msg.data[i] = i;
      }
      canSend(ch, msg);
      break;
    case '2':
      Serial.printf("CPU CLOCK : %d\r\n", HAL_RCC_GetSysClockFreq());
      break;
    case '3':
      canInfoPrint(ch);
      Serial.printf("Print CAN Info\r\n");
      break;
    }
  }

  canReceive(ch);
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
  }
}



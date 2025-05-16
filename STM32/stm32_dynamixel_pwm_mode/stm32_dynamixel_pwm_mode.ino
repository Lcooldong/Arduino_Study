/*******************************************************************************
* Copyright 2016 ROBOTIS CO., LTD.
*
* Licensed under the Apache License, Version 2.0 (the "License");
* you may not use this file except in compliance with the License.
* You may obtain a copy of the License at
*
*     http://www.apache.org/licenses/LICENSE-2.0
*
* Unless required by applicable law or agreed to in writing, software
* distributed under the License is distributed on an "AS IS" BASIS,
* WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
* See the License for the specific language governing permissions and
* limitations under the License.
*******************************************************************************/

#include <Dynamixel2Arduino.h>
#include <HardwareSerial.h>

#define UART2_DE          PA1
#define UART2_TX          PA2
#define UART2_RX          PA3  
 

const uint8_t DXL_ID = 1;
const float DXL_PROTOCOL_VERSION = 2.0;


int pwmValue = 0;
int8_t pwmPercent =0;
uint32_t lastMillis = 0;

HardwareSerial UART2(UART2_RX, UART2_TX);
Dynamixel2Arduino dxl(UART2, UART2_DE);

//This namespace is required to use Control table item names
using namespace ControlTableItem;

void setup() {
  // put your setup code here, to run once:
  
  // Use UART port of DYNAMIXEL Shield to debug.
  Serial.begin(115200);
  
  // Set Port baudrate to 57600bps. This has to match with DYNAMIXEL baudrate.
  dxl.begin(57600);
  // Set Port Protocol Version. This has to match with DYNAMIXEL protocol version.
  dxl.setPortProtocolVersion(DXL_PROTOCOL_VERSION);
  // Get DYNAMIXEL information
  dxl.ping(DXL_ID);

  // Turn off torque when configuring items in EEPROM area
  dxl.torqueOff(DXL_ID);
  dxl.setOperatingMode(DXL_ID, OP_PWM);
  dxl.torqueOn(DXL_ID);
}

void loop() {
  // put your main code here, to run repeatedly:
   
  // Please refer to e-Manual(http://emanual.robotis.com) for available range of value. 
  // Set Goal PWM using RAW unit
  uint32_t currentMillis = millis();

  if(Serial.available())
  {
    int ch = Serial.read();

    switch(ch)
    {
      case '1':
        pwmValue += 10;
      break;
      case '2':
        pwmValue -= 10;
      break;
      case '3':
        pwmPercent += 1;
      break;
      case '4':
        pwmPercent -= 1;
      break;
    }
  }

  if(currentMillis - lastMillis >= 100)
  {
    lastMillis = currentMillis;
    // dxl.setGoalPWM(DXL_ID, pwmValue);
    dxl.setGoalPWM(DXL_ID, pwmPercent, UNIT_PERCENT);
    
    Serial.printf("%3.2f | %5.2f\r\n", dxl.getPresentPWM(DXL_ID), dxl.getPresentPWM(DXL_ID, UNIT_PERCENT));
  }
  
}

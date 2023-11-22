
#include "main.h"
//#define DEBUG
#define TCS3430
// #define STEPPER_MOTOR



void setup() {
  Serial.begin(115200);
  myNeopixel->InitNeopixel();
  outStrip->begin();
  extraLED->begin();
  initPacket(&dataToSend);
  initServo();
  myNeopixel->pickOneLED(0, myNeopixel->strip->Color(0, 255, 255), 50, 1);
  SetOutStripColor(extraLED ,0, extraLED->Color(0, 0, 0), 0, 1);
  SetOutStripColor(extraLED ,1, extraLED->Color(0, 0, 0), 0, 1);


#ifdef STEPPER_MOTOR
  initStepperMotor();
#endif
#ifdef TCS3430
  initTSC3430();
#endif
  SetOutStripColor(outStrip ,0, outStrip->Color(255, 0, 0), 1, 1);
  ledcWrite(COLOR_LED_CHANNEL, 0);
}

void loop() {
  getStatus(INTERVAL);

  if (millis() - colorSensorLastTime > COLOR_SENSOR_INTERVAL)
  {
    colorSensorLastTime = millis();
    //showColorData();  // Test
#ifdef TCS3430
    if(colorSensorFlash)
    {
      showColorData();    // 연결 전에는 작동 X
    dataToSend.colorState = COLOR_ON;
    }
    else
    {
      ledcWrite(COLOR_LED_CHANNEL, 0);
    dataToSend.colorState = COLOR_OFF;
    }
#endif

  }

  if(Serial.available())
  {
    char c = Serial.read();
    switch (c)
    {
    case 'i':      
      //Serial.write('a');
      //initPacket(&dataToSend);
      sendPacket((uint8_t*)&dataToSend, sizeof(dataToSend));
      //Serial.write(dataToSend);
      break;

    case 'o':
      if(dataToSend.hallState == HALL_ARRIVED)
      {
        
        gripperServo.attach(SERVO_PIN, 500, 2400);
        rotateServo(&gripperServo, SERVO_INITIAL_POS, 5);
        gripperServo.detach();
        //Serial.printf("gripper Pos open : %d\r\n", gripperPos);
        dataToSend.servoState = SERVO_OPENED;
        sendPacket((uint8_t*)&dataToSend, sizeof(dataToSend));
        SetOutStripColor(outStrip ,0, outStrip->Color(255, 0, 255), 5, 1);
        delay(100);
      }
      break;

    case 'c':
      if(dataToSend.hallState == HALL_ARRIVED)
      {
        gripperServo.attach(SERVO_PIN, 500, 2400);
        rotateServo(&gripperServo, SERVO_TARGET_POS, 10);
        gripperServo.detach();
        //Serial.printf("gripper Pos close : %d\r\n", gripperPos);
        dataToSend.servoState = SERVO_CLOSED;
        sendPacket((uint8_t*)&dataToSend, sizeof(dataToSend));
        SetOutStripColor(outStrip, 0, outStrip->Color(0, 0, 255), 5, 1);
        delay(100);
      }
      break;

    case '1':
#ifdef STEPPER_MOTOR
      moveStepperMotor(STEPS, FORWARD, STEP_DELAY);
#endif      
      SetOutStripColor(extraLED ,0, extraLED->Color(255, 255, 255), ++brightnessTestValue, 1);
      Serial.printf("Brightness : %d\r\n",brightnessTestValue);
      break;

    case '2':
#ifdef STEPPER_MOTOR
      moveStepperMotor(STEPS, BACKWARD, STEP_DELAY);
#endif
      SetOutStripColor(extraLED ,0, extraLED->Color(255, 255, 255), --brightnessTestValue, 1);
      Serial.printf("Brightness : %d\r\n",brightnessTestValue);
      break;

    case 'n':
      colorSensorFlash = true;
      break;

    case 'f':
      colorSensorFlash = false;
      break;

    case 'a':
      if(!buttonServo.attached())
      {
        buttonServo.attach(SERVO_PIN2, 500, 2400);
      }
      rotateServo(&buttonServo, 0, 10);
      dataToSend.buttonState = SERVO_RELEASE;
      buttonServo.detach();
      sendPacket((uint8_t*)&dataToSend, sizeof(dataToSend));
      SetOutStripColor(outStrip, 0, outStrip->Color(100, 0, 255), 5, 1);
      break;

    case 'b':
      buttonServo.attach(SERVO_PIN2, 500, 2400);
      rotateServo(&buttonServo, 30, 10);
      dataToSend.buttonState = SERVO_PUSH;
      sendPacket((uint8_t*)&dataToSend, sizeof(dataToSend));
      SetOutStripColor(outStrip, 0, outStrip->Color(100, 100, 50), 5, 1);
      break;

    default:
      break;
    }
    delay(1);
  }
}



void initPacket(PACKET* _packet)
{
  _packet->stx = 0x02;
  _packet->etx = 0x03;
}


bool sendPacket(uint8_t* _data, size_t len)
{

  for (int i = 0; i < sizeof(buf); i++)
  {
    //Serial.printf("0x%x \r\n", _data[i]);
    Serial.write(_data[i]);
  }
  

  return true;
}

void getStatus(int interval)
{
  if(millis() - lastTime > interval)
  {
    lastTime = millis();
    
    hallValue = analogRead(HALL_SENSOR_PIN);
#ifdef DEBUG
    Serial.printf("Value : %d\r\n", hallValue);
#endif
    if (hallValue <= HALL_TARGET_VALUE)
    {
      hallCount++;
      if(hallCount > 10)
      {
        //Serial.println("Arrived at Target Height");
        dataToSend.hallState = HALL_ARRIVED;
      }   
    }
    else if(hallValue <= HALL_MID_VALUE)
    {
      dataToSend.hallState = HALL_NEARBY;
    }
    else    
    {
      hallCount = 0;
      dataToSend.hallState = HALL_FAR;
    }

    //int fsrData =  analogRead(FSR_PIN);
    //Serial.printf("FSR : %d\r\n",fsrData);
    //Serial.printf("%d %d | HALL : %d\r\n", dataToSend.servoState, dataToSend.hallState, hallValue);
  }

}

void initServo()
{
  if(!gripperServo.attached())
  {
    gripperServo.setPeriodHertz(50);
    gripperServo.attach(SERVO_PIN, 500, 2400);
  }

  
  //rotateServo(&gripperServo, 0, 15);
  

 // buttonServo.setPeriodHertz(50);
 // buttonServo.attach(SERVO_PIN2, 500, 2400);
  if(!buttonServo.attached())
  {
   buttonServo.setPeriodHertz(50);
   buttonServo.attach(SERVO_PIN2, 500, 2400);
  }
  
  
  //rotateServo(&buttonServo, 0, 10);
  //Serial.println("Init Button");
  gripperServo.write(0);
  delay(500);
  buttonServo.write(0);
  //Serial.println("Init Gripper");
}



void rotateServo(Servo *_servo, int targetPos, uint32_t millisecond)
{     
      int pos;
      delay(10);

      if(_servo == &gripperServo)
      {
        //Serial.println("Gripper!");
        pos = gripperPos;
      }
      else
      {
        //Serial.println("Button!");
        pos = buttonPos;
      }

      if (pos != targetPos)
      {
        //Serial.print("Servo Rotate Start\r\n");

        if(pos < targetPos)
        {
          myNeopixel->pickOneLED(0, myNeopixel->strip->Color(0, 255, 0), 5, 50);
          for (int i = 0; i <= targetPos; i++)
          {
            //gripperServo.write(i);
            _servo->write(i);
            pos = i;
            //Serial.printf("Degree : %d\r\n", i);
            delay(millisecond);
          }  
        }
        else if (pos > targetPos)
        {
          myNeopixel->pickOneLED(0, myNeopixel->strip->Color(255, 0, 0), 5, 50);
          for (int i = pos; i >= targetPos; i--)
          {
            //gripperServo.write(i);
            _servo->write(i);
            pos = i;
            //Serial.printf("Degree : %d\r\n", i);
            delay(millisecond);
          }
              
        }
        digitalWrite(SERVO_PIN, LOW);      // 끄기
        //Serial.printf("Servo Rotated\r\n");
        //_servo->detach();
        delay(10);
        if(_servo == &gripperServo)
        {
          //Serial.println("Gripper!");
          gripperPos = pos;
        }
        else
        {
          //Serial.println("Button!");
          buttonPos = pos;
        }
      }
}

void initStepperMotor()
{
  pinMode(EN_PIN, OUTPUT);
  pinMode(STEP_PIN, OUTPUT);
  pinMode(DIR_PIN, OUTPUT);
  digitalWrite(EN_PIN, HIGH);
}

void moveStepperMotor(int step, bool dir, int stepDelay)
{
  digitalWrite(EN_PIN, LOW);
  digitalWrite(DIR_PIN, dir);  // true 배선쪽
      
  for (uint16_t i = step; i>0; i--) 
  {
    digitalWrite(STEP_PIN, HIGH);
    delayMicroseconds(stepDelay);
    digitalWrite(STEP_PIN, LOW);
    delayMicroseconds(stepDelay);
  }
  digitalWrite(EN_PIN, HIGH);
}

void SetOutStripColor(Adafruit_NeoPixel* targetStrip ,uint8_t ledNum, uint32_t color, uint8_t brightness, int wait)
{
    targetStrip->setBrightness(brightness);
    targetStrip->setPixelColor(ledNum, color);
    targetStrip->show();                                               
    delay(wait);
}

void initTSC3430()
{
  Wire.begin(COLOR_SDA_PIN, COLOR_SDL_PIN);
  ledcSetup(COLOR_LED_CHANNEL, 5000, 8);
  ledcAttachPin(COLOR_LED_PIN, COLOR_LED_CHANNEL);

  while(!tcs3430.begin()){
    Serial.println("Please check that the IIC device is properly connected");
    delay(1000);
  }
}

void showColorData()
{
  //uint16_t XData = tcs3430.getXData();
  uint16_t YData = tcs3430.getYData();
  //uint16_t ZData = tcs3430.getZData();
 // uint16_t IR1Data = tcs3430.getIR1Data();
  //uint16_t IR2Data = tcs3430.getIR2Data();

#ifdef DEBUG
  //Serial.printf("Y Value : %d\r\n", YData);

  //String str = "X : " + String(XData) + "    Y : " + String(YData) + "    Z : " +  String(ZData) + "    IR1 : "+String(IR1Data) + "    IR2 : "+String(IR2Data);
  //Serial.println(str);
#endif

  if(YData >= COLOR_Y_MAX_VALUE)
  {
    YData = COLOR_Y_MAX_VALUE;
  }
  else if(YData <= COLOR_Y_MIN_VALUE)
  {
    YData = COLOR_Y_MIN_VALUE;
  }
  int pwmValue = map(YData, COLOR_Y_MAX_VALUE, COLOR_Y_MIN_VALUE, 0, 255);
  int neopixelValue = map(pwmValue, 0, 255, 0, 150);
  //Serial.println(pwmValue);
  ledcWrite(COLOR_LED_CHANNEL, pwmValue);
  SetOutStripColor(extraLED, 0, extraLED->Color(255, 255, 255), neopixelValue, 1);
  SetOutStripColor(extraLED, 1, extraLED->Color(255, 255, 255), neopixelValue, 1);
}


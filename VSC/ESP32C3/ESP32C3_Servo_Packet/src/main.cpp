#include "main.h"

void setup() {
  Serial.begin(115200);
  myNeopixel->InitNeopixel();
  outStrip->begin();
  initPacket(&dataToSend);
  initServo();
  initStepperMotor();
  //initTSC3430();
  SetOutStripColor(0, outStrip->Color(255, 0, 0), 50, 1);
}

void loop() {
  getStatus(INTERVAL); 

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
        rotateServo(SERVO_INITIAL_POS);
        dataToSend.servoState = SERVO_OPENED;
        sendPacket((uint8_t*)&dataToSend, sizeof(dataToSend));
        SetOutStripColor(0, outStrip->Color(255, 0, 255), 50, 1);
        delay(100);
      }
      break;

    case 'c':
      if(dataToSend.hallState == HALL_ARRIVED)
      {
        rotateServo(SERVO_TARGET_POS);
        dataToSend.servoState = SERVO_CLOSED;
        sendPacket((uint8_t*)&dataToSend, sizeof(dataToSend));
        SetOutStripColor(0, outStrip->Color(0, 0, 255), 50, 1);
        delay(100);
      }
      break;

    case 'f':
      moveStepperMotor(STEPS, FORWARD, STEP_DELAY);
      break;

    case 'b':
      moveStepperMotor(STEPS, BACKWARD, STEP_DELAY);
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
    //Serial.printf("Value : %d\r\n", hallValue);
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
    //showColorData();    // 연결 전에는 작동 X
    int fsrData =  analogRead(FSR_PIN);
    //Serial.printf("FSR : %d\r\n",fsrData);
    //Serial.printf("%d %d | HALL : %d\r\n", dataToSend.servoState, dataToSend.hallState, hallValue);
  }

}

void initServo()
{
  if(!gripperServo.attached())
  {
    gripperServo.setPeriodHertz(50);
    gripperServo.attach(SERVO_PIN, 1000, 2000);
  }
  gripperServo.write(0);
}

void rotateServo(int targetPos)
{
      if (pos != targetPos)
      {
        //Serial.print("Servo Rotate Start\r\n");

        if(pos < targetPos)
        {
          myNeopixel->pickOneLED(0, myNeopixel->strip->Color(0, 255, 0), 50, 50);
          for (int i = 0; i <= targetPos; i++)
          {
            gripperServo.write(i);
            pos = i;
            //Serial.printf("Degree : %d\r\n", i);
            delay(4);
          }  
        }
        else if (pos > targetPos)
        {
          myNeopixel->pickOneLED(0, myNeopixel->strip->Color(255, 0, 0), 50, 50);
          for (int i = pos; i >= targetPos; i--)
          {
            gripperServo.write(i);
            pos = i;
            //Serial.printf("Degree : %d\r\n", i);
            delay(4);
          }    
        }
              
        //Serial.printf("Servo Rotated\r\n");
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

void SetOutStripColor(uint8_t ledNum, uint32_t color, uint8_t brightness, int wait)
{
    outStrip->setBrightness(brightness);
    outStrip->setPixelColor(ledNum, color);  
    outStrip->show();                                               
    delay(wait);
}

void initTSC3430()
{
  Wire.begin(COLOR_SDA_PIN, COLOR_SDL_PIN);
  ledcSetup(COLOR_LED_CHANNEL, 5000, 8);
  ledcAttachPin(COLOR_LED_PIN, COLOR_LED_CHANNEL);

  while(!TCS3430.begin()){
    Serial.println("Please check that the IIC device is properly connected");
    delay(1000);
  }
}

void showColorData()
{
  uint16_t XData = TCS3430.getXData();
  uint16_t YData = TCS3430.getYData();
  uint16_t ZData = TCS3430.getZData();
  uint16_t IR1Data = TCS3430.getIR1Data();
  uint16_t IR2Data = TCS3430.getIR2Data();
  String str = "X : " + String(XData) + "    Y : " + String(YData) + "    Z : " +  String(ZData) + "    IR1 : "+String(IR1Data) + "    IR2 : "+String(IR2Data);
  //Serial.println(str);
  if(ZData >= 1800)
  {
    ZData = 1800;
  }
  else if(ZData <= 80)
  {
    ZData = 80;
  }
  int pwmValue = map(ZData, 1800, 80, 0, 100);
//  Serial.println(pwmValue);
  ledcWrite(COLOR_LED_CHANNEL, pwmValue);
}


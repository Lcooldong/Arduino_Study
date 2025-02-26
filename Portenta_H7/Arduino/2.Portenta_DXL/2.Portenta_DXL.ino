#include <RPC.h>
#include <Queue.h>

#define RS485          Serial1 
#define RS485_BAUDRATE 57600

uint32_t preMillis[2] = {0,};
uint8_t leds[] = {LEDR, LEDG, LEDB};

uint8_t ledrValue = 0;
bool ledToggle = false;
int count = 0;

void setup() {
  Serial.begin(115200);
  RS485.begin(RS485_BAUDRATE);
  Serial3.begin(115200);
  for (int i = 0; i < sizeof(leds); i++)
  {
    pinMode(leds[i], OUTPUT);
  }
  

  RPC.begin();

#ifdef CORE_CM7

  for(int i=0; i< sizeof(leds); i++)
  {
    digitalWrite(leds[i], HIGH);
  }
 digitalWrite(leds[0], LOW);
#endif

#ifdef CORE_CM4
#endif
  

  for (uint32_t i = 0; i < sizeof(preMillis)/sizeof(preMillis[0]); i++)
  {
    preMillis[i] = millis();
  }
  digitalWrite(leds[0], HIGH);
  digitalWrite(leds[1], LOW);
}

void loop() {

#ifdef CORE_CM7
 
  uint32_t curMillis = millis();
  if(curMillis - preMillis[0] >= 500)
  {
    preMillis[0] = curMillis;
    
    digitalWrite(leds[1], ledToggle);
    ledToggle = !ledToggle;
    // Serial.println(ledToggle);
  }
  if(curMillis - preMillis[1] >= 1000)
  {
    preMillis[1] = curMillis;
    count++;
    Serial.println((String)"UART : " + count);
    RS485.println((String)"UART1 : " + count);

    Serial3.println((String)"UART3 : " + count);
  }
  if(Serial3.available()>0)
  {
    char ch = Serial3.read();
    Serial.println((String)"Return : " + ch);
    // char buf[4] = {0,};
    // Serial3.readBytes(buf, sizeof(buf));
    // for(int i=0; i<sizeof(buf); i++)
    // {
    //   Serial.print(buf[i]);
    // }
    // Serial.println("\r\n-----------------");
  }
  if(Serial.available() > 0)
  {
    char c = Serial.read();
    Serial.println((String)"UART : " + c);
  }
  
#endif

#ifdef CORE_CM4

#endif
  
}


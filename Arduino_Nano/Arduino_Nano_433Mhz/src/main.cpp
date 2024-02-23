#include <Arduino.h>
#include <VirtualWire.h>

//#define SENDER
#define RECEIVER

#define DATA_PIN 3
#define BUFFER_SIZE 1


byte TX_BUFFER[BUFFER_SIZE] = {0};
uint64_t senderTime = 0;

byte message[VW_MAX_MESSAGE_LEN];
byte messageLength = VW_MAX_MESSAGE_LEN;
int receivedNumber = 0;


int count = 0;
uint64_t blinkTime = 0;
bool LED_BUILTIN_STATE = false;

void setup() {
  Serial.begin(115200);
  pinMode(LED_BUILTIN, OUTPUT);

#ifdef SENDER
  vw_set_tx_pin(DATA_PIN);
  vw_setup(2000);

  for (int i = 0; i < BUFFER_SIZE; i++)
  {
    TX_BUFFER[i] = 0;
  }
  
#endif

#ifdef RECEIVER
  digitalWrite(LED_BUILTIN, LOW);
  vw_set_rx_pin(3);
  vw_setup(2000); // bps
  vw_rx_start();

#endif


}

void loop() {

  if(millis() - blinkTime > 1000)
  {

#ifdef SENDER
      Serial.print(count++);
      Serial.print(" : ");
      Serial.println(TX_BUFFER[0]);
#endif

#ifdef RECEIVER
      Serial.print(count++);
      Serial.print(" : ");
      Serial.println(receivedNumber);
#endif

      blinkTime = millis();


#ifdef SENDER
      digitalWrite(LED_BUILTIN, LED_BUILTIN_STATE);
      LED_BUILTIN_STATE = !LED_BUILTIN_STATE;
#endif

  }


#ifdef SENDER
  if(millis() - senderTime > 10)
  {
    TX_BUFFER[0] = LED_BUILTIN_STATE;
    vw_send(TX_BUFFER, BUFFER_SIZE);
    vw_wait_tx();
    senderTime = millis();
  }
#endif


#ifdef RECEIVER
  if(vw_get_message(message, &messageLength))
  {
    Serial.print("Value : ");

    for (int i = 0; i < messageLength; i++)
    {
      receivedNumber = message[i];
    }

    Serial.println(receivedNumber);
    digitalWrite(LED_BUILTIN, receivedNumber);
    
  }

#endif



 
}


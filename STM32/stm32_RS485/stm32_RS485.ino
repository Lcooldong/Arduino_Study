#include "HardwareSerial.h"
#include "RS485.h"      // https://github.com/RobTillaart/RS485

#define UART2_DE      PA1
#define UART2_TX      PA2
#define UART2_RX      PA3

#define SERIAL_TX_BUFFER_SIZE 1024
#define SERIAL_RX_BUFFER_SIZE 1024

#define DEVICE_ID     0

#define LED_BUILTIN   PC6
#define LED_INTERVAL  100


uint32_t lastMillis[2] = {0,};
bool led_state = false;
uint32_t count = 0;
bool while_recv = false;

HardwareSerial UART2(UART2_RX, UART2_TX);
RS485 rs485(&UART2, UART2_DE, DEVICE_ID);



void setup() {
  Serial.begin(115200);
  UART2.begin(57600);
  pinMode(LED_BUILTIN, OUTPUT);

}

void loop() {
  uint32_t curMillis = millis();
  if(curMillis - lastMillis[0] >= LED_INTERVAL)
  {
    lastMillis[0] = curMillis;
    while_recv = false;
    digitalWrite(LED_BUILTIN, led_state);
    led_state = !led_state;
    count++;
    rs485.printf("CNT : %d\n\r", count);
    // UART2.printf("CNT : %d\n\r", count);
  }
  else
  {
    while_recv = true;
  }

  if(rs485.available() && while_recv)
  {
    int text = rs485.read();
    delay(1);
    rs485.printf("RECV : 0x%02X\r\n", text);
    
  }

  if(UART2.available()>0)
  {
    int ch = UART2.read();
    delay(1);
    // UART2.printf("RECV : 0x%02X\r\n", ch);
  }
}

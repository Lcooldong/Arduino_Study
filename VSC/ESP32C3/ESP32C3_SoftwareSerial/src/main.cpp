#include <Arduino.h>

#include <SoftwareSerial.h>

#define MYPORT_TX 7
#define MYPORT_RX 6

EspSoftwareSerial::UART myPort;

void setup(){
  Serial.begin(115200); // Standard hardware serial port

  myPort.begin(115200, SWSERIAL_8N1, MYPORT_RX, MYPORT_TX, false);
  if (!myPort) { // If the object did not initialize, then its configuration is invalid
    Serial.println("Invalid EspSoftwareSerial pin configuration, check config"); 
    while (1) { // Don't continue with invalid configuration
      delay (1000);
    }
  }

}


void loop(){
  myPort.printf("Hello SoftwareSerial\r\n");
  delay(2000);

}

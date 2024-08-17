#include <Arduino.h>
#include <Dynamixel2Arduino.h>

#define DXL_SERIAL Serial3
const int DXL_DIR_PIN = 2;

const uint8_t DXL_ID = 1;
const float DXL_PROTOCOL_VERSION = 2.0;

// Dynamixel2Arduino dxl(DXL_SERIAL, DXL_DIR_PIN);
Dynamixel2Arduino dxl(DXL_SERIAL);
using namespace ControlTableItem;

void setup() {
  Serial.begin(115200);
  pinMode(LED_BUILTIN, OUTPUT);
  // Set Port baudrate to 57600bps. This has to match with DYNAMIXEL baudrate.
  dxl.begin(57600);
  // Set Port Protocol Version. This has to match with DYNAMIXEL protocol version.
  dxl.setPortProtocolVersion(DXL_PROTOCOL_VERSION);
  // Get DYNAMIXEL information
  dxl.ping(DXL_ID);
}

void loop() {
  dxl.ledOn(DXL_ID);
  digitalWrite(LED_BUILTIN, HIGH);
  delay(500);
  // Turn off the LED on DYNAMIXEL
  dxl.ledOff(DXL_ID);
  digitalWrite(LED_BUILTIN, LOW);
  delay(500);
}


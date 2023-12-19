#include <Arduino.h>
#include <Wire.h>
#include <SPI.h>

#include "AS5600.h"

#define SDA_PIN 18
#define SCL_PIN 19
#define AS5600_DIR_PIN 10

// //  setDirection
// const uint8_t AS5600_CLOCK_WISE         = 0;  //  LOW
// const uint8_t AS5600_COUNTERCLOCK_WISE  = 1;  //  HIGH

// //  0.087890625;
// const float   AS5600_RAW_TO_DEGREES     = 360.0 / 4096;
// //  0.00153398078788564122971808758949;
// const float   AS5600_RAW_TO_RADIANS     = PI * 2.0 / 4096;
// //  4.06901041666666e-6
// const float   AS5600_RAW_TO_RPM         = 1.0 / 4096 / 60;

// //  getAngularSpeed
// const uint8_t AS5600_MODE_DEGREES       = 0;
// const uint8_t AS5600_MODE_RADIANS       = 1;
// const uint8_t AS5600_MODE_RPM           = 2;

AS5600L as5600;

void setup() {
  Serial.begin(115200);
  //Wire.setPins(SDA_PIN, SCL_PIN);
  //Wire.begin();

  Serial.println(__FILE__);
  Serial.print("AS5600_LIB_VERSION: ");
  Serial.println(AS5600_LIB_VERSION);
  
  as5600.begin(AS5600_DIR_PIN);  //  set direction pin.
  as5600.setDirection(AS5600_CLOCK_WISE);  //  default, just be explicit.
  int b = as5600.isConnected();
  Serial.print("Connect: ");
  Serial.println(b);
  delay(1000);

}

void loop() {
//  Serial.print(as5600.readAngle());
//  Serial.print("\t");
//  Serial.println(as5600.rawAngle());
  //  Serial.println(as5600.rawAngle() * AS5600_RAW_TO_DEGREES);

  Serial.println(analogRead(4));
  delay(100);
}


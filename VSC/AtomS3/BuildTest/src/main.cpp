
#include <math.h>
#include <Arduino.h>
#include <WiFi.h>
#include <Wire.h>
#include <driver/ledc.h>
#include <driver/rmt.h>
// #include <ir_tools.h>

#include "M5GFX.h"
#include "M5Unified.h"
#include "I2C_MPU6886.h"
#include "MadgwickAHRS.h"
// put function declarations here:
int myFunction(int, int);

void setup() {
  // put your setup code here, to run once:
  int result = myFunction(2, 3);
  
}

void loop() {
  // put your main code here, to run repeatedly:
}

// put function definitions here:
int myFunction(int x, int y) {
  return x + y;
}
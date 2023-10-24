#include <Arduino.h>
#include <ESP32Servo.h>
#include <DFRobot_TCS3430.h>
#include <Wire.h>
#include "neopixel.h"

#define M5STAMP_C3
//#define ATOM_LITE

#define INTERVAL 50

// PINOUT | 6, 10 NOT ADC
#ifdef M5STAMP_C3
#define EN_PIN            0 // Enable
#define DIR_PIN          18  // Direction
#define STEP_PIN         19  // Step

#define SERVO_PIN         1
#define HALL_SENSOR_PIN   4

#define FSR_PIN           5
#define OUT_RGB_PIN       6

#define COLOR_SDL_PIN     7
#define COLOR_SDA_PIN     8
#define COLOR_LED_PIN     10

#endif

#ifdef ATOM_LITE
#define SERVO_PIN 19
#define HALL_SENSOR_PIN 33
#endif


#ifdef ATOM_LITE
#define HALL_MID_VALUE 2400
#define HALL_TARGET_VALUE 960
#define SERVO_INITIAL_POS 0
#define SERVO_TARGET_POS 150
#endif 

#ifdef M5STAMP_C3
#define COLOR_LED_CHANNEL 0
#define SERVO_INITIAL_POS 0
#define SERVO_TARGET_POS 150
#define HALL_MID_VALUE 2900
#define HALL_TARGET_VALUE 1305
#define STEPS 1600        // Full Range 10000
#define STEP_DELAY 100
#define INIT_STEP_TIMEOUT 3000
#endif

#define HALL_FAR     0x00
#define HALL_NEARBY  0x04
#define HALL_ARRIVED 0x05
#define SERVO_CLOSED 0x06
#define SERVO_OPENED 0x00

enum StepperDirection
{
  FORWARD = 0,
  BACKWARD = 1
};

//StepperDirection stepDir;

typedef struct __attribute__((packed)) packet
{
  uint8_t stx;
  uint8_t servoState;
  uint8_t hallState;
  uint8_t etx;
}PACKET;

PACKET dataToSend = {0,};
PACKET buf;

long count = 0;
int hallCount = 0;
int pos = 0;
uint64_t lastTime = 0;
uint16_t hallValue;

Adafruit_NeoPixel* outStrip = new Adafruit_NeoPixel(LED_COUNT, OUT_RGB_PIN, NEO_GRB + NEO_KHZ800);
MyNeopixel* myNeopixel = new MyNeopixel();
Servo gripperServo;
DFRobot_TCS3430 TCS3430;

void initPacket(PACKET* _packet);
bool sendPacket(uint8_t* _data, size_t len);
void getStatus(int interval);
void initServo();
void rotateServo(int targetPos);
void initStepperMotor();
void moveStepperMotor(int step, bool dir, int stepDelay);
void SetOutStripColor(uint8_t ledNum, uint32_t color, uint8_t brightness, int wait);
void initTSC3430();
void showColorData();
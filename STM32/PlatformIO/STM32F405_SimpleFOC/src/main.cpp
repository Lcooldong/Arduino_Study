#include <Arduino.h>
#include <SimpleFOC.h>


#define DEBUG

#ifdef DEBUG
#include <OneButton.h>
#define LED_BUILTIN PB2 // LED on board


#endif

// magnetic sensor instance - SPI
#define CS       PB2  // PA15?
#define SPI_SCK  PC10 // SPI3_SCK
#define SPI_MISO PC11 // SPI3_MISO
#define SPI_MOSI PC12 // SPI3_MOSI


// SHUNT SENSING
#define M0_IA _NC // Seulement 2 mesures de courant B&C, A = pas dispo lui.
#define M0_IB PC0
#define M0_IC PC1

// Odrive M0 motor pinout
#define M0_INH_A PA8
#define M0_INH_B PA9
#define M0_INH_C PA10
#define M0_INL_A PB13
#define M0_INL_B PB14
#define M0_INL_C PB15

// M1 & M2 common enable pin
#define EN_GATE PB12

//Pole pair
#define PP 14

//Temp
#define M0_TEMP PC5

// GPIO_3, GPIO_4
#define USART2_TX PA2 // TX
#define USART2_RX PA3 // RX



uint32_t currentMillis = 0;
uint32_t previousMillis[4] = {0,};
int counter = 0;
bool ledState = false;  

BLDCMotor motor = BLDCMotor(PP);
BLDCDriver6PWM driver = BLDCDriver6PWM(M0_INH_A, M0_INL_A, M0_INH_B, M0_INL_B, M0_INH_C, M0_INL_C, EN_GATE);
LowsideCurrentSense currentSense = LowsideCurrentSense(0.0005f, 10.0f, M0_IA, M0_IB, M0_IC);
MagneticSensorSPI sensor = MagneticSensorSPI(CS, 14, 0x3FFF);

HardwareSerial Serial2(USART2_RX, USART2_TX);
Commander command = Commander(Serial2);
Commander cmdDebug = Commander(Serial);
SPIClass SPI_3(SPI_SCK, SPI_MISO, SPI_MOSI);

PhaseCurrent_s current;
float current_magnitude;
float memory_max_current = motor.current_limit;
float target_position = 0;


void doMotor(char* cmd) {
  command.motor(&motor, cmd);
  //command.target(&motor, cmd); // ok
  //command.motion(&motor, cmd);
}

void doTarget(char* cmd) { command.scalar(&target_position, cmd); }
void doLimit(char* cmd) { command.scalar(&motor.voltage_limit, cmd); }
void doVelocity(char* cmd) { command.scalar(&motor.velocity_limit, cmd); }


#ifdef DEBUG
OneButton button;


void buttonClick() {
  Serial.println("Button Clicked!");
}
#endif

void setup() {
  Serial.begin(115200);
  Serial.println("SimpleFOC STM32F405 Example");
#ifdef DEBUG
  pinMode(LED_BUILTIN, OUTPUT);
  // pinMode(PC13, INPUT_PULLDOWN); // Button
  button.setup(PC13, INPUT_PULLDOWN, false);
  button.attachClick(buttonClick);
#endif
  
  SimpleFOCDebug::enable(&Serial);
  driver.voltage_power_supply = 24;
  driver.voltage_limit = 20;

  if(!driver.init())
  {
    Serial.println("Driver init failed!");
    return;
  }
  motor.linkDriver(&driver);
  motor.voltage_limit = 24;
  motor.velocity_limit = 10;
  motor.current_limit = 1.0f;
  motor.phase_resistance = 0.035;
  motor.controller = MotionControlType::angle_openloop;

  motor.useMonitoring(Serial);

  if(!motor.init()){
    Serial.println("Motor init failed!");
    return;
  }

  command.add('T', doTarget, "target angle");
  command.add('L', doLimit,  "voltage limit");
  command.add('V', doLimit,  "movement velocity");

  Serial.println("Motor ready!");
  Serial.println("Set target position [rad]");
  _delay(1000);
  // sensor.init(&SPI_3);
  // motor.linkSensor(&sensor);

  // driver.pwm_frequency = 20000;        // 20kHz
  // driver.voltage_power_supply = 24.0f; // 24V
  // driver.voltage_limit = 56.0f;        // 56V
  
  // driver.init();
  // motor.linkDriver(&driver);

  // motor.torque_controller = TorqueControlType::foc_current; // Use FOC current control
  // motor.controller = MotionControlType::angle; // Use angle control
  // motor.foc_modulation = FOCModulationType::SinePWM; // Use Sine PWM modulation 
  // motor.modulation_centered = true; // Center modulation

  // motor.PID_velocity.P = 2.0f; // Velocity controller P gain
  // motor.PID_velocity.I = 30.0f; // Velocity controller I gain
  // motor.PID_velocity.D = 0.001f; // Velocity controller D gain
  // motor.PID_velocity.output_ramp = 1000.0f; // Velocity controller output ramp
  // motor.LPF_velocity.Tf = 0.01f; // Velocity controller low pass filter time



  // SimpleFOCDebug::enable(&Serial2);
  // motor.init();
  // currentSense.linkDriver(&driver);
  // if(currentSense.init() == 0) {
  //   Serial2.println("Current sense init failed!");
  // } else {
  //   Serial2.println("Current sense init success!");
  // }

  // currentSense.skip_align = true; // Skip the alignment phase
  // motor.linkCurrentSense(&currentSense);

  // motor.initFOC();
  // command.decimal_places = 4; // Set number of decimal places for command output
  // command.add('M', doMotor, "motor exemple ==> M10");
  // motor.target = 0;
  // if (motor.motor_status != 4) { // 0 - fail initFOC
  //   Serial2.println("ERROR:" + String(motor.motor_status));
  //   //return;
  // }

  // delay(1000);
}

void loop() {
  currentMillis = millis();

  if(Serial.available()) {
    char c = Serial.read();
    Serial.printf("Received: %c\r\n", c);
    switch (c)
    {
    case 'a':
      
      break;
    
    default:
      break;
    }
  }

  if(currentMillis - previousMillis[0] >= 500) {
    previousMillis[0] = currentMillis;
    counter++;
    Serial.printf("[%d] %d\r\n", counter, HSE_VALUE);
#ifdef DEBUG
    digitalWrite(LED_BUILTIN, ledState);
#endif
    ledState = !ledState;
  
  }
  else if(currentMillis - previousMillis[1] >= 10) {
    previousMillis[1] = currentMillis;
    button.tick(); // Check button state
  }

  motor.move(target_position);
  command.run();


  // motor.loopFOC();
  // current = currentSense.getPhaseCurrents();
  // current_magnitude = currentSense.getDCCurrent();
  // command.run(Serial2);
  // motor.move();
}


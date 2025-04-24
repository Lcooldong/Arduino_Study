#define LED_BUILTIN   PC6
#define LED_INTERVAL  100

uint32_t lastMillis[2] = {0,};
bool led_state = false;
uint32_t count = 0;

void setup() {
  Serial.begin(115200);
  pinMode(LED_BUILTIN, OUTPUT);

}

void loop() {
  uint32_t curMillis = millis();
  if(curMillis - lastMillis[0] >= LED_INTERVAL)
  {
    lastMillis[0] = curMillis;
    digitalWrite(LED_BUILTIN, led_state);
    led_state = !led_state;
    count++;

    Serial.printf("Serial CNT : %d\n\r", count);

  }

}

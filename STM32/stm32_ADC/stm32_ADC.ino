#define PWM_PIN PB6
#define PWM_INTERVAL 5

uint8_t led_value = 0;
uint8_t led_dir   = 0;
uint32_t lastMillis = 0;

void setup() {
  Serial.begin(115200);

}

void loop() {
  uint32_t curMillis = millis();
  if( curMillis - lastMillis >= PWM_INTERVAL)
  {
    lastMillis = curMillis;
    if(led_dir == 1)
    {
      if(led_value == 0)
      {
        led_dir = 0;
      }
      else
      {
        led_value--;
      }
    }
    else
    {
      if(led_value == 255)
      {
        led_dir = 1;
      }
      else
      {
        led_value++;
      }
    }
    
    analogWrite(PWM_PIN, led_value);
    Serial.printf("%d\r\n", led_value);
  }
}

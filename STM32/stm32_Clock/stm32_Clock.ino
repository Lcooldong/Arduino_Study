// #include "stm32g4xx_hal.h"

// #define HSE_VALUE ((uint32_t)8000000U)
// #define HAL_FDCAN_MODULE_ENABLED


#define BUILTIN_LED PC13 //STM32G474CET6 -> Weact G474CE 로 실행가능
// #define BUILTIN_LED PC6





uint32_t currentMiilis = 0;
uint32_t lastMillis = 0;
int count = 0;
bool ledState = false;

// When MCU STOP ->  STLINK Connect with pushing Reset Button. Erase Full Flash

void setup() {
  Serial.begin(115200);
  pinMode(BUILTIN_LED, OUTPUT);


}

void loop() {
  currentMiilis = millis();
  if(currentMiilis - lastMillis >= 1000)
  {
    lastMillis  = currentMiilis;
    count++;
    Serial.printf("[%04d] %d MHz| %d MHz| %s | %d MHz| %d MHz| [Time:%d]\r\n", 
      count, HAL_RCC_GetSysClockFreq() / 1000000,
      RCC_SYSCLKSOURCE_STATUS_HSE,
      __HAL_RCC_GET_SYSCLK_SOURCE()==RCC_SYSCLKSOURCE_STATUS_PLLCLK?"PLL":"NOT"
      , HAL_RCC_GetHCLKFreq() / 1000000,
      HAL_RCCEx_GetPeriphCLKFreq((RCC_PERIPHCLK_USB))/ 1000000,
      millis()
    );
    digitalWrite(BUILTIN_LED, !digitalRead(BUILTIN_LED));
  }
}

// WeactG474 는 160MHz 로 동작  -> pF capacitor check 필요
void SystemClock_Config(void)
{
  RCC_OscInitTypeDef RCC_OscInitStruct = {0};
  RCC_ClkInitTypeDef RCC_ClkInitStruct = {0};

  /** Configure the main internal regulator output voltage
  */
  HAL_PWREx_ControlVoltageScaling(PWR_REGULATOR_VOLTAGE_SCALE1_BOOST);

  /** Initializes the RCC Oscillators according to the specified parameters
  * in the RCC_OscInitTypeDef structure.
  */
  RCC_OscInitStruct.OscillatorType = RCC_OSCILLATORTYPE_HSI48|RCC_OSCILLATORTYPE_HSE;
  RCC_OscInitStruct.HSEState = RCC_HSE_ON;
  RCC_OscInitStruct.HSI48State = RCC_HSI48_ON;
  RCC_OscInitStruct.PLL.PLLState = RCC_PLL_ON;
  RCC_OscInitStruct.PLL.PLLSource = RCC_PLLSOURCE_HSE;
  RCC_OscInitStruct.PLL.PLLM = RCC_PLLM_DIV1;
  RCC_OscInitStruct.PLL.PLLN = 40;
  RCC_OscInitStruct.PLL.PLLP = RCC_PLLP_DIV2;
  RCC_OscInitStruct.PLL.PLLQ = RCC_PLLQ_DIV4;
  RCC_OscInitStruct.PLL.PLLR = RCC_PLLR_DIV2;
  if (HAL_RCC_OscConfig(&RCC_OscInitStruct) != HAL_OK)
  {
    Error_Handler();
  }

  /** Initializes the CPU, AHB and APB buses clocks
  */
  RCC_ClkInitStruct.ClockType = RCC_CLOCKTYPE_HCLK|RCC_CLOCKTYPE_SYSCLK
                              |RCC_CLOCKTYPE_PCLK1|RCC_CLOCKTYPE_PCLK2;
  RCC_ClkInitStruct.SYSCLKSource = RCC_SYSCLKSOURCE_PLLCLK;
  RCC_ClkInitStruct.AHBCLKDivider = RCC_SYSCLK_DIV1;
  RCC_ClkInitStruct.APB1CLKDivider = RCC_HCLK_DIV1;
  RCC_ClkInitStruct.APB2CLKDivider = RCC_HCLK_DIV1;

  if (HAL_RCC_ClockConfig(&RCC_ClkInitStruct, FLASH_LATENCY_4) != HAL_OK)
  {
    Error_Handler();
  }
}


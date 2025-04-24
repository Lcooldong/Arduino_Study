#include "HardwareSerial.h"
#include "RS485.h"      // https://github.com/RobTillaart/RS485

#define UART2_DE      PA1
#define UART2_TX      PA2
#define UART2_RX      PA3

#define SERIAL_TX_BUFFER_SIZE 1024
#define SERIAL_RX_BUFFER_SIZE 1024

#define DEVICE_ID     0

#define LED_BUILTIN   PC6
#define LED_INTERVAL  1000


uint32_t lastMillis[2] = {0,};
bool led_state = false;
uint32_t count = 0;
bool while_recv = false;

HardwareSerial UART2(UART2_RX, UART2_TX);
RS485 rs485(&UART2, UART2_DE, DEVICE_ID);

void initClock();

void setup() {
  Serial.begin(115200);
  UART2.begin(115200);
  
  pinMode(LED_BUILTIN, OUTPUT);

  delay(1000);
  initClock();
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
    rs485.printf("RS485 CNT : %d\n\r", count);
    Serial.printf("Serial CNT : %d\n\r", count);
    // UART2.printf("CNT : %d\n\r", count);
  }
  else
  {
    while_recv = true;
  }

  if(rs485.available() && while_recv)
  {
    int text = rs485.read();
    //delay(1);
    rs485.printf("RECV : 0x%02X\r\n", text);
    Serial.printf("RECV : 0x%02X\r\n", text);
  }

  // if(UART2.available()>0)
  // {
  //   int ch = UART2.read();
  //   delay(1);
  //   // UART2.printf("RECV : 0x%02X\r\n", ch);
  // }
}

void initClock()
{
  Serial.printf("%dMHz SYS:%d\r\n", RCC_SYSCLKSOURCE_STATUS_HSE, HAL_RCC_GetSysClockFreq());

}



// // G431
// void SystemClock_Config(void)
// {
//   RCC_OscInitTypeDef RCC_OscInitStruct = {0};
//   RCC_ClkInitTypeDef RCC_ClkInitStruct = {0};
//   RCC_PeriphCLKInitTypeDef PeriphClkInit = {0};

//   /** Configure the main internal regulator output voltage
//   */
//   HAL_PWREx_ControlVoltageScaling(PWR_REGULATOR_VOLTAGE_SCALE1_BOOST);
//   /** Configure LSE Drive Capability
//   */
//   HAL_PWR_EnableBkUpAccess();
//   __HAL_RCC_LSEDRIVE_CONFIG(RCC_LSEDRIVE_LOW);
//   /** Initializes the RCC Oscillators according to the specified parameters
//   * in the RCC_OscInitTypeDef structure.
//   */
//   RCC_OscInitStruct.OscillatorType = RCC_OSCILLATORTYPE_HSI48|RCC_OSCILLATORTYPE_HSE
//                                 |RCC_OSCILLATORTYPE_LSE;
//   RCC_OscInitStruct.HSEState = RCC_HSE_ON;
//   RCC_OscInitStruct.LSEState = RCC_LSE_ON;
//   RCC_OscInitStruct.HSI48State = RCC_HSI48_ON;
//   RCC_OscInitStruct.PLL.PLLState = RCC_PLL_ON;
//   RCC_OscInitStruct.PLL.PLLSource = RCC_PLLSOURCE_HSE;
//   RCC_OscInitStruct.PLL.PLLM = RCC_PLLM_DIV1;
//   RCC_OscInitStruct.PLL.PLLN = 40;
//   RCC_OscInitStruct.PLL.PLLP = RCC_PLLP_DIV2;
//   RCC_OscInitStruct.PLL.PLLQ = RCC_PLLQ_DIV4;
//   RCC_OscInitStruct.PLL.PLLR = RCC_PLLR_DIV2;
//   if (HAL_RCC_OscConfig(&RCC_OscInitStruct) != HAL_OK)
//   {
//     Error_Handler();
//   }
//   /** Initializes the CPU, AHB and APB buses clocks
//   */
//   RCC_ClkInitStruct.ClockType = RCC_CLOCKTYPE_HCLK|RCC_CLOCKTYPE_SYSCLK
//                               |RCC_CLOCKTYPE_PCLK1|RCC_CLOCKTYPE_PCLK2;
//   RCC_ClkInitStruct.SYSCLKSource = RCC_SYSCLKSOURCE_PLLCLK;
//   RCC_ClkInitStruct.AHBCLKDivider = RCC_SYSCLK_DIV1;
//   RCC_ClkInitStruct.APB1CLKDivider = RCC_HCLK_DIV1;
//   RCC_ClkInitStruct.APB2CLKDivider = RCC_HCLK_DIV1;

//   if (HAL_RCC_ClockConfig(&RCC_ClkInitStruct, FLASH_LATENCY_4) != HAL_OK)
//   {
//     Error_Handler();
//   }
//   // /** Initializes the peripherals clocks
//   // */
//   // PeriphClkInit.PeriphClockSelection = RCC_PERIPHCLK_RTC|RCC_PERIPHCLK_I2C1
//   //                             |RCC_PERIPHCLK_USB|RCC_PERIPHCLK_FDCAN;
//   // PeriphClkInit.I2c1ClockSelection = RCC_I2C1CLKSOURCE_PCLK1;
//   // PeriphClkInit.FdcanClockSelection = RCC_FDCANCLKSOURCE_PLL;
//   // PeriphClkInit.UsbClockSelection = RCC_USBCLKSOURCE_HSI48;
//   // PeriphClkInit.RTCClockSelection = RCC_RTCCLKSOURCE_LSE;

//   // if (HAL_RCCEx_PeriphCLKConfig(&PeriphClkInit) != HAL_OK)
//   // {
//   //   Error_Handler();
//   // }
// }
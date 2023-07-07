/* USER CODE BEGIN Header */
/**
 ******************************************************************************
 * @file           : main.c
 * @brief          : Main program body
 ******************************************************************************
 * @attention
 *
 * Copyright (c) 2023 STMicroelectronics.
 * All rights reserved.
 *
 * This software is licensed under terms that can be found in the LICENSE file
 * in the root directory of this software component.
 * If no LICENSE file comes with this software, it is provided AS-IS.
 *
 ******************************************************************************
 */
/* USER CODE END Header */
/* Includes ------------------------------------------------------------------*/
#include "main.h"
#include "dma.h"
#include "spi.h"
#include "tim.h"
#include "usart.h"
#include "gpio.h"

/* Private includes ----------------------------------------------------------*/
/* USER CODE BEGIN Includes */

/* USER CODE END Includes */

/* Private typedef -----------------------------------------------------------*/
/* USER CODE BEGIN PTD */

/* USER CODE END PTD */

/* Private define ------------------------------------------------------------*/
/* USER CODE BEGIN PD */

/* USER CODE END PD */

/* Private macro -------------------------------------------------------------*/
/* USER CODE BEGIN PM */

/* USER CODE END PM */

/* Private variables ---------------------------------------------------------*/

/* USER CODE BEGIN PV */
unsigned int led_state = 0;
/* USER CODE END PV */

/* Private function prototypes -----------------------------------------------*/
void SystemClock_Config(void);
/* USER CODE BEGIN PFP */

/* USER CODE END PFP */

/* Private user code ---------------------------------------------------------*/
/* USER CODE BEGIN 0 */
int _write(int file, char *ptr, int len)
{
  HAL_UART_Transmit(&huart1, (const uint8_t *)ptr, len, HAL_MAX_DELAY);
  return len;
}

__STATIC_INLINE uint32_t GXT_SYSTICK_IsActiveCounterFlag(void)
{
  return ((SysTick->CTRL & SysTick_CTRL_COUNTFLAG_Msk) == (SysTick_CTRL_COUNTFLAG_Msk));
}
static uint32_t getCurrentMicros(void)
{
  /* Ensure COUNTFLAG is reset by reading SysTick control and status register */
  GXT_SYSTICK_IsActiveCounterFlag();
  uint32_t m = HAL_GetTick();
  const uint32_t tms = SysTick->LOAD + 1;
  __IO uint32_t u = tms - SysTick->VAL;
  if (GXT_SYSTICK_IsActiveCounterFlag())
  {
    m = HAL_GetTick();
    u = tms - SysTick->VAL;
  }
  return (m * 1000 + (u * 1000) / tms);
}
/* USER CODE END 0 */

/**
 * @brief  The application entry point.
 * @retval int
 */
int main(void)
{
  /* USER CODE BEGIN 1 */

  /* USER CODE END 1 */

  /* MCU Configuration--------------------------------------------------------*/

  /* Reset of all peripherals, Initializes the Flash interface and the Systick. */
  HAL_Init();

  /* USER CODE BEGIN Init */

  /* USER CODE END Init */

  /* Configure the system clock */
  SystemClock_Config();

  /* USER CODE BEGIN SysInit */

  /* USER CODE END SysInit */

  /* Initialize all configured peripherals */
  MX_GPIO_Init();
  MX_DMA_Init();
  MX_SPI1_Init();
  MX_USART1_UART_Init();
  MX_USART2_UART_Init();
  MX_TIM1_Init();
  /* USER CODE BEGIN 2 */

  printf("\r\n###################\r\n### Hello World.\r\n");

  HAL_TIM_Base_Start_IT(&htim1);

#ifdef DEBUG
  printf("[%.3f] main: Set AD9910 ports.\r\n", 1.0 * getCurrentMicros() / 1000.0);
#endif

  ad9910_PortSettingsTypeDef ad9910_settings;
  ad9910_settings.hspi = &hspi1;

  ad9910_settings.ioupdate_gpio_group = AD9910_IOUPDATE_GPIO_Port;
  ad9910_settings.ioupdate_gpio_pin = AD9910_IOUPDATE_Pin;

  ad9910_settings.reset_gpio_group = AD9910_RESET_GPIO_Port;
  ad9910_settings.reset_gpio_pin = AD9910_RESET_Pin;

  ad9910_settings.drctl_gpio_group = AD9910_DRC_GPIO_Port;
  ad9910_settings.drctl_gpio_pin = AD9910_DRC_Pin;

  ad9910_settings.drhold_gpio_group = AD9910_DRH_GPIO_Port;
  ad9910_settings.drhold_gpio_pin = AD9910_DRH_Pin;

  ad9910_settings.drover_gpio_group = AD9910_DRO_GPIO_Port;
  ad9910_settings.drover_gpio_pin = AD9910_DRO_Pin;

  ad9910_settings.osk_gpio_group = AD9910_OSK_GPIO_Port;
  ad9910_settings.osk_gpio_pin = AD9910_OSK_Pin;

  ad9910_settings.pf0_gpio_group = AD9910_PF0_GPIO_Port;
  ad9910_settings.pf0_gpio_pin = AD9910_PF0_Pin;

  ad9910_settings.pf1_gpio_group = AD9910_PF1_GPIO_Port;
  ad9910_settings.pf1_gpio_pin = AD9910_PF1_Pin;

  ad9910_settings.pf2_gpio_group = AD9910_PF2_GPIO_Port;
  ad9910_settings.pf2_gpio_pin = AD9910_PF2_Pin;

  ad9910_settings.rso_gpio_group = AD9910_RSO_GPIO_Port;
  ad9910_settings.rso_gpio_pin = AD9910_RSO_Pin;

  ad9910_settings.sync_gpio_group = AD9910_SYNC_GPIO_Port;
  ad9910_settings.sync_gpio_pin = AD9910_SYNC_Pin;

#ifdef DEBUG
  printf("[%.3f] main: Initialize AD9910.\r\n", 1.0 * getCurrentMicros() / 1000.0);
#endif

  ad9910_init(&ad9910_settings);

#ifdef DEBUG
  printf("[%.3f] main: Set AD9910 PLL.\r\n", 1.0 * getCurrentMicros() / 1000.0);
#endif

  ad9910_set_pll(1, 1, AD9910_REFCLK_OUTCURRENT_HIGH, AD9910_PLL_CHARGE_PUMP_CUR_237, 0, AD9910_VCO_SEL_5);

  /* USER CODE END 2 */

  /* Infinite loop */
  /* USER CODE BEGIN WHILE */
  while (1)
  {
    /* USER CODE END WHILE */

    /* USER CODE BEGIN 3 */
  }
  /* USER CODE END 3 */
}

/**
 * @brief System Clock Configuration
 * @retval None
 */
void SystemClock_Config(void)
{
  RCC_OscInitTypeDef RCC_OscInitStruct = {0};
  RCC_ClkInitTypeDef RCC_ClkInitStruct = {0};

  /** Configure the main internal regulator output voltage
   */
  __HAL_RCC_PWR_CLK_ENABLE();
  __HAL_PWR_VOLTAGESCALING_CONFIG(PWR_REGULATOR_VOLTAGE_SCALE2);

  /** Initializes the RCC Oscillators according to the specified parameters
   * in the RCC_OscInitTypeDef structure.
   */
  RCC_OscInitStruct.OscillatorType = RCC_OSCILLATORTYPE_HSE;
  RCC_OscInitStruct.HSEState = RCC_HSE_ON;
  RCC_OscInitStruct.PLL.PLLState = RCC_PLL_ON;
  RCC_OscInitStruct.PLL.PLLSource = RCC_PLLSOURCE_HSE;
  RCC_OscInitStruct.PLL.PLLM = 25;
  RCC_OscInitStruct.PLL.PLLN = 168;
  RCC_OscInitStruct.PLL.PLLP = RCC_PLLP_DIV2;
  RCC_OscInitStruct.PLL.PLLQ = 4;
  if (HAL_RCC_OscConfig(&RCC_OscInitStruct) != HAL_OK)
  {
    Error_Handler();
  }

  /** Initializes the CPU, AHB and APB buses clocks
   */
  RCC_ClkInitStruct.ClockType = RCC_CLOCKTYPE_HCLK | RCC_CLOCKTYPE_SYSCLK | RCC_CLOCKTYPE_PCLK1 | RCC_CLOCKTYPE_PCLK2;
  RCC_ClkInitStruct.SYSCLKSource = RCC_SYSCLKSOURCE_PLLCLK;
  RCC_ClkInitStruct.AHBCLKDivider = RCC_SYSCLK_DIV1;
  RCC_ClkInitStruct.APB1CLKDivider = RCC_HCLK_DIV2;
  RCC_ClkInitStruct.APB2CLKDivider = RCC_HCLK_DIV1;

  if (HAL_RCC_ClockConfig(&RCC_ClkInitStruct, FLASH_LATENCY_2) != HAL_OK)
  {
    Error_Handler();
  }

  /** Enables the Clock Security System
   */
  HAL_RCC_EnableCSS();
}

/* USER CODE BEGIN 4 */

/* USER CODE END 4 */

/**
 * @brief  This function is executed in case of error occurrence.
 * @retval None
 */
void Error_Handler(void)
{
  /* USER CODE BEGIN Error_Handler_Debug */
  /* User can add his own implementation to report the HAL error return state */
  __disable_irq();
  while (1)
  {
  }
  /* USER CODE END Error_Handler_Debug */
}

#ifdef USE_FULL_ASSERT
/**
 * @brief  Reports the name of the source file and the source line number
 *         where the assert_param error has occurred.
 * @param  file: pointer to the source file name
 * @param  line: assert_param error line source number
 * @retval None
 */
void assert_failed(uint8_t *file, uint32_t line)
{
  /* USER CODE BEGIN 6 */
  /* User can add his own implementation to report the file name and line number,
     ex: printf("Wrong parameters value: file %s on line %d\r\n", file, line) */
  /* USER CODE END 6 */
}
#endif /* USE_FULL_ASSERT */

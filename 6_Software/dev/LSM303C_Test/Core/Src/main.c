/* USER CODE BEGIN Header */
/**
  ******************************************************************************
  * @file           : main.c
  * @brief          : Main program body
  ******************************************************************************
  * @attention
  *
  * Copyright (c) 2024 STMicroelectronics.
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
#include "i2c.h"
#include "usart.h"
#include "gpio.h"

/* Private includes ----------------------------------------------------------*/
/* USER CODE BEGIN Includes */

#include <stdio.h>
#include <string.h>
#include <stdlib.h>

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

extern I2C_HandleTypeDef hi2c1;
extern UART_HandleTypeDef huart2;

/* USER CODE END PV */

/* Private function prototypes -----------------------------------------------*/
void SystemClock_Config(void);
/* USER CODE BEGIN PFP */

/* USER CODE END PFP */

/* Private user code ---------------------------------------------------------*/
/* USER CODE BEGIN 0 */

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
  MX_USART2_UART_Init();
  MX_I2C1_Init();
  /* USER CODE BEGIN 2 */

  uint8_t CTRL_REGx_A[8] = {0x20, 0x3F, 0x00, 0x00, 0x34, 0x00, 0x00, 0x00}; //init array

//  uint8_t STATUS_REG_A_reg = 0x27;
//  uint8_t STATUS_REG_A_val = 0x00;

  uint8_t OUT_X_reg = 0x28;
  int16_t OUT_X_val = 0x00;
  double accX = 0;

  uint8_t OUT_Y_reg = 0x2A;
  int16_t OUT_Y_val = 0x00;
  double accY = 0;

  uint8_t OUT_Z_reg = 0x2C;
  int16_t OUT_Z_val = 0x00;
  double accZ = 0;

  int16_t accActive = 0;
  int16_t accActiveOld = 0;

  int16_t accDelta = 0;

  uint8_t str[50] = "";

  HAL_I2C_Master_Transmit(&hi2c1, 0x3A, CTRL_REGx_A, sizeof(CTRL_REGx_A), HAL_MAX_DELAY);   //init

  /* USER CODE END 2 */

  /* Infinite loop */
  /* USER CODE BEGIN WHILE */
  while (1)
  {
      HAL_I2C_Master_Transmit(&hi2c1, 0x3A, &OUT_X_reg, 1, HAL_MAX_DELAY);
      HAL_I2C_Master_Receive(&hi2c1, 0x3A, (uint8_t *)&OUT_X_val, 2, HAL_MAX_DELAY);
      accX = OUT_X_val * 0.244; //[accX]mg = LSB * 0.244mg/LSB

      HAL_I2C_Master_Transmit(&hi2c1, 0x3A, &OUT_Y_reg, 1, HAL_MAX_DELAY);
      HAL_I2C_Master_Receive(&hi2c1, 0x3A, (uint8_t *)&OUT_Y_val, 2, HAL_MAX_DELAY);
      accY = OUT_Y_val * 0.244; //[accY]mg = LSB * 0.244mg/LSB

      HAL_I2C_Master_Transmit(&hi2c1, 0x3A, &OUT_Z_reg, 1, HAL_MAX_DELAY);
      HAL_I2C_Master_Receive(&hi2c1, 0x3A, (uint8_t *)&OUT_Z_val, 2, HAL_MAX_DELAY);
      accZ = OUT_Z_val * 0.244; //[accZ]mg = LSB * 0.244mg/LSB

      accActive = (int16_t)(accX+accY+accZ);

      accDelta = abs(accActive - accActiveOld);

      accActiveOld = accActive;

      sprintf((char *)str, "accX:%+07.1fmg, accY:%+07.1fmg, accZ:%+07.1fmg\r\n", accX, accY, accZ);
//      sprintf((char *)str, "%f\t%f\t%f\t%d\r\n", accX, accY, accZ, accActive);
//      sprintf((char *)str, "%d\t\r\n", accDelta);
      HAL_UART_Transmit(&huart2, str, strlen((char *)str), HAL_MAX_DELAY);

      HAL_Delay(20);

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
  if (HAL_PWREx_ControlVoltageScaling(PWR_REGULATOR_VOLTAGE_SCALE1) != HAL_OK)
  {
    Error_Handler();
  }

  /** Initializes the RCC Oscillators according to the specified parameters
  * in the RCC_OscInitTypeDef structure.
  */
  RCC_OscInitStruct.OscillatorType = RCC_OSCILLATORTYPE_HSI;
  RCC_OscInitStruct.HSIState = RCC_HSI_ON;
  RCC_OscInitStruct.HSICalibrationValue = RCC_HSICALIBRATION_DEFAULT;
  RCC_OscInitStruct.PLL.PLLState = RCC_PLL_ON;
  RCC_OscInitStruct.PLL.PLLSource = RCC_PLLSOURCE_HSI;
  RCC_OscInitStruct.PLL.PLLM = 1;
  RCC_OscInitStruct.PLL.PLLN = 10;
  RCC_OscInitStruct.PLL.PLLP = RCC_PLLP_DIV7;
  RCC_OscInitStruct.PLL.PLLQ = RCC_PLLQ_DIV2;
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

#ifdef  USE_FULL_ASSERT
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

/* USER CODE BEGIN Header */
/**
  ******************************************************************************
  * @file           : main.c
  * @brief          : Main program body
  ******************************************************************************
  * @attention
  *
  * <h2><center>&copy; Copyright (c) 2022 STMicroelectronics.
  * All rights reserved.</center></h2>
  *
  * This software component is licensed by ST under BSD 3-Clause license,
  * the "License"; You may not use this file except in compliance with the
  * License. You may obtain a copy of the License at:
  *                        opensource.org/licenses/BSD-3-Clause
  *
  ******************************************************************************
  */
/* USER CODE END Header */
/* Includes ------------------------------------------------------------------*/
#include "main.h"
#include "adc.h"
#include "dac.h"
#include "dma.h"
#include "i2c.h"
#include "tim.h"
#include "usart.h"
#include "gpio.h"

/* Private includes ----------------------------------------------------------*/
/* USER CODE BEGIN Includes */
#include "string.h"
#include "Global_IT.h"
#include "oled.h"
#include "stm32_dsp.h"
#include "USART_IT.h"
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
uint16_t DAC_values[100] = { 2010,2136,2261,2386,2509,2631,2749,2865,2978,3087,3191,3291,3385,3475,3558,3636,3707,3771,3828,3878,3921,3956,3984,4004,4016,4019,4016,4004,3984,3956,3921,3878,3828,3771,3707,3636,3558,3475,3385,3291,3191,3087,2978,2865,2749,2631,2509,2386,2261,2136,2010,1883,1758,1633,1510,1388,1270,1154,1041,932,828,728,634,544,461,383,312,248,191,141,98,63,35,15,3,0,3,15,35,63,98,141,191,248,312,383,461,544,634,728,828,932,1041,1154,1270,1388,1510,1633,1758,1883 };
uint32_t ADC_Values[1024];//储存ADC数据

uint8_t aRxBuffer[2];
uint8_t uartWavStr[10000];
uint8_t* rxPtr;
uint8_t rxFlg;
uint8_t rxStr[200];
uint8_t sendFlag;
extern uint8_t stopADC;
//short save_statue = 0;

/* USER CODE END PV */

/* Private function prototypes -----------------------------------------------*/
void SystemClock_Config(void);
/* USER CODE BEGIN PFP */
/*void HAL_ADC_ConvCpltCallback(ADC_HandleTypeDef* hadc)//ADC转换完成回调函数
{
  static uint32_t i = 0;//函数内部计数
  if (i < 1500)
  {
    ADC_values[i] = HAL_ADC_GetValue(&hadc1);
    if (i == 499)
    {
      save_statue = 1;//计数为1，告诉主函数发送前半段
      i++;
    }
    else if (i == 999)
    {
      save_statue = 2;//计数为2，告诉主函数发送后半段
      i = 0;
    }

    else i++;
  }
}*/
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
  MX_DMA_Init();
  MX_DAC_Init();
  MX_ADC1_Init();
  MX_TIM3_Init();
  MX_USART1_UART_Init();
  MX_TIM2_Init();
  MX_I2C1_Init();
  MX_USART2_UART_Init();
  /* USER CODE BEGIN 2 */
  OLED_Init();

  OLED_Clear();
  rxPtr = rxStr;
  //printf("HelloWorld!\xff\xff\xff");
  sprintf(uartWavStr, "addt 1,0,325\xff\xff\xff");
  for (int i = 0;i < 330;i++)
    strcat(uartWavStr, "\xfe");
  strcat(uartWavStr, "\xff\xff\xff");
  OLED_ShowString(0, 0, "PrePare2ShowF&Vpp", 16);
  //HAL_ADC_Start_IT(&hadc1);

  HAL_UART_Receive_IT(&huart1, aRxBuffer, 1);

  HAL_TIM_Base_Start(&htim2);//DAC计时
  HAL_DAC_Start_DMA(&hdac, DAC1_CHANNEL_1, (uint32_t*)DAC_values, 100, DAC_ALIGN_12B_R);
  HAL_TIM_Base_Start(&htim3);//ADC开始计时
  HAL_ADC_Start_DMA(&hadc1, ADC_Values, 1024);


  unsigned long int num = 0;
  /* USER CODE END 2 */

  /* Infinite loop */
  /* USER CODE BEGIN WHILE */
  while (1)
  {
    /* USER CODE END WHILE */

    /* USER CODE BEGIN 3 */
//		if(stopADC){
//    HAL_UART_Receive_IT(&huart1, aRxBuffer, 1);
//		}
    /*if (save_statue == 1)
    {
      for (short i = 0;i < 500;i++)
      {
        sprintf(text, "%ld,%hu\r\n", num * 1, ADC_values[i]);
        HAL_UART_Transmit(&huart1, text, strlen(text), 100);
        num++;
      }
      if (save_statue == 1) save_statue = 0;
    }
    else if (save_statue == 2)
    {
      for (short i = 500;i < 1000;i++)
      {
        sprintf(text, "%ld,%hu\r\n", num * 1, ADC_values[i]);
        HAL_UART_Transmit(&huart1, text, strlen(text), 100);
        num++;
      }
      if (save_statue == 2) save_statue = 0;
    }

  }*/
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
  RCC_PeriphCLKInitTypeDef PeriphClkInit = {0};

  /** Initializes the RCC Oscillators according to the specified parameters
  * in the RCC_OscInitTypeDef structure.
  */
  RCC_OscInitStruct.OscillatorType = RCC_OSCILLATORTYPE_HSE;
  RCC_OscInitStruct.HSEState = RCC_HSE_ON;
  RCC_OscInitStruct.HSEPredivValue = RCC_HSE_PREDIV_DIV1;
  RCC_OscInitStruct.HSIState = RCC_HSI_ON;
  RCC_OscInitStruct.PLL.PLLState = RCC_PLL_ON;
  RCC_OscInitStruct.PLL.PLLSource = RCC_PLLSOURCE_HSE;
  RCC_OscInitStruct.PLL.PLLMUL = RCC_PLL_MUL9;
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
  RCC_ClkInitStruct.APB1CLKDivider = RCC_HCLK_DIV2;
  RCC_ClkInitStruct.APB2CLKDivider = RCC_HCLK_DIV1;

  if (HAL_RCC_ClockConfig(&RCC_ClkInitStruct, FLASH_LATENCY_2) != HAL_OK)
  {
    Error_Handler();
  }
  PeriphClkInit.PeriphClockSelection = RCC_PERIPHCLK_ADC;
  PeriphClkInit.AdcClockSelection = RCC_ADCPCLK2_DIV6;
  if (HAL_RCCEx_PeriphCLKConfig(&PeriphClkInit) != HAL_OK)
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
     tex: printf("Wrong parameters value: file %s on line %d\r\n", file, line) */
  /* USER CODE END 6 */
}
#endif /* USE_FULL_ASSERT */

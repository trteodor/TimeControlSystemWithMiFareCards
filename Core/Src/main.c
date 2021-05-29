/* USER CODE BEGIN Header */
/**
  ******************************************************************************
  * @file           : main.c
  * @brief          : Main program body
  ******************************************************************************
  * @attention
  *
  * <h2><center>&copy; Copyright (c) 2021 STMicroelectronics.
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
#include "dma.h"
#include "i2c.h"
#include "lwip.h"
#include "spi.h"
#include "tim.h"
#include "usart.h"
#include "gpio.h"

/* Private includes ----------------------------------------------------------*/
/* USER CODE BEGIN Includes */
#include <stdio.h>
#include <string.h>
#include "rtc.h"
#include "sntp_client.h"

#include "http_app.h"
#include "lwip/apps/httpd.h"

/* USER CODE END Includes */

/* Private typedef -----------------------------------------------------------*/
/* USER CODE BEGIN PTD */
/* USER CODE END PTD */

/* Private define ------------------------------------------------------------*/
/* USER CODE BEGIN PD */
/* USER CODE END PD */

/* Private macro -------------------------------------------------------------*/
/* USER CODE BEGIN PM */
void HAL_TIM_PeriodElapsedCallback(TIM_HandleTypeDef *htim)
{
	if(htim->Instance==TIM2)
	{
		  MX_LWIP_Process ();   //I don't use it its only for LwIP stack driver!
	}
}
/* USER CODE END PM */

/* Private variables ---------------------------------------------------------*/

/* USER CODE BEGIN PV */

#define COUNT  5
static const char * const serverTbl[COUNT] = {
  "wikipedia.org",
  "nie.ma.takiego.serwera",
  "vega.cbk.poznan.pl",
  "tempus1.gum.gov.pl",
  "tempus2.gum.gov.pl"
};
static int status[COUNT] = {
  SNTP_NOT_RUNNING,
  SNTP_NOT_RUNNING,
  SNTP_NOT_RUNNING,
  SNTP_NOT_RUNNING,
  SNTP_NOT_RUNNING
};
/* USER CODE END PV */

/* Private function prototypes -----------------------------------------------*/
void SystemClock_Config(void);
static void MPU_Config(void);
/* USER CODE BEGIN PFP */

/* USER CODE END PFP */

/* Private user code ---------------------------------------------------------*/
/* USER CODE BEGIN 0 */
void BackupDateToBR(void);


RTC_TimeTypeDef RtcTime;
RTC_DateTypeDef RtcDate;
uint16_t Milliseconds;

uint8_t CompareMilliseconds;
uint8_t CompareDate;

uint8_t Message[120];
uint8_t MessageLen;




void BackupDateToBR(void)
{
	HAL_RTCEx_BKUPWrite(&hrtc, RTC_BKP_DR2, ((RtcDate.Date << 8) | (RtcDate.Month)));
	HAL_RTCEx_BKUPWrite(&hrtc, RTC_BKP_DR3, ((RtcDate.Year << 8) | (RtcDate.WeekDay)));
}

void SetRTC(RTC_TimeTypeDef* sTime, 	RTC_DateTypeDef* DateToUpdate)
{


	if (HAL_RTC_SetTime(&hrtc, sTime, RTC_FORMAT_BIN) != HAL_OK)
	{
	  Error_Handler();
	}

	if (HAL_RTC_SetDate(&hrtc, DateToUpdate, RTC_FORMAT_BIN) != HAL_OK)
	{
	  Error_Handler();
	}

	BackupDateToBR();
}

void ClearStandbyFlags()
{

	  if (__HAL_PWR_GET_FLAG(PWR_FLAG_SB) != RESET)  //jesli wstal z stadby
	  {
		  __HAL_PWR_CLEAR_FLAG(PWR_FLAG_SB);
		  HAL_RTCEx_DeactivateWakeUpTimer(&hrtc);
			 __HAL_RTC_WAKEUPTIMER_CLEAR_FLAG(&hrtc, RTC_FLAG_WUTF);
	  }
	  	  else
	  	  	  {
		 MX_RTC_Init();
			}
}

void EnterStandbyByMe(uint16_t time)
{
	float t_helper=0;

	t_helper=time/0.0005;

	  if (HAL_RTCEx_SetWakeUpTimer_IT(&hrtc,(uint16_t)t_helper, RTC_WAKEUPCLOCK_RTCCLK_DIV16) != HAL_OK)
	  {
	    Error_Handler();
	  }
	  HAL_PWR_EnterSTANDBYMode();
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

  /* MPU Configuration--------------------------------------------------------*/
  MPU_Config();

  /* Enable I-Cache---------------------------------------------------------*/
  SCB_EnableICache();

  /* Enable D-Cache---------------------------------------------------------*/
  SCB_EnableDCache();

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
  MX_LWIP_Init();
  MX_USART3_UART_Init();
  MX_I2C1_Init();
  MX_SPI1_Init();
  MX_TIM2_Init();
  MX_SPI3_Init();
  /* USER CODE BEGIN 2 */
  MX_RTC_Init();  // -< I modified this function so much...

  SCB_CleanInvalidateDCache();

 // ClearStandbyFlags();

  HAL_RTC_GetTime(&hrtc, &RtcTime, RTC_FORMAT_BIN);
  HAL_RTC_GetDate(&hrtc, &RtcDate, RTC_FORMAT_BIN);


  SNTPclientStart(serverTbl[3], &status[3]);

  httpd_init();

  // initializing CGI  [= CGI #7 =]
  myCGIinit();

  // initializing SSI [* SSI #6 *]
  mySSIinit();

  HAL_TIM_Base_Start_IT(&htim2);

  uint32_t YellowLedTime=HAL_GetTick();
 uint32_t TimeActualization=HAL_GetTick();
 uint32_t zSNTPtimertime=HAL_GetTick();
  /* USER CODE END 2 */

  /* Infinite loop */
  /* USER CODE BEGIN WHILE */
  while (1)
  {

	  if(HAL_GetTick() - YellowLedTime > 1000) //HeardBit
	  {
		  YellowLedTime=HAL_GetTick();
		  HAL_GPIO_TogglePin(LED_YELLOW_GPIO_Port, LED_YELLOW_Pin);
		  HAL_UART_Transmit(&huart3, (uint8_t*) "HeartBit", 8, 100);
	  }

	  if( HAL_GetTick() -zSNTPtimertime > 200)
	  {
		  zSNTPtimertime=HAL_GetTick();
		  SNTPtimer();
	  }
	  if(HAL_GetTick() - TimeActualization > 60000)
	  {
		  TimeActualization=HAL_GetTick();
		  SNTPclientStart(serverTbl[3], &status[3]);
	  }
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

  /** Supply configuration update enable
  */
  HAL_PWREx_ConfigSupply(PWR_LDO_SUPPLY);
  /** Configure the main internal regulator output voltage
  */
  __HAL_PWR_VOLTAGESCALING_CONFIG(PWR_REGULATOR_VOLTAGE_SCALE0);

  while(!__HAL_PWR_GET_FLAG(PWR_FLAG_VOSRDY)) {}
  /** Initializes the RCC Oscillators according to the specified parameters
  * in the RCC_OscInitTypeDef structure.
  */
  RCC_OscInitStruct.OscillatorType = RCC_OSCILLATORTYPE_HSI|RCC_OSCILLATORTYPE_LSI;
  RCC_OscInitStruct.HSIState = RCC_HSI_DIV1;
  RCC_OscInitStruct.HSICalibrationValue = RCC_HSICALIBRATION_DEFAULT;
  RCC_OscInitStruct.LSIState = RCC_LSI_ON;
  RCC_OscInitStruct.PLL.PLLState = RCC_PLL_ON;
  RCC_OscInitStruct.PLL.PLLSource = RCC_PLLSOURCE_HSI;
  RCC_OscInitStruct.PLL.PLLM = 8;
  RCC_OscInitStruct.PLL.PLLN = 65;
  RCC_OscInitStruct.PLL.PLLP = 1;
  RCC_OscInitStruct.PLL.PLLQ = 4;
  RCC_OscInitStruct.PLL.PLLR = 2;
  RCC_OscInitStruct.PLL.PLLRGE = RCC_PLL1VCIRANGE_3;
  RCC_OscInitStruct.PLL.PLLVCOSEL = RCC_PLL1VCOWIDE;
  RCC_OscInitStruct.PLL.PLLFRACN = 0;
  if (HAL_RCC_OscConfig(&RCC_OscInitStruct) != HAL_OK)
  {
    Error_Handler();
  }
  /** Initializes the CPU, AHB and APB buses clocks
  */
  RCC_ClkInitStruct.ClockType = RCC_CLOCKTYPE_HCLK|RCC_CLOCKTYPE_SYSCLK
                              |RCC_CLOCKTYPE_PCLK1|RCC_CLOCKTYPE_PCLK2
                              |RCC_CLOCKTYPE_D3PCLK1|RCC_CLOCKTYPE_D1PCLK1;
  RCC_ClkInitStruct.SYSCLKSource = RCC_SYSCLKSOURCE_PLLCLK;
  RCC_ClkInitStruct.SYSCLKDivider = RCC_SYSCLK_DIV1;
  RCC_ClkInitStruct.AHBCLKDivider = RCC_HCLK_DIV2;
  RCC_ClkInitStruct.APB3CLKDivider = RCC_APB3_DIV2;
  RCC_ClkInitStruct.APB1CLKDivider = RCC_APB1_DIV2;
  RCC_ClkInitStruct.APB2CLKDivider = RCC_APB2_DIV2;
  RCC_ClkInitStruct.APB4CLKDivider = RCC_APB4_DIV2;

  if (HAL_RCC_ClockConfig(&RCC_ClkInitStruct, FLASH_LATENCY_3) != HAL_OK)
  {
    Error_Handler();
  }
}

/* USER CODE BEGIN 4 */

/* USER CODE END 4 */

/* MPU Configuration */

void MPU_Config(void)
{
  MPU_Region_InitTypeDef MPU_InitStruct = {0};

  /* Disables the MPU */
  HAL_MPU_Disable();
  /** Initializes and configures the Region and the memory to be protected
  */
  MPU_InitStruct.Enable = MPU_REGION_ENABLE;
  MPU_InitStruct.Number = MPU_REGION_NUMBER0;
  MPU_InitStruct.BaseAddress = 0x30000000;
  MPU_InitStruct.Size = MPU_REGION_SIZE_256B;
  MPU_InitStruct.SubRegionDisable = 0x0;
  MPU_InitStruct.TypeExtField = MPU_TEX_LEVEL0;
  MPU_InitStruct.AccessPermission = MPU_REGION_FULL_ACCESS;
  MPU_InitStruct.DisableExec = MPU_INSTRUCTION_ACCESS_ENABLE;
  MPU_InitStruct.IsShareable = MPU_ACCESS_NOT_SHAREABLE;
  MPU_InitStruct.IsCacheable = MPU_ACCESS_NOT_CACHEABLE;
  MPU_InitStruct.IsBufferable = MPU_ACCESS_BUFFERABLE;

  HAL_MPU_ConfigRegion(&MPU_InitStruct);
  /** Initializes and configures the Region and the memory to be protected
  */
  MPU_InitStruct.Number = MPU_REGION_NUMBER1;
  MPU_InitStruct.BaseAddress = 0x30004000;
  MPU_InitStruct.Size = MPU_REGION_SIZE_16KB;
  MPU_InitStruct.TypeExtField = MPU_TEX_LEVEL1;
  MPU_InitStruct.IsShareable = MPU_ACCESS_SHAREABLE;
  MPU_InitStruct.IsBufferable = MPU_ACCESS_NOT_BUFFERABLE;

  HAL_MPU_ConfigRegion(&MPU_InitStruct);
  /* Enables the MPU */
  HAL_MPU_Enable(MPU_PRIVILEGED_DEFAULT);

}

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

/************************ (C) COPYRIGHT STMicroelectronics *****END OF FILE****/

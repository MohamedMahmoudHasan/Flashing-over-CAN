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
  * This software is li*********************************************************
  */
/* USER CODE END Header */
/* Includes ------------------------------------------------------------------*/
#include "main.h"
#include "Bootloader.h"
#include "stm32f1xx_hal_uart.h"
volatile int  StdIdVal,x;
uint32_t rcvData[2];
volatile uint32_t Seq_counter = 0;
volatile uint8_t debg_state = 0;

CAN_HandleTypeDef hcan;
UART_HandleTypeDef huart1;

CAN_TxHeaderTypeDef TxHeader1;
//uint8_t  RxData1[8];
//CAN_RxHeaderTypeDef RxHeader1;




uint8_t TxData[8];


uint32_t TxMailbox[4];


void SystemClock_Config(void);
static void MX_GPIO_Init(void);
static void MX_CAN_Init(void);
static void MX_USART1_UART_Init(void);

/**
  * @brief  The application entry point.
  * @retval int
  */
int main(void)
{
  
  HAL_Init();

  
  SystemClock_Config();

  
  MX_GPIO_Init();
  MX_CAN_Init();
	MX_USART1_UART_Init();
  /* USER CODE BEGIN 2 */
	HAL_CAN_Start(&hcan);
	HAL_CAN_ActivateNotification(&hcan, CAN_IT_RX_FIFO0_MSG_PENDING);  
	
  TxHeader1.DLC = 0;
	TxHeader1.ExtId = 0;
	TxHeader1.IDE = CAN_ID_STD;
	TxHeader1.RTR = CAN_RTR_DATA;
	TxHeader1.StdId = 12;  // ID
		
	
	//bootloader();
	//HAL_CAN_AddTxMessage(&hcan,&TxHeader1,TxData,TxMailbox);
  while (1)
  {
		
	}
}

/**
  * @brief System Clock Configuration
  * @retval None
  */
void SystemClock_Config(void)
{
  RCC_OscInitTypeDef RCC_OscInitStruct = {0};
  RCC_ClkInitTypeDef RCC_ClkInitStruct = {0};

  
  
  RCC_OscInitStruct.OscillatorType = RCC_OSCILLATORTYPE_HSI;
  RCC_OscInitStruct.HSIState = RCC_HSI_ON;
  RCC_OscInitStruct.HSICalibrationValue = RCC_HSICALIBRATION_DEFAULT;
  RCC_OscInitStruct.PLL.PLLState = RCC_PLL_NONE;
  if (HAL_RCC_OscConfig(&RCC_OscInitStruct) != HAL_OK)
  {
    Error_Handler();
  }

  
  RCC_ClkInitStruct.ClockType = RCC_CLOCKTYPE_HCLK|RCC_CLOCKTYPE_SYSCLK
                              |RCC_CLOCKTYPE_PCLK1|RCC_CLOCKTYPE_PCLK2;
  RCC_ClkInitStruct.SYSCLKSource = RCC_SYSCLKSOURCE_HSI;
  RCC_ClkInitStruct.AHBCLKDivider = RCC_SYSCLK_DIV1;
  RCC_ClkInitStruct.APB1CLKDivider = RCC_HCLK_DIV1;
  RCC_ClkInitStruct.APB2CLKDivider = RCC_HCLK_DIV1;

  if (HAL_RCC_ClockConfig(&RCC_ClkInitStruct, FLASH_LATENCY_0) != HAL_OK)
  {
    Error_Handler();
  }
}

/**
  * @brief CAN Initialization Function
  * @param None
  * @retval None
  */
static void MX_CAN_Init(void)
{

  /* USER CODE BEGIN CAN_Init 0 */

  /* USER CODE END CAN_Init 0 */

  /* USER CODE BEGIN CAN_Init 1 */

  /* USER CODE END CAN_Init 1 */
  hcan.Instance = CAN1;
  hcan.Init.Prescaler = 1;
  hcan.Init.Mode = CAN_MODE_NORMAL;
  hcan.Init.SyncJumpWidth = CAN_SJW_1TQ;
  hcan.Init.TimeSeg1 = CAN_BS1_12TQ;
  hcan.Init.TimeSeg2 = CAN_BS2_5TQ;
  hcan.Init.TimeTriggeredMode = DISABLE;
  hcan.Init.AutoBusOff = DISABLE;
  hcan.Init.AutoWakeUp = DISABLE;
  hcan.Init.AutoRetransmission = DISABLE;
  hcan.Init.ReceiveFifoLocked = DISABLE;
  hcan.Init.TransmitFifoPriority = DISABLE;
  if (HAL_CAN_Init(&hcan) != HAL_OK)
  {
    Error_Handler();
  }
  /* USER CODE BEGIN CAN_Init 2 */
		
CAN_FilterTypeDef canfilterconfig1;
    canfilterconfig1.FilterActivation = CAN_FILTER_ENABLE;
    canfilterconfig1.FilterBank = 10;  // which filter bank to use from the assigned ones
    canfilterconfig1.FilterFIFOAssignment = CAN_FILTER_FIFO0;
    canfilterconfig1.FilterIdHigh = 0x007<<5;
    canfilterconfig1.FilterIdLow = 0;
    canfilterconfig1.FilterMaskIdHigh = 0x007<<13;
    canfilterconfig1.FilterMaskIdLow = 0x0000;
    canfilterconfig1.FilterMode = CAN_FILTERMODE_IDMASK;
    canfilterconfig1.FilterScale = CAN_FILTERSCALE_32BIT;
    canfilterconfig1.SlaveStartFilterBank = 13;  // doesn't matter in single can controllers
	
	CAN_FilterTypeDef canfilterconfig2;
    canfilterconfig2.FilterActivation = CAN_FILTER_ENABLE;
    canfilterconfig2.FilterBank = 10;  // which filter bank to use from the assigned ones
    canfilterconfig2.FilterFIFOAssignment = CAN_FILTER_FIFO0;
    canfilterconfig2.FilterIdHigh = 0x008<<5;
    canfilterconfig2.FilterIdLow = 0;
    canfilterconfig2.FilterMaskIdHigh = 0x008<<13;
    canfilterconfig2.FilterMaskIdLow = 0x0000;
    canfilterconfig2.FilterMode = CAN_FILTERMODE_IDMASK;
    canfilterconfig2.FilterScale = CAN_FILTERSCALE_32BIT;
    canfilterconfig2.SlaveStartFilterBank = 13;  // doesn't matter in single can controllers

CAN_FilterTypeDef canfilterconfig3;
    canfilterconfig3.FilterActivation = CAN_FILTER_ENABLE;
    canfilterconfig3.FilterBank = 10;  // which filter bank to use from the assigned ones
    canfilterconfig3.FilterFIFOAssignment = CAN_FILTER_FIFO0;
    canfilterconfig3.FilterIdHigh = 0x009<<5;
    canfilterconfig3.FilterIdLow = 0;
    canfilterconfig3.FilterMaskIdHigh = 0x009<<13;
    canfilterconfig3.FilterMaskIdLow = 0x0000;
    canfilterconfig3.FilterMode = CAN_FILTERMODE_IDMASK;
    canfilterconfig3.FilterScale = CAN_FILTERSCALE_32BIT;
    canfilterconfig3.SlaveStartFilterBank = 13;  // doesn't matter in single can controllers

CAN_FilterTypeDef canfilterconfig4;
    canfilterconfig4.FilterActivation = CAN_FILTER_ENABLE;
    canfilterconfig4.FilterBank = 10;  // which filter bank to use from the assigned ones
    canfilterconfig4.FilterFIFOAssignment = CAN_FILTER_FIFO0;
    canfilterconfig4.FilterIdHigh = 10<<5;
    canfilterconfig4.FilterIdLow = 0;
    canfilterconfig4.FilterMaskIdHigh = 10<<13;
    canfilterconfig4.FilterMaskIdLow = 0x0000;
    canfilterconfig4.FilterMode = CAN_FILTERMODE_IDMASK;
    canfilterconfig4.FilterScale = CAN_FILTERSCALE_32BIT;
    canfilterconfig4.SlaveStartFilterBank = 13;  // doesn't matter in single can controllers

CAN_FilterTypeDef canfilterconfig5;
    canfilterconfig5.FilterActivation = CAN_FILTER_ENABLE;
    canfilterconfig5.FilterBank = 10;  // which filter bank to use from the assigned ones
    canfilterconfig5.FilterFIFOAssignment = CAN_FILTER_FIFO0;
    canfilterconfig5.FilterIdHigh = 11<<5;
    canfilterconfig5.FilterIdLow = 0;
    canfilterconfig5.FilterMaskIdHigh = 11<<13;
    canfilterconfig5.FilterMaskIdLow = 0x0000;
    canfilterconfig5.FilterMode = CAN_FILTERMODE_IDMASK;
    canfilterconfig5.FilterScale = CAN_FILTERSCALE_32BIT;
    canfilterconfig5.SlaveStartFilterBank = 13;  // doesn't matter in single can controllers

    HAL_CAN_ConfigFilter(&hcan, &canfilterconfig1);
		HAL_CAN_ConfigFilter(&hcan, &canfilterconfig2);
		HAL_CAN_ConfigFilter(&hcan, &canfilterconfig3);
		HAL_CAN_ConfigFilter(&hcan, &canfilterconfig4);
		HAL_CAN_ConfigFilter(&hcan, &canfilterconfig5);
  /* USER CODE END CAN_Init 2 */

}

/**
  * @brief GPIO Initialization Function
  * @param None
  * @retval None
  */
static void MX_GPIO_Init(void)
{
/* USER CODE BEGIN MX_GPIO_Init_1 */
/* USER CODE END MX_GPIO_Init_1 */

  /* GPIO Ports Clock Enable */
  __HAL_RCC_GPIOA_CLK_ENABLE();

/* USER CODE BEGIN MX_GPIO_Init_2 */
/* USER CODE END MX_GPIO_Init_2 */
}

/* USER CODE BEGIN 4 */

/**
  * @brief USART1 Initialization Function
  * @param None
  * @retval None
  */
static void MX_USART1_UART_Init(void)
{

  /* USER CODE BEGIN USART1_Init 0 */

  /* USER CODE END USART1_Init 0 */

  /* USER CODE BEGIN USART1_Init 1 */

  /* USER CODE END USART1_Init 1 */
  huart1.Instance = USART1;
  huart1.Init.BaudRate = 9600;
  huart1.Init.WordLength = UART_WORDLENGTH_8B;
  huart1.Init.StopBits = UART_STOPBITS_1;
  huart1.Init.Parity = UART_PARITY_NONE;
  huart1.Init.Mode = UART_MODE_TX_RX;
  huart1.Init.HwFlowCtl = UART_HWCONTROL_NONE;
  huart1.Init.OverSampling = UART_OVERSAMPLING_16;
  if (HAL_UART_Init(&huart1) != HAL_OK)
  {
    Error_Handler();
  }
  /* USER CODE BEGIN USART1_Init 2 */

  /* USER CODE END USART1_Init 2 */

}

//void HAL_CAN_RxFifo0MsgPendingCallback(CAN_HandleTypeDef *hcan)
//{
//	x++;
//	StdIdVal=(CAN1->sFIFOMailBox[0].RIR >> 21);
//	HAL_CAN_GetRxMessage(hcan, CAN_RX_FIFO0, &RxHeader1, RxData1);
//	HAL_CAN_AddTxMessage(hcan, &TxHeader1, TxData, &TxMailbox[0]);
//}

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

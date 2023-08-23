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
#include "program.h"



/* Private variables ---------------------------------------------------------*/
CAN_HandleTypeDef hcan;

/* USER CODE BEGIN PV */

/* USER CODE END PV */
CAN_TxHeaderTypeDef TxHeader;
CAN_TxHeaderTypeDef TxHeader1;
CAN_TxHeaderTypeDef TxHeader2;
CAN_TxHeaderTypeDef TxHeader3;
CAN_TxHeaderTypeDef TxHeader4;

CAN_RxHeaderTypeDef RxHeader;

uint8_t seq =0;
uint8_t StdIdVal=0;
uint32_t TxMailbox[4];

uint16_t DataSoFar = 0;
uint16_t pageLimit = 0;
uint8_t PageBurned = 0;
uint8_t TxData[8];
uint8_t RxData[8];
uint8_t AuthData[4]={0xDE,0xCF,0x37,0xAB};
uint32_t SizeData = (sizeof(array)/sizeof(char));

int i=0;

uint8_t ImgData[16]={0x10,0x20,0x30,0x40,0x50,0x60,0x70,0x80,0x11,0x21,0x31,0x41,0x51,0x61,0x71,0x81};

/* Private function prototypes -----------------------------------------------*/
void SystemClock_Config(void);
static void MX_GPIO_Init(void);
static void MX_CAN_Init(void);


void HAL_CAN_RxFifo0MsgPendingCallback(CAN_HandleTypeDef *hcan)
{
	StdIdVal=(CAN1->sFIFOMailBox[0].RIR >> 21);
	HAL_CAN_GetRxMessage(hcan, CAN_RX_FIFO0, &RxHeader, RxData);
	if(StdIdVal == 12){
				seq++;
		    DataSoFar = 0;
	}else if(StdIdVal == 13){
				seq = 0;
	}else if(StdIdVal == 14){
				PageBurned = 0;
		    pageLimit = 0;
	}
	
}


int main(void)
{
  
  /* Reset of all peripherals, Initializes the Flash interface and the Systick. */
  HAL_Init();

  /* Configure the system clock */
  SystemClock_Config();

  /* Initialize all configured peripherals */
  MX_GPIO_Init();
  MX_CAN_Init();


	HAL_CAN_Start(&hcan);

  HAL_CAN_ActivateNotification(&hcan, CAN_IT_RX_FIFO0_MSG_PENDING);
	
	/*Defining the TxMessage*/
	//prog session
	TxHeader.DLC = 0;
  TxHeader.ExtId = 0;
  TxHeader.IDE = CAN_ID_STD;
  TxHeader.RTR = CAN_RTR_DATA;
  TxHeader.StdId = 0x7;
  TxHeader.TransmitGlobalTime = DISABLE;
	//Auth
	TxHeader1.DLC = 4;
  TxHeader1.ExtId = 0;
  TxHeader1.IDE = CAN_ID_STD;
  TxHeader1.RTR = CAN_RTR_DATA;
  TxHeader1.StdId = 0x8;
  TxHeader1.TransmitGlobalTime = DISABLE;
	//Size
	TxHeader2.DLC = 4;
  TxHeader2.ExtId = 0;
  TxHeader2.IDE = CAN_ID_STD;
  TxHeader2.RTR = CAN_RTR_DATA;
  TxHeader2.StdId = 0x9;
  TxHeader2.TransmitGlobalTime = DISABLE;
	//Data
	TxHeader3.DLC = 8;
  TxHeader3.ExtId = 0;
  TxHeader3.IDE = CAN_ID_STD;
  TxHeader3.RTR = CAN_RTR_DATA;
  TxHeader3.StdId = 10;
  TxHeader3.TransmitGlobalTime = DISABLE;
	//Exit
	TxHeader4.DLC = 0;
  TxHeader4.ExtId = 0;
  TxHeader4.IDE = CAN_ID_STD;
  TxHeader4.RTR = CAN_RTR_DATA;
  TxHeader4.StdId = 0xB;
  TxHeader4.TransmitGlobalTime = DISABLE;
	
	/*The data to be sent*/
	TxData[0] = 0;
	TxData[1] = 50;
	
	//HAL_CAN_AddTxMessage(&hcan, &TxHeader, &TxData[0], &TxMailbox[0]);
	
	
  /* Infinite loop */
  /* USER CODE BEGIN WHILE */
  while (1)
  {
		//start prog session
		//DataSoFar=0;
		pageLimit=0;
		switch(seq){
			case 0 :
				HAL_CAN_AddTxMessage(&hcan, &TxHeader, &TxData[0], &TxMailbox[0]);
				HAL_Delay(2000);
			break;
			
			case 1 :
				HAL_CAN_AddTxMessage(&hcan, &TxHeader1, AuthData, &TxMailbox[0]);
				HAL_Delay(2000);

			break;
			
			case 2 :
				HAL_CAN_AddTxMessage(&hcan, &TxHeader2,(uint8_t*) &SizeData, &TxMailbox[0]);
				HAL_Delay(2000);
			break;
			
			case 3 :
				while(DataSoFar < SizeData){
					if(pageLimit < 1024 && PageBurned == 0){
						HAL_CAN_AddTxMessage(&hcan, &TxHeader3, array+(i*8), &TxMailbox[0]);
					  DataSoFar += 8;
						pageLimit += 8;
						i++;
					}else{
						pageLimit = 0;
						PageBurned = 1;
					}
					HAL_Delay(50);
				}
				
			break;
				
			case 4 :
				HAL_CAN_AddTxMessage(&hcan, &TxHeader4, &TxData[0], &TxMailbox[0]);
				HAL_Delay(2000);
				
			break;	
			
			default:
				seq=0;
			break;
		}			
		
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

  /** Initializes the RCC Oscillators according to the specified parameters
  * in the RCC_OscInitTypeDef structure.
  */
  RCC_OscInitStruct.OscillatorType = RCC_OSCILLATORTYPE_HSI;
  RCC_OscInitStruct.HSIState = RCC_HSI_ON;
  RCC_OscInitStruct.HSICalibrationValue = RCC_HSICALIBRATION_DEFAULT;
  RCC_OscInitStruct.PLL.PLLState = RCC_PLL_NONE;
  if (HAL_RCC_OscConfig(&RCC_OscInitStruct) != HAL_OK)
  {
    Error_Handler();
  }
  /** Initializes the CPU, AHB and APB buses clocks
  */
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
  hcan.Init.AutoWakeUp = ENABLE;
  hcan.Init.AutoRetransmission = ENABLE;
  hcan.Init.ReceiveFifoLocked = DISABLE;
  hcan.Init.TransmitFifoPriority = DISABLE;
  if (HAL_CAN_Init(&hcan) != HAL_OK)
  {
    Error_Handler();
  }
	CAN1->MCR &= ~(1<<0); 
  /* USER CODE BEGIN CAN_Init 2 */
	CAN_FilterTypeDef canfilterconfig1;
	CAN_FilterTypeDef canfilterconfig2;
	CAN_FilterTypeDef canfilterconfig3;

  canfilterconfig1.FilterActivation = CAN_FILTER_ENABLE;
  canfilterconfig1.FilterBank = 10;  // anything between 0 to SlaveStartFilterBank
  canfilterconfig1.FilterFIFOAssignment = CAN_RX_FIFO0;
  canfilterconfig1.FilterIdHigh = 12<<5;
  canfilterconfig1.FilterIdLow = 0x0000;
  canfilterconfig1.FilterMaskIdHigh = 12<<13;
  canfilterconfig1.FilterMaskIdLow = 0x0000;
  canfilterconfig1.FilterMode = CAN_FILTERMODE_IDMASK;
  canfilterconfig1.FilterScale = CAN_FILTERSCALE_32BIT;
  canfilterconfig1.SlaveStartFilterBank = 13;  // 13 to 27 are assigned to slave CAN (CAN 2) OR 0 to 12 are assgned to CAN1

	canfilterconfig2.FilterActivation = CAN_FILTER_ENABLE;
  canfilterconfig2.FilterBank = 10;  // anything between 0 to SlaveStartFilterBank
  canfilterconfig2.FilterFIFOAssignment = CAN_RX_FIFO0;
  canfilterconfig2.FilterIdHigh = 13<<5;
  canfilterconfig2.FilterIdLow = 0x0000;
  canfilterconfig2.FilterMaskIdHigh = 13<<13;
  canfilterconfig2.FilterMaskIdLow = 0x0000;
  canfilterconfig2.FilterMode = CAN_FILTERMODE_IDMASK;
  canfilterconfig2.FilterScale = CAN_FILTERSCALE_32BIT;
  canfilterconfig2.SlaveStartFilterBank = 13;  // 13 to 27 are assigned to slave CAN (CAN 2) OR 0 to 12 are assgned to CAN1
	
	canfilterconfig3.FilterActivation = CAN_FILTER_ENABLE;
  canfilterconfig3.FilterBank = 10;  // anything between 0 to SlaveStartFilterBank
  canfilterconfig3.FilterFIFOAssignment = CAN_RX_FIFO0;
  canfilterconfig3.FilterIdHigh = 14<<5;
  canfilterconfig3.FilterIdLow = 0x0000;
  canfilterconfig3.FilterMaskIdHigh = 14<<13;
  canfilterconfig3.FilterMaskIdLow = 0x0000;
  canfilterconfig3.FilterMode = CAN_FILTERMODE_IDMASK;
  canfilterconfig3.FilterScale = CAN_FILTERSCALE_32BIT;
  canfilterconfig3.SlaveStartFilterBank = 13;  // 13 to 27 are assigned to slave CAN (CAN 2) OR 0 to 12 are assgned to CAN1
	
  HAL_CAN_ConfigFilter(&hcan, &canfilterconfig1);
	HAL_CAN_ConfigFilter(&hcan, &canfilterconfig2);
	HAL_CAN_ConfigFilter(&hcan, &canfilterconfig3);
	
  /* USER CODE END CAN_Init 2 */

}

/**
  * @brief GPIO Initialization Function
  * @param None
  * @retval None
  */
static void MX_GPIO_Init(void)
{

  /* GPIO Ports Clock Enable */
  __HAL_RCC_GPIOA_CLK_ENABLE();

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


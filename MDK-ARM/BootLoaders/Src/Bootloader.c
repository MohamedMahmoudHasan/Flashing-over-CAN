/*
 * Bootloader.c
 *
 *  Created on: Jul 13, 2023
 *      Author: ahmedshindy
 */

#include "Bootloader.h"
#include "Boot_Mngr.h"
#include "Program.h"
#include  "Flash.h"
#include "stm32f1xx_hal.h"
#include "stm32f1xx_hal_flash.h"





/************************** Variables ***********************************/
volatile uint32_t Request = 0;
volatile uint8_t DataArrived  = 0;
uint32_t App_Valid = APP_VALID_EXISTS;
uint32_t App_Request = NO_APP_REQUEST;
uint32_t Image_Size;
uint32_t PageAddress = APPLICATION_1_ADDRESS;
volatile uint8_t  RxData[8];
CAN_RxHeaderTypeDef RxHeader;
CAN_TxHeaderTypeDef TxHeader_NegRes;
volatile uint32_t Request_Sequence = 0;

// This will hold the Page 1024 bytes of data.
uint8_t Rx_Page[PAGE_SIZE];
uint32_t data_index ;

uint8_t BootMode_Msg[] = "Boot loader is Running \r\n";
uint8_t Invalid_SequenceMsg[] = "Invalid Sequence \r\n";
uint8_t BootModeError_Msg[] = "Boot loader running always [ ERROR ] \r\n";

uint8_t PrgMsg_Ack[] = "Programming Session Done!, Send Data \r\n";

uint8_t Msg_Ack[] =  " Done, Continue \r\n";
uint8_t NegResp_Msg[] = "Sending negative response \r\n";
uint8_t Invalid_RequestMsg[] = "Invalid Request \r\n";


/************************** External Variables **************************/
extern UART_HandleTypeDef huart1;
extern CAN_HandleTypeDef hcan;
extern CAN_TxHeaderTypeDef TxHeader1;


extern uint8_t TxData[8];
extern uint32_t TxMailbox[4];
/************************** Functions Prototypes ************************/


/************************** Static Functions Prototypes *****************/


/************************** Bootloader Main function ********************/
int bootloader(void)
{
	
	uint32_t Received_Key = 0; // read the key.


	/* This Function decides weather to continue to the boot loader or
	* jump to the application. */
	Boot_Mngr();

	// Configure pc13 as gpio output
	GPIO_InitTypeDef GpioC13_Init= {GPIO_PIN_13,GPIO_MODE_OUTPUT_PP,GPIO_NOPULL,GPIO_SPEED_FREQ_LOW};
	HAL_GPIO_Init(GPIOC, &GpioC13_Init);

	// Send Debug Msg Over UART
	HAL_UART_Transmit (&huart1, BootMode_Msg,  sizeof(BootMode_Msg)/sizeof(char),1000);
  uint32_t dummy = 0;
	while(1)
	{
		// Poll until you receive request over CAN
    while(DataArrived == 0)
		{
			dummy++;
		}
		// Switch on the request type and process it
		switch(Request)
		{
			case PROGRAMMING_REQUEST:
				/* Enter Programming Sequence. */
				if(Request_Sequence == 0 )
				{
					// Keep following the sequence
					Request_Sequence++;
					// Send Debug Msg Over UART
					HAL_UART_Transmit (&huart1, PrgMsg_Ack,sizeof(PrgMsg_Ack)/sizeof(char),1000);
					// Send positive Response
					HAL_CAN_AddTxMessage(&hcan,&TxHeader1,TxData,TxMailbox);
				}
				else
				{
					Request_Sequence = 0;
					// Send Invalid Sequence
					HAL_UART_Transmit (&huart1, Invalid_SequenceMsg,sizeof(Invalid_SequenceMsg)/sizeof(char),1000);
					// Send Negative Response
					SendNegative_Response(PROGRAMMING_REQUEST);

				}
				break;

			case AUTHENTICATION_REQUEST:
				/* Authentication request, Read 32 bit key */
				for( uint8_t counter =0 ; counter < 4; counter ++ )
				{
					Received_Key |= (RxData[counter] << counter*8 );
				}
				if( (Request_Sequence == 1) && (Received_Key == RESERVED_KEY ) )
				{
					// Clear the key
					Received_Key = 0x00;
					// Keep following the sequence
					Request_Sequence++;
					// Send Debug Msg Over UART
					HAL_UART_Transmit (&huart1, PrgMsg_Ack,sizeof(Msg_Ack)/sizeof(char),1000);
					// Send positive Response
					HAL_CAN_AddTxMessage(&hcan,&TxHeader1,TxData,TxMailbox);
				}
				else
				{
					Request_Sequence = 0;
					// Send Invalid Sequence
					HAL_UART_Transmit (&huart1, Invalid_SequenceMsg,sizeof(Invalid_SequenceMsg)/sizeof(char),1000);
					// Send Negative Response
					SendNegative_Response(AUTHENTICATION_REQUEST);
				}
				break;

			case DATA_SIZE_REQUEST:
				// Validate sequence
				if( Request_Sequence == 2 )
				{
							/* Receive Data size and record it */
							for( uint8_t counter =0 ; counter < 4; counter ++ )
							{
									Image_Size |= (RxData[counter] << counter*8 );
							}
					// Keep following the sequence
					Request_Sequence++;
					// Send Debug Msg Over UART
					HAL_UART_Transmit (&huart1, Msg_Ack,sizeof(Msg_Ack)/sizeof(char),1000);
					// Send positive Response
					SendPositive_Response(DATA_SIZE_REQUEST);
				}
				else
				{
					// Send Invalid Sequence
					HAL_UART_Transmit (&huart1, Invalid_SequenceMsg,sizeof(Invalid_SequenceMsg)/sizeof(char),1000);
					// Send Negative Response
					SendNegative_Response(DATA_SIZE_REQUEST);
				}
				break;
			case DATA_TRANSFER_REQUEST:

				/* Receive the data continuously over CAN
				 * Write to flash page by page */
				if( Request_Sequence == 3 )
				{
							/* Receive actual Data and record it */
							for( uint8_t counter =0 ; counter < 4; counter ++ )
							{
									Rx_Page[data_index] = RxData[counter];
									data_index ++;
							}
					// Decrement Image size		
					Image_Size -= 8;
						
					// Keep following the sequence
					if(Image_Size == 0)		{   Request_Sequence++;  }
					
					if( data_index == 1024 )
					{
						// Write page to flash
						Flash_Write_Data (PageAddress, (uint32_t *)Rx_Page,  PAGE_SIZE/4);
						
						// Increment page address for next iteration
						PageAddress += PAGE_SIZE;	
						
						// Clear data index
						data_index =0;
					}
					// Send Debug Msg Over UART
					HAL_UART_Transmit (&huart1, Msg_Ack,sizeof(Msg_Ack)/sizeof(char),1000);
					// Send positive Response
					SendPositive_Response(DATA_TRANSFER_REQUEST);
				}
				else
				{
					// Send Invalid Sequence
					HAL_UART_Transmit (&huart1, Invalid_SequenceMsg,
							sizeof(Invalid_SequenceMsg)/sizeof(char),1000);
					// Send Negative Response
					SendNegative_Response(DATA_TRANSFER_REQUEST);
				}
				break;

			case TRANSFER_EXIT:
				if( Request_Sequence == 4 )
				{
					// Mark application as valid
					Flash_Write_Data (APP_VALIDATION_ADDRESS, &App_Valid, 1);
					// Clear the App_Request Byte
					Flash_Write_Data (APP_VALIDATION_ADDRESS + 4, &App_Request, 1);
					// Keep following the sequence
					Request_Sequence++;
					// Send Debug Msg Over UART
					HAL_UART_Transmit (&huart1, Msg_Ack,sizeof(Msg_Ack)/sizeof(char),1000);
					// Send positive Response
					SendPositive_Response(TRANSFER_EXIT);

					// Jump to the application
					Goto_Application(APPLICATION_1_ADDRESS);
				}
				else
				{
					// Send Invalid Sequence
					HAL_UART_Transmit (&huart1, Invalid_SequenceMsg,sizeof(Invalid_SequenceMsg)/sizeof(char),1000);
					// Send Negative Response
					SendNegative_Response(TRANSFER_EXIT);
				}
				break;
			default:
				HAL_UART_Transmit (&huart1, Invalid_RequestMsg,sizeof(Invalid_RequestMsg)/sizeof(char),1000);
				break;
		}
		DataArrived = 0;
	}


	// This Line should never reached.
	while(1)
	{
		HAL_UART_Transmit (&huart1, BootModeError_Msg,  sizeof(BootModeError_Msg)/sizeof(char),  1000);
		HAL_Delay(2000);
	}
	return 0;
}



/*@brief : This function should send positive response over CAN.
 */
void SendPositive_Response(uint32_t Response_Code)
{
	Response_Code = 0;
//	switch(Response_Code)
//	{
//	case PROGRAMMING_REQUEST :
//		// Send SID + 0x40
//    HAL_CAN_AddTxMessage(&hcan,&TxHeader1,TxData,TxMailbox);
//		break;
//	case AUTHENTICATION_REQUEST:
//		// Send SID + 0x40
//	  HAL_CAN_AddTxMessage(&hcan,&TxHeader1,TxData,TxMailbox);
//		break;
//	case SENDING_KEY_REQUEST :
//		// Send SID + 0x40
//		break;
//	case DATA_SIZE_REQUEST:
//		// Send SID + 0x40
//		break;
//	case DATA_TRANSFER_REQUEST :
//		// Send SID + 0x40
//		break;
//	case TRANSFER_EXIT:
//		// Send SID + 0x40
//		break;
//	case CHECK_CRC:
//		// Send SID + 0x40
//		break;
//	default :
//			  

//		break;
//	}
HAL_CAN_AddTxMessage(&hcan,&TxHeader1,TxData,TxMailbox);
}

/*@brief : This function should send negative response over CAN.
 */
void SendNegative_Response(uint32_t Response_Code)
{
	// To Jump to default session only
	Response_Code = 0;
	
	TxHeader_NegRes.DLC = 0;
	TxHeader_NegRes.ExtId = 0;
	TxHeader_NegRes.IDE = CAN_ID_STD;
	TxHeader_NegRes.RTR = CAN_RTR_DATA;
	TxHeader_NegRes.StdId = 13;  // ID
	// Send 0x7f + SID.
	switch(Response_Code)
	{
	case PROGRAMMING_REQUEST :
		// Send 0x7f + SID.
		break;
	case AUTHENTICATION_REQUEST:
		// Send 0x7f + SID.
		break;
	case SENDING_KEY_REQUEST :
		// Send 0x7f + SID.
		break;
	case DATA_SIZE_REQUEST:
		// Send 0x7f + SID.
		break;
	case DATA_TRANSFER_REQUEST :
		break;
	case TRANSFER_EXIT:
		break;
	case CHECK_CRC:
		break;
	default :
		// Send 0x7f with no other data
		HAL_CAN_AddTxMessage(&hcan,&TxHeader_NegRes,TxData,TxMailbox);

		break;
	}
}



void Goto_Application(const uint32_t address)
{
	const sJmp_t* vector_p = (sJmp_t*)address;

	__asm("msr msp, %0; bx %1;" : : "r"(vector_p->stack_addr), "r"(vector_p->func_p));
}






void HAL_CAN_RxFifo0MsgPendingCallback(CAN_HandleTypeDef *hcan)
{
	DataArrived = 1;
	Request=(CAN1->sFIFOMailBox[0].RIR >> 21);
	HAL_CAN_GetRxMessage(hcan, CAN_RX_FIFO0, &RxHeader, RxData);
}

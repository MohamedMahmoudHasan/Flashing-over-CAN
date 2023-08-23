/*
 * Bootloader.h
 *
 *  Created on: Jun 23, 2023
 *      Author: ahmedshindy
 */

#ifndef INC_BOOTLOADER_H_
#define INC_BOOTLOADER_H_


#include "stdint.h"
#define NO_VALID_APP_EXISTS 			0x00UL
#define VALID_APP_EXISTS 				0x01UL
#define APPLICATION_1_ADDRESS			0x8005000UL
#define PAGE_SIZE						(1024UL) 		/* 1024 byte, 256 word*/
#define RESERVED_KEY					0xAB37CFDEUL


#define PROGRAMMING_REQUEST				(0x07)
#define AUTHENTICATION_REQUEST			(0x08)
#define SENDING_KEY_REQUEST				(0x09)
#define DATA_SIZE_REQUEST				(0X0B)
#define DATA_TRANSFER_REQUEST			(0X0C)
#define TRANSFER_EXIT					(0X0D)
#define CHECK_CRC						(0X0E)



typedef void (app_t)(void);

typedef struct
{
    uint32_t		stack_addr;     // Stack Pointer
    app_t*				func_p;     // Program Counter
}sJmp_t;



/************************************ Functions Prototypes ******************/

/* @brief : This is the bootloader main function */
int bootloader(void);

/* @brief : This function should Jump to the application */
void Goto_Application(const uint32_t address);

/* @brief : This function should send positive response over CAN. */
void SendPositive_Response(uint32_t Response_Code);

/* @brief : This function should send negative response over CAN. */
void SendNegative_Response(uint32_t Response_Code);


#endif /* INC_BOOTLOADER_H_ */

/*
 * Boot_Mngr.h
 *
 *  Created on: Jul 16, 2023
 *      Author: Ahmed Shendy
 */

#ifndef INC_BOOT_MNGR_H_
#define INC_BOOT_MNGR_H_

#define    APP_VALIDATION_ADDRESS  (*(volatile uint32_t*)0x0800FC00)
#define    APP_REQ_ADDRESS     (*(volatile uint32_t*)0x0800FC00)

#define 	NO_APP_REQUEST		0X55
#define		APP_VALID_EXISTS	0x66

/* @name 	: Boot_Mngr
 * @brief	: This Function Should decide if I continue in Boot mode or I jump to the application.
 * 			  It Also should handle the programming request coming from the application.
 */
void Boot_Mngr(void);



#endif /* INC_BOOT_MNGR_H_ */

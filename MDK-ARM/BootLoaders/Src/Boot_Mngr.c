/*
 * Boot_Mngr.c
 *
 *  Created on: Jul 16, 2023
 *      Author: Ahmed Shendy
 */



#include "Boot_Mngr.h"
#include "Bootloader.h"
#include "stdint.h"



/* @name 	: Boot_Mngr
 * @brief	: This Function Should decide if I continue in Boot mode or I jump to the application.
 * 			  It Also should handle the programming request coming from the application.
 */
void Boot_Mngr(void)
{

	// Read the first 2 words of the page 63 storing them into App_Valid & Bl_AppRequest variables respectively
	uint32_t Page63_Word0 = APP_VALIDATION_ADDRESS;  // Read page 63, word 0
	uint32_t Page63_Word1 = APP_REQ_ADDRESS;		 // Read page 63, word 1

	// If there is a valid application and Bl_AppRequest == NO_APP_REQUEST
	if(  ( Page63_Word0 == APP_VALID_EXISTS )   &&    (Page63_Word1 == NO_APP_REQUEST) )
	{
		// Jump to the application
		Goto_Application(APPLICATION_1_ADDRESS);

	}
	else
	{
		// Continue to the Bootloader, aka do nothing and Return
	}

	return ;
}


/**********************************************************************************************************************
 *  FILE DESCRIPTION
 *  -------------------------------------------------------------------------------------------------------------------
 *         File:  Flash.h
 *       Module:  Flash
 *
 *  Description:  header file for Flash Module    
 *********************************************************************************************************************/
 
#ifndef FLASH_H
#define FLASH_H

/******************************************************************************
* \Syntax          : uint32_t Flash_Write_Data (uint32_t StartPageAddress, uint32_t *Data, uint16_t numberofwords);                                 
* \Description     : This erase a specified page and rewrite it with a new data                                   
*                                                                             
* \Sync\Async      : Sync                                             
* \Reentrancy      : Nonreentrant      
* \Parameters (in) : uint32_t StartPageAddress -> the page start address
				     uint32_t *Data -> the data to be written
					 uint16_t numberofwords-> the number of words to be written
* \Parameters (out):  non
* \Return value:   :  uint32_t -> the error happened during writing to flash
*******************************************************************************/
uint32_t Flash_Write_Data (uint32_t StartPageAddress, uint32_t *Data, uint16_t numberofwords);

#endif


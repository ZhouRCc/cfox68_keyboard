/* USER CODE BEGIN Header */
/**
 ******************************************************************************
 * File Name        :  FlashDrv.h
 * Description      :  falsh driver base on stm32f446
 ******************************************************************************
 * @attention
 *
* COPYRIGHT:    Copyright (c) 2024  tangminfei2013@126.com
* DATE:         JUL 05th, 2024
 ******************************************************************************
 */
/* USER CODE END Header */
 
/* Includes ------------------------------------------------------------------*/
#pragma once
 
/*****************************************************************************/
/* Includes                                                                  */
/*****************************************************************************/
#include "main.h"
 
#ifdef __cplusplus
extern "C" {
#endif 
    
#define ADDR_FLASH_SECTOR_0     ((uint32_t)0x08000000) /* Base address of Sector 0, 16 Kbytes */
#define ADDR_FLASH_SECTOR_1     ((uint32_t)0x08004000) /* Base address of Sector 1, 16 Kbytes */
#define ADDR_FLASH_SECTOR_2     ((uint32_t)0x08008000) /* Base address of Sector 2, 16 Kbytes */
#define ADDR_FLASH_SECTOR_3     ((uint32_t)0x0800C000) /* Base address of Sector 3, 16 Kbytes */
#define ADDR_FLASH_SECTOR_4     ((uint32_t)0x08010000) /* Base address of Sector 4, 64 Kbytes */
#define ADDR_FLASH_SECTOR_5     ((uint32_t)0x08020000) /* Base address of Sector 5, 128 Kbytes */
#define ADDR_FLASH_SECTOR_6     ((uint32_t)0x08040000) /* Base address of Sector 6, 128 Kbytes */
#define ADDR_FLASH_SECTOR_7     ((uint32_t)0x08060000) /* Base address of Sector 7, 128 Kbytes */
 
HAL_StatusTypeDef FlashDrv_Write(uint32_t Addr, uint8_t *source, uint32_t length);
uint32_t FlashDrv_read(uint32_t Addr, uint8_t *source, uint32_t length);
uint32_t Find_Last_NonEmpty_Address(uint32_t startAddr, uint32_t endAddr);
uint32_t Read_Flash_Uint32(uint32_t address);
 
#ifdef __cplusplus
}
#endif   
 

 
 
/* USER CODE BEGIN Header */
/**
 ******************************************************************************
 * File Name        :  FlashDrv.c
 * Description      :  falsh driver base on stm32f446
 ******************************************************************************
 * @attention
 *
* COPYRIGHT:    Copyright (c) 2024  tangminfei2013@126.com
* DATE:         JUL 05th, 2024
 ******************************************************************************
 */
/* USER CODE END Header */
#include "FlashDrv.h"
#include "stdint.h"
 
 /* Start @ of user Flash area */
#define FLASH_USER_START_ADDR   ADDR_FLASH_SECTOR_2  
 
/* End @ of user Flash area : sector start address + sector size -1 */
#define FLASH_USER_END_ADDR     ADDR_FLASH_SECTOR_7  +  GetSectorSize(ADDR_FLASH_SECTOR_7) -1 
 
 
#define  FLASH_TIMEOUT_VALUE     1000
 
static uint32_t GetSectorSize(uint32_t Sector)
{
  uint32_t sectorsize = 0x00;
 
  if((Sector == FLASH_SECTOR_0) || (Sector == FLASH_SECTOR_1) || (Sector == FLASH_SECTOR_2) || (Sector == FLASH_SECTOR_3))
  {
    sectorsize = 16 * 1024;
  }
  else if(Sector == FLASH_SECTOR_4)
  {
    sectorsize = 64 * 1024;
  }
  else
  {
    sectorsize = 128 * 1024;
  }  
  return sectorsize;
}
 
 
static uint32_t GetSector(uint32_t Address)
{
  uint32_t sector = 0;
 
  if((Address < ADDR_FLASH_SECTOR_1) && (Address >= ADDR_FLASH_SECTOR_0))
  {
    sector = FLASH_SECTOR_0;
  }
  else if((Address < ADDR_FLASH_SECTOR_2) && (Address >= ADDR_FLASH_SECTOR_1))
  {
    sector = FLASH_SECTOR_1;
  }
  else if((Address < ADDR_FLASH_SECTOR_3) && (Address >= ADDR_FLASH_SECTOR_2))
  {
    sector = FLASH_SECTOR_2;
  }
  else if((Address < ADDR_FLASH_SECTOR_4) && (Address >= ADDR_FLASH_SECTOR_3))
  {
    sector = FLASH_SECTOR_3;
  }
  else if((Address < ADDR_FLASH_SECTOR_5) && (Address >= ADDR_FLASH_SECTOR_4))
  {
    sector = FLASH_SECTOR_4;
  }
  else if((Address < ADDR_FLASH_SECTOR_6) && (Address >= ADDR_FLASH_SECTOR_5))
  {
    sector = FLASH_SECTOR_5;
  }
  else if((Address < ADDR_FLASH_SECTOR_7) && (Address >= ADDR_FLASH_SECTOR_6))
  {
    sector = FLASH_SECTOR_6;
  }
  else /* (Address < FLASH_END_ADDR) && (Address >= ADDR_FLASH_SECTOR_7) */
  {
    sector = FLASH_SECTOR_7;
  }
 
  return sector;
}
 
HAL_StatusTypeDef FlashDrv_Write(uint32_t Addr, uint8_t *source, uint32_t length)
{
    uint32_t FirstSector = 0, NbOfSectors = 0;
    HAL_StatusTypeDef FlashStatus = HAL_OK;
    uint32_t  SECTORError = 0;
    FLASH_EraseInitTypeDef EraseInitStruct;
    int trycnt = 0;
    int i;
    
    /* Unlock the Flash to enable the flash control register access *************/
    HAL_FLASH_Unlock();
 
    /* Get the bank */
    FirstSector = GetSector(Addr);
    NbOfSectors = GetSector(FLASH_USER_END_ADDR) - FirstSector + 1;
    
    /* Fill EraseInit structure*/
    EraseInitStruct.TypeErase     = FLASH_TYPEERASE_SECTORS;
    EraseInitStruct.VoltageRange  = FLASH_VOLTAGE_RANGE_3;
    EraseInitStruct.Sector        = FirstSector;
    EraseInitStruct.NbSectors     = NbOfSectors;
    
    do
    {
        FlashStatus = HAL_FLASHEx_Erase(&EraseInitStruct, &SECTORError);
        if( FlashStatus != HAL_OK)
        {
            /* Infinite loop */
            trycnt++;
            if( trycnt > 3 )
            {
                 HAL_FLASH_Lock();
                return FlashStatus;
            }
        }
        else
            break;
        FLASH_WaitForLastOperation((uint32_t)FLASH_TIMEOUT_VALUE);
    }while( trycnt < 3);
    
    __HAL_FLASH_DATA_CACHE_DISABLE();
    __HAL_FLASH_INSTRUCTION_CACHE_DISABLE();
 
    __HAL_FLASH_DATA_CACHE_RESET();
    __HAL_FLASH_INSTRUCTION_CACHE_RESET();
 
    __HAL_FLASH_INSTRUCTION_CACHE_ENABLE();
    __HAL_FLASH_DATA_CACHE_ENABLE();
 
    FlashStatus = FLASH_WaitForLastOperation((uint32_t)FLASH_TIMEOUT_VALUE); // Wait for a FLASH operation to complete.
    
    if( FlashStatus == HAL_OK )
    {
        /* Program the user Flash area word by word
        (area defined by FLASH_USER_START_ADDR and FLASH_USER_END_ADDR) ***********/
        for ( i = 0; i < length; i += 4)
        {
           trycnt = 0;
           do{
                FlashStatus = HAL_FLASH_Program(FLASH_TYPEPROGRAM_WORD, Addr + i, *(uint32_t *)(source + i));
           }while( trycnt < 3 && FlashStatus != HAL_OK);
        }
    }
    /* Lock the Flash to disable the flash control register access (recommended
    to protect the FLASH memory against possible unwanted operation) *********/
    HAL_FLASH_Lock();
 
    return  HAL_OK;
}
 
 
uint32_t FlashDrv_read(uint32_t Addr, uint8_t *source, uint32_t length)
{
    int i;
    
    for ( i = 0; i < length; i++)
    {
        source[i] =  *(__IO uint8_t *)(Addr + i);
    }
 
    return  1;
}
 
 
/* End of this file */
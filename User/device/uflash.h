#pragma once

#ifdef __cplusplus
extern "C" {
#endif
#include "main.h"

#define BOARD_NUM_ADDR 0x0800C000

#define STM32_FLASH_BASE 0x08000000 //STM32 FLASH的起始地址
#define FLASH_WAITETIME 50000       //FLASH等待超时时间

//FLASH 扇区的起始地址
#define ADDR_FLASH_SECTOR_0     ((uint32_t)0x08000000) /* Base address of Sector 0, 16 Kbytes */
#define ADDR_FLASH_SECTOR_1     ((uint32_t)0x08004000) /* Base address of Sector 1, 16 Kbytes */
#define ADDR_FLASH_SECTOR_2     ((uint32_t)0x08008000) /* Base address of Sector 2, 16 Kbytes */
#define ADDR_FLASH_SECTOR_3     ((uint32_t)0x0800C000) /* Base address of Sector 3, 16 Kbytes */
#define ADDR_FLASH_SECTOR_4     ((uint32_t)0x08010000) /* Base address of Sector 4, 64 Kbytes */
#define ADDR_FLASH_SECTOR_5     ((uint32_t)0x08020000) /* Base address of Sector 5, 128 Kbytes */
#define ADDR_FLASH_SECTOR_6     ((uint32_t)0x08040000) /* Base address of Sector 6, 128 Kbytes */
#define ADDR_FLASH_SECTOR_7     ((uint32_t)0x08060000) /* Base address of Sector 7, 128 Kbytes */
#define ADDR_FLASH_SECTOR_END   ((uint32_t)0x0807ffff) /* End address of Flash sector */



HAL_StatusTypeDef STMFLASH_EraseWord(uint32_t SectorAddr); //擦除一个uint32所在扇区
uint32_t STMFLASH_ReadWord(uint32_t faddr);                             //读出字
HAL_StatusTypeDef STMFLASH_WriteWord(uint32_t addr, uint32_t data);     //写入一个字
HAL_StatusTypeDef STMFLASH_Write(uint32_t WriteAddr, uint32_t *pBuffer, uint32_t size);//从指定地址开始写入指定长度的数据
void STMFLASH_Read(uint32_t ReadAddr, uint32_t *pBuffer, uint32_t size);    //从指定地址开始读出指定长度的数据
HAL_StatusTypeDef STMFLASH_Erase(uint32_t WriteAddr, uint32_t size);    //擦除指定长度的数据

#ifdef __cplusplus
}
#endif


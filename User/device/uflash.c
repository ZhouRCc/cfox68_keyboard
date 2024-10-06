/**------------------------------------------
  * @brief  内部flash读写
  --------------------------------------------*/
#include "device/uflash.h"

/**
 * @brief  获取flash扇区
 * @param  addr:地址
 * @retval 扇区
 */
uint8_t STMFLASH_GetFlashSector(uint32_t addr)
{
    if (addr < ADDR_FLASH_SECTOR_1)
        return FLASH_SECTOR_0;
    else if (addr < ADDR_FLASH_SECTOR_2)
        return FLASH_SECTOR_1;
    else if (addr < ADDR_FLASH_SECTOR_3)
        return FLASH_SECTOR_2;
    else if (addr < ADDR_FLASH_SECTOR_4)
        return FLASH_SECTOR_3;
    else if (addr < ADDR_FLASH_SECTOR_5)
        return FLASH_SECTOR_4;
    else if (addr < ADDR_FLASH_SECTOR_6)
        return FLASH_SECTOR_5;
    else if (addr < ADDR_FLASH_SECTOR_7)
        return FLASH_SECTOR_6;
    else
        return FLASH_SECTOR_7;

}

/**
 * @brief  从flash读取一个字
 * @param  faddr:地址
 * @retval 读取的值
 */
uint32_t STMFLASH_ReadWord(uint32_t faddr)
{
    return *(__IO uint32_t *)faddr;
}


/**
 * @brief  向flash写入一个字节
 * @param  addr:地址
 * @param  data:要写入的数据
 * @retval 是否成功
*/
HAL_StatusTypeDef STMFLASH_WriteWord(uint32_t addr, uint32_t data)
{
    if (addr < STM32_FLASH_BASE || addr % 4)
        return HAL_ERROR; //非法地址
    HAL_StatusTypeDef FlashStatus = HAL_OK;
    
    STMFLASH_EraseWord(addr); //擦除整个扇区
    HAL_FLASH_Unlock();            //解锁
    FlashStatus = FLASH_WaitForLastOperation(FLASH_WAITETIME); //等待上次操作完成
    if (FlashStatus == HAL_OK)
    {
        FlashStatus = HAL_FLASH_Program(FLASH_TYPEPROGRAM_WORD, addr, data); //写入数据
        if (FlashStatus != HAL_OK) //写入数据
        {
            return FlashStatus; //写入异常
        }
    }

    HAL_FLASH_Lock(); //上锁
    if (STMFLASH_ReadWord(addr) != data)
        return HAL_ERROR; //写入异常
    else
        return HAL_OK; //写入成功
}

/**
 * @brief  flash擦除一个uint32数据所在扇区
 * @param  SectorAddr:扇区地址
 * @retval 发生错误的扇区，没问题返回hal_ok
*/
HAL_StatusTypeDef STMFLASH_EraseWord(uint32_t SectorAddr)
{
    if (SectorAddr < STM32_FLASH_BASE || SectorAddr % 4)
        return HAL_ERROR; //非法地址
    FLASH_EraseInitTypeDef FlashEraseInit;
    HAL_StatusTypeDef FlashStatus = HAL_OK;
    uint32_t SectorError = 0;
    
    HAL_FLASH_Unlock();            //解锁
    
    if (STMFLASH_ReadWord(SectorAddr) != 0XFFFFFFFF) //如果数据不是0XFFFFFFFF，则需要擦除
    {
        FlashEraseInit.TypeErase = FLASH_TYPEERASE_SECTORS;     //擦除类型，扇区擦除
        FlashEraseInit.Sector = STMFLASH_GetFlashSector(SectorAddr); //要擦除的扇区
        FlashEraseInit.NbSectors = 1;                           //一次只擦除一个扇区
        FlashEraseInit.VoltageRange = FLASH_VOLTAGE_RANGE_3;    //电压范围，VCC=2.7~3.6V之间!!
        
        if (HAL_FLASHEx_Erase(&FlashEraseInit, &SectorError) != HAL_OK)
        {
            return SectorError; //返回错误所在扇区
        }
    }
    
    FlashStatus = FLASH_WaitForLastOperation(FLASH_WAITETIME); //等待上次操作完成
    HAL_FLASH_Lock(); //上锁
    return FlashStatus;
}

/******************************************************************************************************************/

/**
 * @brief  向flash写入
 * @param  WriteAddr:起始地址
 * @param  pBuffer:数据指针
 * @param  size:要写入的字节数
 * @retval HAL_StatusTypeDef 是否成功
 */
HAL_StatusTypeDef STMFLASH_Write(uint32_t WriteAddr, uint32_t *pBuffer, uint32_t size)
{
    if (WriteAddr < STM32_FLASH_BASE || WriteAddr % 4)
        return HAL_ERROR; //非法地址
    HAL_StatusTypeDef FlashStatus = HAL_OK;
    uint32_t addrx = 0;
    uint32_t endaddr = 0;

    HAL_FLASH_Unlock();            //解锁
    addrx = WriteAddr;             //写入的起始地址
    endaddr = WriteAddr + size * 4; //写入的结束地址

    if(endaddr < ADDR_FLASH_SECTOR_END)
    {
        STMFLASH_Erase(WriteAddr, size); //擦除整个扇区
    }
    FlashStatus = FLASH_WaitForLastOperation(FLASH_WAITETIME); //等待上次操作完成
    if (FlashStatus == HAL_OK)
    {
        while (WriteAddr < endaddr) //写数据
        {
            if (HAL_FLASH_Program(FLASH_TYPEPROGRAM_WORD, WriteAddr, *pBuffer) != HAL_OK) //写入数据
            {
                break; //写入异常
            }
            WriteAddr += 4;
            pBuffer++;
        }
    }

    HAL_FLASH_Lock(); //上锁
    if (STMFLASH_ReadWord(addrx) != *pBuffer)
        return HAL_ERROR; //写入异常
    else
        return HAL_OK; //写入成功
}

/**
 * @brief  从flash读取数据
 * @param  ReadAddr:起始地址
 * @param  pBuffer:数据存放指针
 * @param  size:要读取的字节数
 * @retval 无
 */
void STMFLASH_Read(uint32_t ReadAddr, uint32_t *pBuffer, uint32_t size)
{
    uint32_t i;
    for (i = 0; i < size; i++)
    {
        pBuffer[i] = STMFLASH_ReadWord(ReadAddr); //读取4个字节.
        ReadAddr += 4;                            //偏移4个字节.
    }
}

/**
 * @brief  擦除flash扇区
*/
HAL_StatusTypeDef STMFLASH_Erase(uint32_t WriteAddr, uint32_t size)
{
    if (WriteAddr < STM32_FLASH_BASE || WriteAddr % 4)
        return HAL_ERROR; //非法地址
    FLASH_EraseInitTypeDef FlashEraseInit;
    HAL_StatusTypeDef FlashStatus = HAL_OK;
    uint32_t SectorError = 0;
    uint32_t addrx = 0;
    uint32_t endaddr = 0;

    HAL_FLASH_Unlock();            //解锁
    addrx = WriteAddr;             //写入的起始地址
    endaddr = WriteAddr + size * 4; //写入的结束地址
    while (addrx < endaddr)
        {
            if (STMFLASH_ReadWord(addrx) != 0XFFFFFFFF)
            {
                FlashEraseInit.TypeErase = FLASH_TYPEERASE_SECTORS;     //擦除类型，扇区擦除
                FlashEraseInit.Sector = STMFLASH_GetFlashSector(addrx); //要擦除的扇区
                FlashEraseInit.NbSectors = 1;                           //一次只擦除一个扇区
                FlashEraseInit.VoltageRange = FLASH_VOLTAGE_RANGE_3;    //电压范围，VCC=2.7~3.6V之间!!
                if (HAL_FLASHEx_Erase(&FlashEraseInit, &SectorError) != HAL_OK)
                {
                    break; //发生错误了
                }
            }
            else
                addrx += 4;
            FLASH_WaitForLastOperation(FLASH_WAITETIME); //等待上次操作完成
        }
    
    FlashStatus = FLASH_WaitForLastOperation(FLASH_WAITETIME); //等待上次操作完成
    HAL_FLASH_Lock(); //上锁
    return FlashStatus;
}
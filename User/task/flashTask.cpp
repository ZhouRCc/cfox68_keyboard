#include "task/userTask.h"
#include <cmsis_os2.h>
#include "device/FlashDrv.h"
#include "main.h"
#include "task/TopDefine.h"

#define FLASH_DEBUG
#ifdef FLASH_DEBUG
uint32_t flash_data[2];
#endif
msg_flash_t flash_msg;
uint8_t *msg_prio;

void FunctionFlash(void *argument)
{
    (void)argument; /* 未使用argument，消除警告 */
    const uint32_t delay_tick = osKernelGetTickFreq() / TASK_FREQ_FLASH;
    *msg_prio = 1;
    uint32_t tick = osKernelGetTickCount();
    while(1)
    {
//         if(osMessageQueueGet(robotStruct.msgq.q_flash_send, &flash_msg, msg_prio , 0) == osOK)
//         {
//             STMFLASH_Write(flash_msg.ptr, flash_msg.data, WS_FLASH_LENGTH);
// #ifdef FLASH_DEBUG
//             flash_data[0] = STMFLASH_ReadWord(flash_msg.ptr);
//             flash_data[1] = STMFLASH_ReadWord(flash_msg.ptr + 4);
// #endif
//         }
        tick += delay_tick;
        osDelayUntil(tick);
    }
}


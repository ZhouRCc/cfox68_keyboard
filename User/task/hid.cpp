#include "task/userTask.h"
#include <cmsis_os2.h>
#include "module/key_scan.hpp"

extern KeyScan keyboard;

void FunctionHid(void *argument)
{
    (void)argument; /* 未使用argument，消除警告 */
    const uint32_t delay_tick = osKernelGetTickFreq() / TASK_FREQ_HID;
    uint32_t tick = osKernelGetTickCount();
    while(1)
    {
        keyboard.send_report();
        tick += delay_tick;
        osDelayUntil(tick);
    }
}


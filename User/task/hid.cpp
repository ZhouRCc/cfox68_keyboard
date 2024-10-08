#include "task/userTask.h"
#include <cmsis_os2.h>
#include "module/key_scan.hpp"
#include "main.h"

extern KeyScan keyboard;
// GPIO_PinState pin;

void FunctionHid(void *argument)
{
    (void)argument; /* 未使用argument，消除警告 */
    const uint32_t delay_tick = osKernelGetTickFreq() / TASK_FREQ_HID;
    // HAL_GPIO_WritePin(COL13_GPIO_Port, COL13_Pin, GPIO_PIN_SET);
    uint32_t tick = osKernelGetTickCount();
    while(1)
    {
        // pin =     HAL_GPIO_ReadPin(ROW0_GPIO_Port, ROW0_Pin);
        keyboard.send_report();
        tick += delay_tick;
        osDelayUntil(tick);
    }
}


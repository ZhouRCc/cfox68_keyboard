/**
 * 案键扫描任务
*/
#include "task/TopDefine.h"
#include "task/userTask.h"
#include "module/key_scan.hpp"
#include "usb_device.h"

KeyScan keyboard;

/**
 * \brief 案键扫描任务
 *
 * \param argument 未使用
 */
void FunctionKey(void *argument) 
{
    (void)argument; /* 未使用argument，消除警告 */
    const uint32_t delay_tick = osKernelGetTickFreq() / TASK_FREQ_KEY;

    uint32_t tick = osKernelGetTickCount(); /* 控制任务运行频率的计时 */
    while(1)
    {
        keyboard();
        tick += delay_tick; /* 计算下一个唤醒时刻 */
        osDelayUntil(tick); /* 运行结束，等待下一次唤醒 */
    }
}

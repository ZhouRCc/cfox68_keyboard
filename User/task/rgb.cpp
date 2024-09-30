/**
 * rgb任务
*/
#include "task/TopDefine.h"
#include "task/userTask.h"
#include "device/ws2812b.h"

WS2812 rgb;
uint32_t getFreqSys(void)
{
    return TASK_FREQ_RGB;
}
/**
 * \brief rgb任务
 *
 * \param argument 未使用
 */
void FunctionRgb(void *argument) 
{
    (void)argument; /* 未使用argument，消除警告 */
    const uint32_t delay_tick = osKernelGetTickFreq() / TASK_FREQ_RGB;

    uint32_t RGB_COLOR_zhou = 0xff8300;
    uint32_t RGB_COLOR_di = 0x465618;
    rgb.setColor(RGB_COLOR_zhou, LED_Z, 0);
	rgb.setColor(RGB_COLOR_di, LED_D, 0);
    // rgb.setColor(RGB_COLOR_di);
    rgb.show();
    rgb.breathStep(getFreqSys, 100);


    uint32_t tick = osKernelGetTickCount(); /* 控制任务运行频率的计时 */
    while(1)
    {
        rgb.breathingLight(LED_Z);
        tick += delay_tick; /* 计算下一个唤醒时刻 */
        osDelayUntil(tick); /* 运行结束，等待下一次唤醒 */
    }
}

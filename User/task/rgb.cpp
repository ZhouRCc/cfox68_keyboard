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
    rgb.ws2812Init();
    uint32_t RGB_COLOR_zhou = 0x151200;
    uint32_t RGB_COLOR_di = 0x465618;
    rgb.setColor(RGB_COLOR_zhou, LED_Z);
	rgb.setColor(RGB_COLOR_di, LED_D);
    rgb.setMode(MODE_BREATH, LED_Z);
    rgb.show();
    rgb.breathStep(getFreqSys, 10);


    uint32_t tick = osKernelGetTickCount(); /* 控制任务运行频率的计时 */
    while(1)
    {
        rgb.wsLoop();
        tick += delay_tick; /* 计算下一个唤醒时刻 */
        osDelayUntil(tick); /* 运行结束，等待下一次唤醒 */
    }
}

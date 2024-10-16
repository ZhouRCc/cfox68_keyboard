/**
 * 初始化任务
*/
#include "task/TopDefine.h"
#include "task/userTask.h"
#include "usb_device.h"

//任务与队列结构体
robot_t robotStruct;

/*下面是所有的任务属性*/
const osThreadAttr_t  attr_rgb = {
    .name = "rgb",
    .priority = osPriorityRealtime,
    .stack_size = 256 * 8,
};

const osThreadAttr_t attr_key = {
    .name = "key",
    .priority = osPriorityRealtime,
    .stack_size = 256 * 8,
};

const osThreadAttr_t attr_flash = {
    .name = "flash",
    .priority = osPriorityRealtime,
    .stack_size = 128 * 4,
};

/*********************/
/**
 * \brief 初始化
 *
 * \param argument 未使用
 */
void initFunction(void *argument) 
{
    (void)argument; /* 未使用argument，消除警告 */
    MX_USB_DEVICE_Init();
    osKernelLock();
    /* 创建任务 */
    robotStruct.thread.rgb =
        osThreadNew(FunctionRgb, NULL, &attr_rgb);
    robotStruct.thread.key =
        osThreadNew(FunctionKey, NULL, &attr_key);
    robotStruct.thread.flash =
        osThreadNew(FunctionFlash, NULL, &attr_flash);

    robotStruct.msgq.q_flash_send =
        osMessageQueueNew(1, sizeof(msg_flash_t), NULL);
    robotStruct.msgq.q_key_press =
        osMessageQueueNew(1, sizeof(msg_key_t), NULL);
    
    osKernelUnlock();
    osThreadTerminate(osThreadGetId()); /* 结束自身 */
}

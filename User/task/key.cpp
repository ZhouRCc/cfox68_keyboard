/**
 * 案键扫描任务
*/
#include "task/TopDefine.h"
#include "task/userTask.h"
#include "module/key_scan.hpp"
#include "usb_device.h"
#include "usbd_hid.h"

extern USBD_HandleTypeDef hUsbDeviceFS;

/**
 * \brief 案键扫描任务
 *
 * \param argument 未使用
 */
void FunctionKey(void *argument) 
{
    (void)argument; /* 未使用argument，消除警告 */
    const uint32_t delay_tick = osKernelGetTickFreq() / TASK_FREQ_KEY;

    uint8_t buff[4] = {0, 10, 0 ,0};

    // uint8_t keyboard_report[8] = {0};  // 初始化8字节报告
    // keyboard_report[2] = 0x04;         // 'a' 的键码
    // USBD_HID_SendReport(&hUsbDeviceFS, keyboard_report, 8);  // 发送报告

    // USBD_HID_SendReport(&hUsbDeviceFS, keyboard_report, 8);  // 发送报告
    // memset(keyboard_report, 0, sizeof(keyboard_report));
    // USBD_HID_SendReport(&hUsbDeviceFS, keyboard_report, 8);

//	  USBD_HID_SendReport(&hUsbDeviceFS, buff, 4); // ��������

//	  HAL_Delay(1000); // ����buff�е�ֵ��ÿ������ϵĹ�꽫�����ƶ�һ��


    uint32_t tick = osKernelGetTickCount(); /* 控制任务运行频率的计时 */
    while(1)
    {
        USBD_HID_SendReport(&hUsbDeviceFS, buff, 4); // ��������

	    osDelay(1000); // ����buff�е�ֵ��ÿ������ϵĹ�꽫�����ƶ�һ��
        tick += delay_tick; /* 计算下一个唤醒时刻 */
        osDelayUntil(tick); /* 运行结束，等待下一次唤醒 */
    }
}

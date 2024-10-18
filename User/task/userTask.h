#pragma once


#ifdef __cplusplus
extern "C" {
#endif

#include <cmsis_os2.h>
#include "FreeRTOS.h"
#include "task/TopDefine.h"

/* 所有任务都要define一个“任务运行频率”和“初始化延时” */
#define TASK_FREQ_RGB (500u)
#define TASK_FREQ_KEY (1000u)
#define TASK_FREQ_FLASH (1u)


typedef struct
{
    /*线程*/
    struct
    {
        osThreadId_t rgb;
        osThreadId_t key;
        osThreadId_t flash;

    }thread;
    
    /*消息队列*/
    struct
    {
        osMessageQueueId_t q_flash_send; 
        osMessageQueueId_t q_key_press;
    }msgq;
    // /*事件组*/
    // struct
    // {
    //     // osEventFlagsId_t eventReceive;
    // }events;
    
#if _DEBUG == 1
    // uint32_t flash_data;
#endif
}robot_t;

/* Exported functions prototypes -------------------------------------------- */
void FunctionRgb(void *argument);
void FunctionKey(void *argument);
void FunctionFlash(void *argument);

extern robot_t robotStruct;

#ifdef __cplusplus
}
#endif


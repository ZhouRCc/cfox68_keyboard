#pragma once


#ifdef __cplusplus
extern "C" {
#endif

#include <cmsis_os2.h>
#include "FreeRTOS.h"
#include "task/TopDefine.h"

/* 所有任务都要define一个“任务运行频率”和“初始化延时” */
#define TASK_FREQ_RGB (500u)
#define TASK_FREQ_KEY (500u)


typedef struct
{
    /*线程*/
    struct
    {
        osThreadId_t rgb;
        osThreadId_t key;

    }thread;
    
    /*消息队列*/
    // struct
    // {
    //     // osMessageQueueId_t q_can_send; 
    // }msgq;
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


#ifdef __cplusplus
}
#endif


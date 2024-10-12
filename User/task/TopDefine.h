#pragma once


#ifdef TOP_USER_CFG
#include TOP_USER_CFG
#endif

#include <stdint.h>
//===================用户配置===================
//是否使用freertos
#ifndef _FREERTOS
#define _FREERTOS 1
#endif
//是否开启调试
#ifndef _DEBUG
#define _DEBUG 1
#endif

typedef enum
{
    LED_Z = 0,//轴灯
    LED_D = 1,//底灯
    LED_TYPE_NUM,
}led_type_e;//LED类型定义

typedef enum
{
    WS_FLASH_ON_LENGTH = 4,//led类型是否开启的flash存储数据长度
    WS_FLASH_MODE_LENGTH = 4,//led模式的flash存储数据长度
    WS_FLASH_COLOR_LENGTH = 24,//led颜色的flash存储数据长度
}ws_flash_def_e;//ws2812b的flash存储数据相关宏定义

#define WS_FLASH_LENGTH LED_TYPE_NUM * 4


typedef struct
{
    uint32_t ptr;
    uint32_t data[WS_FLASH_LENGTH];
}msg_flash_t;



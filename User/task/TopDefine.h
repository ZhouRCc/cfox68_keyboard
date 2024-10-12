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
    LED_TYPE_MOST_NUM = 4,
    LED_TYPE_MOST_MODE_NUM = 3
}led_type_e;//LED类型，如果要增加，最多4种，只有前三种能支持更改灯光模式

typedef enum
{
    WS_FLASH_ON_LENGTH = 4,//led类型是否开启的flash存储数据长度
    WS_FLASH_MODE_LENGTH = 4,//led模式的flash存储数据长度
    WS_FLASH_COLOR_LENGTH = 24,//led颜色的flash存储数据长度
    WS_FLASH_DUMMY_LENGTH = 8,//补全的flash存储数据长度
    WS_FLASH_LEAST_LENGTH = 64,//最少的flash存储数据长度
}ws_flash_def_e;//ws2812b的flash存储数据相关宏定义

#define WS_FLASH_LENGTH (WS_FLASH_LEAST_LENGTH + (WS_FLASH_COLOR_LENGTH + WS_FLASH_DUMMY_LENGTH) * (LED_TYPE_NUM - 2)) / 32


typedef struct
{
    uint32_t ptr;
    uint32_t data[WS_FLASH_LENGTH];
}msg_flash_t;



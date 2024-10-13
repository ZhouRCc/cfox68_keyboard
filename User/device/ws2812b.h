#pragma once

#include "stdint.h"
#include "tim.h"

#ifdef SYS_DEFINE
#include SYS_DEFINE
#endif

#include "FreeRTOS.h"
#include "cmsis_os2.h"


#define LED_NUM 74


typedef struct
{
    uint32_t freq_set;
    uint32_t freq_sys;
    uint8_t delay_tick;
}breath_t;//呼吸灯参数


typedef enum
{
    DUMMY_WORD = 1,
    RESET_WORD = 64,
    WS_H = 59,
    WS_L = 29,
    MAX_BRIGHTNESS = 255,
    MIN_BRIGHTNESS = 0

}ws_def_e;//ws2812b相关宏定义

typedef enum
{
    MODE_BREATH = 0,
    MODE_LIGHT,
    MODE_PRESS,
    MODE_NUM,
}ws_mode_e;

typedef union {
    uint32_t value;  // 整体作为一个32位整数

    struct {
        uint32_t is_on    : 1;   // bit0：表示是否开启 (0: 关闭, 1: 开启)
        uint32_t reserved : 3;   // bit1-3：保留为 1
        uint32_t mode     : 4;   // bit4-7：表示 LED 类型的模式
        uint32_t color    : 24;  // bit8-31：表示 LED 类型的颜色
    } fields;

} LED_Params;

typedef struct
{
    /*存放灯光参数的flash首地址
    一个uint32表示一个led类型的灯光参数
    bit0表示是否开启
    bit1-3保留为1
    bit4-7表示led类型的模式
    bit8-31表示led类型的颜色
    */
    uint32_t flash_addr;
    LED_Params ws_data[LED_TYPE_NUM];
}ws_flash_t;//ws2812b相关flash


class WS2812 {
public:
    WS2812();

    void ws2812Init();

    // ~WS2812();
    void setColor(uint32_t color);//设置所有灯的颜色

    void setColor(uint32_t color, led_type_e type);//设置指定类型灯的颜色

    void show(uint32_t color, led_type_e type);//根据color更新指定类型灯的颜色，不更新current_color

    void show(uint32_t color);//根据color更新所有灯的颜色，不更新current_color

    void show();//根据current_color更新所有灯的颜色

    void setMode(ws_mode_e mode, led_type_e type);//设置指定类型灯的模式

    void setBrightness(int brightness);//设置所有灯的亮度

    void setBrightness(int brightness, led_type_e type);//设置指定类型灯的亮度

    //设置呼吸灯参数，第一个参数为获取频率的函数指针，第二个参数为呼吸灯频率，单位:Hz
    void breathStep(uint32_t (*get_freq)(void) ,uint32_t freq);

    void breathingLight( led_type_e type);//指定类型的呼吸灯

    void breathingLight();//所有类型的呼吸灯

    void wsLoop();

private:

    uint32_t pwm_data[RESET_WORD + 24 * LED_NUM + DUMMY_WORD];//pwm数据
    breath_t breath_param;//呼吸灯参数
    ws_flash_t ws_flash;//ws2812b相关flash
    msg_flash_t msg_flash;//发送flash消息队列

    void update_flash();//更新flash
    uint32_t scale_color(uint32_t color, uint8_t target_brightness);//颜色亮度缩放
    void breath_circle(uint8_t* brightness ,uint8_t* increasing);//呼吸灯调整亮度
    void update_pwm();//更新pwm数据

    //清空当前颜色
    void clear() {
        for (int i = 0; i < LED_TYPE_NUM; i++) {
            ws_flash.ws_data[i].value = 0;
        }
    }
};





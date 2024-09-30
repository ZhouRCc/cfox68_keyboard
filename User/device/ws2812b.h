#pragma once

#include "stdint.h"
#include "tim.h"

#ifdef SYS_DEFINE
#include SYS_DEFINE
#endif

#include "FreeRTOS.h"
#include "cmsis_os2.h"


#define LED_NUM 74

typedef enum
{
    LED_Z = 0,//轴灯
    LED_D = 1//底灯
}led_type_e;

typedef struct
{
    uint32_t freq_set;
    uint32_t freq_sys;
    uint8_t delay_tick;
}breath_t;


typedef enum
{
    DUMMY_WORD = 1,
    RESET_WORD = 64,
    WS_H = 59,
    WS_L = 29,
    MAX_BRIGHTNESS = 255,
    MIN_BRIGHTNESS = 0

}ws_def_e;

class WS2812 {
public:
    WS2812();

    // ~WS2812();

    void setColor(uint32_t color, int dot_index);

    void setColor(uint32_t color);

    void setColor(uint32_t color, led_type_e type);

    void show(int dot_index, uint32_t color);

    void show();

    void setBrightness(int brightness, int dot_index);

    void setBrightness(int brightness);

    void setBrightness(int brightness, led_type_e type);

    void breathStep(uint32_t (*get_freq)(void) ,uint32_t freq);

    void breathingLight( led_type_e type);

    void breathingLight();

private:

    uint32_t pwm_data[RESET_WORD + 24 * LED_NUM + DUMMY_WORD];
    uint32_t current_color[LED_NUM];
    breath_t breath_param;

    uint32_t scale_color(uint32_t color, uint8_t target_brightness);
    void breath_circle(uint8_t* brightness ,uint8_t* increasing);

    void clear() {
        for (int i = 0; i < LED_NUM; i++) {
            current_color[i] = 0;
        }
    }
};





#include "device/ws2812b.h"

led_type_e led_type[LED_NUM] = {
    LED_Z,LED_Z,LED_Z,LED_Z,LED_Z,LED_Z,LED_Z,LED_Z,LED_Z,LED_Z,LED_Z,LED_Z,LED_Z,LED_Z,LED_Z,
    LED_Z,LED_Z,LED_Z,LED_Z,LED_Z,LED_Z,LED_Z,LED_Z,LED_Z,LED_Z,LED_Z,LED_Z,LED_Z,LED_Z,LED_Z,
    LED_Z,LED_Z,LED_Z,LED_Z,LED_Z,LED_Z,LED_Z,LED_Z,LED_Z,LED_Z,LED_Z,LED_Z,LED_Z,LED_Z,
    LED_Z,LED_Z,LED_Z,LED_Z,LED_Z,LED_Z,LED_Z,LED_Z,LED_Z,LED_Z,LED_Z,LED_Z,LED_Z,LED_Z,
    LED_Z,LED_Z,LED_Z,LED_Z,LED_Z,LED_Z,LED_Z,LED_Z,LED_Z,LED_Z,LED_D,LED_D,LED_D,LED_D,LED_D,LED_D
};

WS2812::WS2812() {
    clear();
    breath_param.freq_set = 0;
    breath_param.freq_sys = 0;
    breath_param.delay_tick = 0;
}

// WS2812::~WS2812() {
// }

void WS2812::setColor(uint32_t color, led_type_e type) {
    // 确保类型索引在允许的范围内
    if ( type >= LED_TYPE_NUM) return;
    
    // 设置该类型对应的颜色
    current_color[type] = color;

    // 遍历所有LED
    for (int i = 0; i < LED_NUM; i++) {
        // 判断当前LED是否属于指定的类型
        if (led_type[i] == type) {
            // 设置颜色值到pwm_data
            for (int j = 15; j >= 8; j--) { // G (绿)
                pwm_data[RESET_WORD + (i * 24 + (15 - j))] = ((current_color[type] >> j) & 0x01) ? WS_H : WS_L;
            }
            for (int j = 23; j >= 16; j--) { // R (红)
                pwm_data[RESET_WORD + (i * 24 + (31 - j))] = ((current_color[type] >> j) & 0x01) ? WS_H : WS_L;
            }
            for (int j = 7; j >= 0; j--) { // B (蓝)
                pwm_data[RESET_WORD + (i * 24 + (23 - j))] = ((current_color[type] >> j) & 0x01) ? WS_H : WS_L;
            }
        }
    }
}


void WS2812::setColor(uint32_t color) {
    int i = 0;
    led_type_e type;
    for(i = 0; i < LED_TYPE_NUM; i++) {
        current_color[i] = color;
    }
    // 遍历所有LED
    for (int i = 0; i < LED_NUM; i++) {
        type = led_type[i];
        // 设置颜色值到pwm_data
        for (int j = 15; j >= 8; j--) { // G (绿)
            pwm_data[RESET_WORD + (i * 24 + (15 - j))] = ((current_color[type] >> j) & 0x01) ? WS_H : WS_L;
        }
        for (int j = 23; j >= 16; j--) { // R (红)
            pwm_data[RESET_WORD + (i * 24 + (31 - j))] = ((current_color[type] >> j) & 0x01) ? WS_H : WS_L;
        }
        for (int j = 7; j >= 0; j--) { // B (蓝)
            pwm_data[RESET_WORD + (i * 24 + (23 - j))] = ((current_color[type] >> j) & 0x01) ? WS_H : WS_L;
        }
    }
}

void WS2812::show(uint32_t color, led_type_e type) {
    // 确保类型索引在允许的范围内
    if ( type >= LED_TYPE_NUM) return;

    // 遍历所有LED
    for (int i = 0; i < LED_NUM; i++) {
        // 判断当前LED是否属于指定的类型
        if (led_type[i] == type) {
            // 设置颜色值到pwm_data
            for (int j = 15; j >= 8; j--) { // G (绿)
                pwm_data[RESET_WORD + (i * 24 + (15 - j))] = ((color >> j) & 0x01) ? WS_H : WS_L;
            }
            for (int j = 23; j >= 16; j--) { // R (红)
                pwm_data[RESET_WORD + (i * 24 + (31 - j))] = ((color >> j) & 0x01) ? WS_H : WS_L;
            }
            for (int j = 7; j >= 0; j--) { // B (蓝)
                pwm_data[RESET_WORD + (i * 24 + (23 - j))] = ((color >> j) & 0x01) ? WS_H : WS_L;
            }
        }
    }
    HAL_TIM_PWM_Start_DMA(&htim2, TIM_CHANNEL_4, pwm_data, RESET_WORD + 24 * LED_NUM + DUMMY_WORD);
}

void WS2812::show(uint32_t color) {
     // 遍历所有LED
    for (int i = 0; i < LED_NUM; i++) {
        // 设置颜色值到pwm_data
        for (int j = 15; j >= 8; j--) { // G (绿)
            pwm_data[RESET_WORD + (i * 24 + (15 - j))] = ((color >> j) & 0x01) ? WS_H : WS_L;
        }
        for (int j = 23; j >= 16; j--) { // R (红)
            pwm_data[RESET_WORD + (i * 24 + (31 - j))] = ((color >> j) & 0x01) ? WS_H : WS_L;
        }
        for (int j = 7; j >= 0; j--) { // B (蓝)
            pwm_data[RESET_WORD + (i * 24 + (23 - j))] = ((color >> j) & 0x01) ? WS_H : WS_L;
        }
    }
    HAL_TIM_PWM_Start_DMA(&htim2, TIM_CHANNEL_4, pwm_data, RESET_WORD + 24 * LED_NUM + DUMMY_WORD);
}

void WS2812::show() {
    HAL_TIM_PWM_Start_DMA(&htim2, TIM_CHANNEL_4, pwm_data, RESET_WORD + 24 * LED_NUM + DUMMY_WORD);
}

void WS2812::setBrightness(int brightness) {
    uint32_t result_color[LED_TYPE_NUM] = {0};

    for (int i = 0; i < LED_TYPE_NUM; i++) {
        result_color[i] = scale_color(current_color[i], brightness);
        show(result_color[i], (led_type_e)i);
    }
}

void WS2812::setBrightness(int brightness, led_type_e type) {
    if( type >= LED_TYPE_NUM) return;

    uint32_t result_color = 0;
    result_color = scale_color(current_color[type], brightness);
    show(result_color, type);
}


void WS2812::breathStep(uint32_t (*get_freq)(void) ,uint32_t freq) {
    breath_param.freq_set = freq;
    breath_param.freq_sys = get_freq();
    breath_param.delay_tick = breath_param.freq_sys / breath_param.freq_set;
}

void WS2812::breathingLight(led_type_e type) {
    if(type >= LED_TYPE_NUM) return;

    static uint8_t brightness = 0;
    static uint8_t increasing = 1; // 控制亮度增加或减少的标志
    static uint8_t tick = 0;
    setBrightness(brightness, type); // 根据当前亮度显示 LED
    if(!breath_param.freq_set || tick >= breath_param.delay_tick)
    {
        breath_circle(&brightness ,&increasing);
        tick = 0;
    }
    else
        tick ++;
}

void WS2812::breathingLight() {
    static uint8_t brightness = 0;
    static uint8_t increasing = 1; // 控制亮度增加或减少的标志
    static uint8_t tick = 0;
    setBrightness(brightness); // 根据当前亮度显示 LED
    if(!breath_param.freq_set || tick >= breath_param.delay_tick)
    {
        breath_circle(&brightness ,&increasing);
        tick = 0;
    }
    else
        tick ++;
}

void WS2812::breath_circle(uint8_t* brightness ,uint8_t* increasing) {
    // 调整亮度
    if (*increasing) {
        *brightness += 5; // 增加亮度
        if (*brightness >= MAX_BRIGHTNESS) {
            *brightness = MAX_BRIGHTNESS;
            *increasing = 0; // 达到最大亮度后开始减少亮度
        }
    } else {
        *brightness -= 5; // 减少亮度
        if (*brightness <= MIN_BRIGHTNESS) {
            *brightness = MIN_BRIGHTNESS;
            *increasing = 1; // 达到最小亮度后开始增加亮度
        }
    }
}

uint32_t WS2812::scale_color(uint32_t color, uint8_t target_brightness) {
    uint32_t r = (color >> 16) & 0xFF;
    uint32_t g = (color >> 8) & 0xFF;
    uint32_t b = color & 0xFF;

    uint32_t scale_factor = target_brightness;

    r = (r * scale_factor) / 255;
    g = (g * scale_factor) / 255;
    b = (b * scale_factor) / 255;

    return (r << 16) | (g << 8) | b;
}


// 回调函数
void HAL_TIM_PWM_PulseFinishedCallback(TIM_HandleTypeDef *htim) {
    if (htim == &htim2) {
        HAL_TIM_PWM_Stop_DMA(&htim2, TIM_CHANNEL_4);
    }
}

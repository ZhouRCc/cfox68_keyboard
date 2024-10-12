#include "device/ws2812b.h"
#include "device/FlashDrv.h"
#include "task/userTask.h"

// 设置所有LED的类型
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


void WS2812::ws2812Init() {
    // uint32_t start_addr = ADDR_FLASH_SECTOR_6;
    // uint32_t lastNonEmptyAddress = 0;

    // // Traverse from the start to the end of the flash sector
    // for (start_addr = ADDR_FLASH_SECTOR_6; start_addr <= (ADDR_FLASH_SECTOR_7 - 4); start_addr += 4)
    // {
    //     uint32_t data = FlashDrv_read(); // Read the word at the current address
        
    //     // Check if the read data is not empty (0xFFFFFFFF is the default erased value in flash)
    //     if (data != 0xFFFFFFFF)
    //     {
    //         lastNonEmptyAddress = start_addr; // Update the last non-empty address
    //     }
    //     else
    //     {
    //         break; // If the read data is empty, break out of the loop
    //     }
    // }
    // // Update the flash address
    // if(lastNonEmptyAddress == 0) {
    //     ws_flash.flash_addr = ADDR_FLASH_SECTOR_6;
    // }
    // else{
    //     ws_flash.flash_addr = lastNonEmptyAddress - (WS_FLASH_LENGTH - 1) * 4;
    // }
    // for(int i = 0; i < WS_FLASH_LENGTH; i++) {
    //     ws_flash.ws_data[i].value = STMFLASH_ReadWord(ws_flash.flash_addr + i * 4);
    // }
    // for(int i = 0; i < LED_TYPE_NUM; i++) {
    //     ws_flash.ws_data.on[i] = ws_flash.flash_data[0] >> i & 0x01;
    // }
    // for(int i = 0; i < LED_TYPE_NUM; i++) {
    //     ws_flash.ws_data.mode[i] = (ws_mode_e)(ws_flash.flash_data[0] >> (WS_FLASH_ON_LENGTH + i * WS_FLASH_MODE_LENGTH) & 0x0F);
    // }
    // ws_flash.ws_data.color[0] = (ws_flash.flash_data[0] >> 16 | ws_flash.flash_data[1] << 16) & 0x00FFFFFF;
    // ws_flash.ws_data.color[1] = (ws_flash.flash_data[1] >> 8) & 0x00FFFFFF;
    // for(int i = 2; i < LED_TYPE_NUM; i++) {
    //     ws_flash.ws_data.color[i] = ws_flash.ws_data[i].value;
    // }
    
}
// WS2812::~WS2812() {
// }

void WS2812::setColor(uint32_t color, led_type_e type) {
    // 确保类型索引在允许的范围内
    if ( type >= LED_TYPE_NUM) return;
    
    // 设置该类型对应的颜色
    ws_flash.ws_data[type].fields.color = color;
    update_flash();
    // 遍历所有LED
    for (int i = 0; i < LED_NUM; i++) {
        // 判断当前LED是否属于指定的类型
        if (led_type[i] == type) {
            // 设置颜色值到pwm_data
            for (int j = 15; j >= 8; j--) { // G (绿)
                pwm_data[RESET_WORD + (i * 24 + (15 - j))] = ((ws_flash.ws_data[type].fields.color >> j) & 0x01) ? WS_H : WS_L;
            }
            for (int j = 23; j >= 16; j--) { // R (红)
                pwm_data[RESET_WORD + (i * 24 + (31 - j))] = ((ws_flash.ws_data[type].fields.color >> j) & 0x01) ? WS_H : WS_L;
            }
            for (int j = 7; j >= 0; j--) { // B (蓝)
                pwm_data[RESET_WORD + (i * 24 + (23 - j))] = ((ws_flash.ws_data[type].fields.color >> j) & 0x01) ? WS_H : WS_L;
            }
        }
    }
}


void WS2812::setColor(uint32_t color) {
    int i = 0;
    led_type_e type;
    for(i = 0; i < LED_TYPE_NUM; i++) {
        ws_flash.ws_data[i].fields.color = color;
    }
    update_flash();
    // 遍历所有LED
    for (int i = 0; i < LED_NUM; i++) {
        type = led_type[i];
        // 设置颜色值到pwm_data
        for (int j = 15; j >= 8; j--) { // G (绿)
            pwm_data[RESET_WORD + (i * 24 + (15 - j))] = ((ws_flash.ws_data[type].fields.color >> j) & 0x01) ? WS_H : WS_L;
        }
        for (int j = 23; j >= 16; j--) { // R (红)
            pwm_data[RESET_WORD + (i * 24 + (31 - j))] = ((ws_flash.ws_data[type].fields.color >> j) & 0x01) ? WS_H : WS_L;
        }
        for (int j = 7; j >= 0; j--) { // B (蓝)
            pwm_data[RESET_WORD + (i * 24 + (23 - j))] = ((ws_flash.ws_data[type].fields.color >> j) & 0x01) ? WS_H : WS_L;
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

void WS2812::setMode(ws_mode_e mode, led_type_e type) {
    if( type >= LED_TYPE_NUM) return;
    ws_flash.ws_data[type].fields.mode = mode;
    update_flash();
}

void WS2812::setBrightness(int brightness) {
    uint32_t result_color[LED_TYPE_NUM] = {0};

    for (int i = 0; i < LED_TYPE_NUM; i++) {
        result_color[i] = scale_color(ws_flash.ws_data[i].fields.color, brightness);
        show(result_color[i], (led_type_e)i);
    }
}

void WS2812::setBrightness(int brightness, led_type_e type) {
    if( type >= LED_TYPE_NUM) return;

    uint32_t result_color = 0;
    result_color = scale_color(ws_flash.ws_data[type].fields.color, brightness);
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

void WS2812::wsLoop() {
    for(int i = 0; i < LED_TYPE_NUM; i++) {
        if(ws_flash.ws_data[i].fields.mode == MODE_BREATH) {
            breathingLight((led_type_e)i);
        }
    }
}

void WS2812::update_flash() {
    // ws_flash.flash_addr += WS_FLASH_LENGTH * 4;
    // ws_flash.flash_data[0] = ws_flash.ws_data.on[0] | (ws_flash.ws_data.on[1] << 1);
    // ws_flash.flash_data[0] |= (ws_flash.ws_data.mode[0] << WS_FLASH_ON_LENGTH) | (ws_flash.ws_data.mode[1] << (WS_FLASH_ON_LENGTH + WS_FLASH_MODE_LENGTH));
    // ws_flash.flash_data[0] |= ((ws_flash.ws_data.color[0] << 16) & 0xFFFF0000);
    // ws_flash.flash_data[1] = (ws_flash.ws_data.color[0] >> 16 & 0xFF) | ((ws_flash.ws_data.color[1] << 8) & 0xFFFFFF00);
    // for(int i = 2; i < WS_FLASH_LENGTH; i++) {
    //     ws_flash.ws_data[i].value = ws_flash.ws_data.color[i];
    // }
    flash_send();
}

void WS2812::flash_send() {
    msg_flash.ptr = ws_flash.flash_addr;
    for(int i = 0; i < WS_FLASH_LENGTH; i++) {
        msg_flash.data[i] = ws_flash.ws_data[i].value;
    }
    osMessageQueueReset(robotStruct.msgq.q_flash_send);
    osMessageQueuePut(robotStruct.msgq.q_flash_send, &msg_flash, 1, 0);
}

// 回调函数
void HAL_TIM_PWM_PulseFinishedCallback(TIM_HandleTypeDef *htim) {
    if (htim == &htim2) {
        HAL_TIM_PWM_Stop_DMA(&htim2, TIM_CHANNEL_4);
    }
}



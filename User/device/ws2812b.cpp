#include "device/ws2812b.h"
#include "device/FlashDrv.h"
#include "task/userTask.h"
#include <string.h>
// 设置所有LED的类型
led_type_e led_type[LED_NUM] = {
    LED_Z,LED_Z,LED_Z,LED_Z,LED_Z,LED_Z,LED_Z,LED_Z,LED_Z,LED_Z,LED_Z,LED_Z,LED_Z,LED_Z,LED_Z,
    LED_Z,LED_Z,LED_Z,LED_Z,LED_Z,LED_Z,LED_Z,LED_Z,LED_Z,LED_Z,LED_Z,LED_Z,LED_Z,LED_Z,LED_Z,
    LED_Z,LED_Z,LED_Z,LED_Z,LED_Z,LED_Z,LED_Z,LED_Z,LED_Z,LED_Z,LED_Z,LED_Z,LED_Z,LED_Z,
    LED_Z,LED_Z,LED_Z,LED_Z,LED_Z,LED_Z,LED_Z,LED_Z,LED_Z,LED_Z,LED_Z,LED_Z,LED_Z,LED_Z,
    LED_Z,LED_Z,LED_Z,LED_Z,LED_Z,LED_Z,LED_Z,LED_Z,LED_Z,LED_Z,LED_D,LED_D,LED_D,LED_D,LED_D,LED_D
};

const uint32_t rgb_value[8] = {0x300030, 0x301520, 0x243020, 0x151200, 0x202020, 0x300000, 0x003000, 0x000030};// RGB颜色预设值

#define RGB_TOG_NUM 6
#define RGB_TOG_HZ 1

WS2812::WS2812() {
    clear();

    rgb_control.rgb_index = 0;
    rgb_control.rgb_type = LED_Z;
    rgb_control.rgb_tog_cnt = 0;
    rgb_control.rgb_tog_tick = 0;
    rgb_control.need_tog = false;

    //默认频率设置
    breath_param.freq_set = 50;
    breath_param.freq_sys = 500;
    breath_param.delay_tick = breath_param.freq_sys / breath_param.freq_set;
    rgb_control.rgb_tog_delay = breath_param.freq_sys / RGB_TOG_HZ;

    memset(&press_pram,0,sizeof(press_pram));
    *press_pram.prio = 1;
    press_pram.press_delay = 520;



}


void WS2812::ws2812Init() {

    // addr = Find_Last_NonEmpty_Address(ADDR_FLASH_SECTOR_6, ADDR_FLASH_SECTOR_7);
    ws_flash.flash_addr = ADDR_FLASH_SECTOR_7;
    // if( addr == ADDR_FLASH_SECTOR_6){
    //     ws_flash.flash_addr = ADDR_FLASH_SECTOR_6;
    // }
    // else{
    //     ws_flash.flash_addr = addr - (LED_TYPE_NUM - 1) * 4;
    // }
    for(int i = 0; i < LED_TYPE_NUM; i++) {
        ws_flash.ws_data[i].value = Read_Flash_Uint32(ws_flash.flash_addr + i * 4);
    }
    update_pwm();
    show();
    for(int i = 0; i < LED_TYPE_NUM; i++) {
        if(!(ws_flash.ws_data[i].fields.is_on)) {
            show(0x000000, (led_type_e)i);
        }
        if(ws_flash.ws_data[0].fields.mode == MODE_PRESS) {
            show(0x000000, LED_Z);
        }
    }
    
}
// WS2812::~WS2812() {
// }

void WS2812::setColor(uint32_t color, led_type_e type) {
    // 确保类型索引在允许的范围内
    if ( type >= LED_TYPE_NUM) return;
    
    // 设置该类型对应的颜色
    ws_flash.ws_data[type].fields.color = color;
    update_flash();
    update_pwm();
}

void WS2812::setColor_no_pwm(uint32_t color, led_type_e type) {
    // 确保类型索引在允许的范围内
    if ( type >= LED_TYPE_NUM) return;
    
    // 设置该类型对应的颜色
    ws_flash.ws_data[type].fields.color = color;
    update_flash();
}

void WS2812::setColor_show(uint32_t color, led_type_e type) {
    // 确保类型索引在允许的范围内
    if ( type >= LED_TYPE_NUM) return;
    
    // 设置该类型对应的颜色
    ws_flash.ws_data[type].fields.color = color;
    update_flash();
    update_pwm();
    show();
}

void WS2812::setColor(uint32_t color) {
    int i = 0;
    for(i = 0; i < LED_TYPE_NUM; i++) {
        ws_flash.ws_data[i].fields.color = color;
    }
    update_flash();
    update_pwm();
}

void WS2812::update_pwm()
{
    led_type_e type;
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

void WS2812::show(uint32_t color, uint16_t dot) {
    // 设置颜色值到pwm_data
    for (int j = 15; j >= 8; j--) { // G (绿)
        pwm_data[RESET_WORD + (dot * 24 + (15 - j))] = ((color >> j) & 0x01) ? WS_H : WS_L;
    }
    for (int j = 23; j >= 16; j--) { // R (红)
        pwm_data[RESET_WORD + (dot * 24 + (31 - j))] = ((color >> j) & 0x01) ? WS_H : WS_L;
    }
    for (int j = 7; j >= 0; j--) { // B (蓝)
        pwm_data[RESET_WORD + (dot * 24 + (23 - j))] = ((color >> j) & 0x01) ? WS_H : WS_L;
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

void WS2812::setMode(ws_mode_e mode) {
    for (int i = 0; i < LED_TYPE_NUM; i++) {
        ws_flash.ws_data[i].fields.mode = mode;
    }
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
    rgb_control.rgb_tog_delay = breath_param.freq_sys / RGB_TOG_HZ;
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

void WS2812::deal_flags() {
    thread_flag = osThreadFlagsGet();
    if(thread_flag >> (RGB_TOG - RGB_START) & 0x01) {
        led_troggle((led_type_e)rgb_control.rgb_type);
        update_flash();
        osThreadFlagsClear((1 << (RGB_TOG - RGB_START)));
    }else if(thread_flag >> (RGB_MOD - RGB_START) & 0x01) {
        led_mod_change(LED_Z);// 没想好咋写，先只支持改底灯
        osThreadFlagsClear((1 << (RGB_MOD - RGB_START)));
    }else if(thread_flag >> (RGB_COLOR - RGB_START) & 0x01) {
        if(ws_flash.ws_data[rgb_control.rgb_type].fields.mode == MODE_BREATH) {
            setColor(rgb_value[rgb_control.rgb_index], (led_type_e)rgb_control.rgb_type);
        }else if(ws_flash.ws_data[rgb_control.rgb_type].fields.mode == MODE_LIGHT) {
            setColor_show(rgb_value[rgb_control.rgb_index], ( led_type_e)rgb_control.rgb_type);
        }else if(ws_flash.ws_data[rgb_control.rgb_type].fields.mode == MODE_PRESS) {
            setColor_no_pwm(rgb_value[rgb_control.rgb_index], ( led_type_e)rgb_control.rgb_type);
        }else{
            ;
        }
        if(rgb_control.rgb_index < 7)
        {
            rgb_control.rgb_index ++;
        }
        else
        {
            rgb_control.rgb_index = 0;
        }
        osThreadFlagsClear((1 << (RGB_COLOR - RGB_START)));
    }else if(thread_flag >> (RGB_TYPE - RGB_START) & 0x01) {
        led_type_changed();
        osThreadFlagsClear((1 << (RGB_TYPE - RGB_START)));
    }else{
        ;
    }
    osThreadFlagsClear(0xffffffff);// 清除所有标志位，一次只允许进行一种更改
}

void WS2812::wsLoop() {
    for(int i = 0; i < LED_TYPE_NUM; i++) {
        if(rgb_control.need_tog) {
            led_tog_loop();
        }
        else {
            if(ws_flash.ws_data[i].fields.is_on) {
                if(ws_flash.ws_data[i].fields.mode == MODE_BREATH) {
                    breathingLight((led_type_e)i);
                }else if(ws_flash.ws_data[i].fields.mode == MODE_PRESS) {
                    press_loop();
                }
                else {
                    ;
                }
            }
            else {
                ;
            }
        }
    }
}


void WS2812::press_loop() {
    osMessageQueueGet(robotStruct.msgq.q_key_press,press_pram.msg_key,press_pram.prio,0);
    for(int i = 0; i < KEY_NUM; i ++) {
        if(press_pram.msg_key[i])
        {
            if(press_pram.need_light[i]) {
                press_pram.press_tick[i] = 1;
            }else {
                press_pram.need_light[i] = true;
                press_pram.press_tick[i] = 0;
            }
        }
    }
    for(int i = 0; i < LED_NUM; i ++)
    {
        int led_d_num = 0;
        if(led_type[i] == LED_Z) {
            if(press_pram.need_light[i - led_d_num]) {
                if(press_pram.press_tick[i - led_d_num] == 0) {
                    show(ws_flash.ws_data[LED_Z].fields.color,i);
                    press_pram.press_tick[i - led_d_num] ++;
                }else if(press_pram.press_tick[i - led_d_num] < press_pram.press_delay) {
                    press_pram.press_tick[i - led_d_num] ++;
                }else {
                    show(0x000000,i);
                    press_pram.press_tick[i - led_d_num] = 0;
                    press_pram.need_light[i - led_d_num] = false;
                }
            }else {
                ;
            }
        }
        else {
            led_d_num ++;
        }
    }
}

void WS2812::update_flash() {
    msg_flash.ptr = ws_flash.flash_addr;
    for (int i = 0; i < LED_TYPE_NUM; i++)
    {
        uint32_t value = ws_flash.ws_data[i].value;  // 读取union中的value

        // 将32位的 value 拆解成 4 个字节，并存入 msg_flash.data
        msg_flash.data[i * 4]     = (value & 0xFF);           // 取低 8 位
        msg_flash.data[i * 4 + 1] = (value >> 8) & 0xFF;      // 取第 8-15 位
        msg_flash.data[i * 4 + 2] = (value >> 16) & 0xFF;     // 取第 16-23 位
        msg_flash.data[i * 4 + 3] = (value >> 24) & 0xFF;     // 取高 8 位
    }
    osMessageQueueReset(robotStruct.msgq.q_flash_send);
    osMessageQueuePut(robotStruct.msgq.q_flash_send, &msg_flash, 1, 0);
}

void WS2812::led_troggle( led_type_e type) {
    if(type > LED_TYPE_NUM) return;

    if(ws_flash.ws_data[type].fields.is_on) {
        show(0x000000 , type);
        ws_flash.ws_data[type].fields.is_on = false;
    }
    else {
        show(ws_flash.ws_data[type].fields.color, type);
        ws_flash.ws_data[type].fields.is_on = true;
    }

    // update_flash();
}

void WS2812::led_troggle() {
    for(int i = 0; i < LED_TYPE_NUM; i++) {
        
        if(ws_flash.ws_data[i].fields.is_on) {
            show(0x000000 , (led_type_e)i);
            ws_flash.ws_data[i].fields.is_on = false;
        }
        else {
            show(ws_flash.ws_data[i].fields.color,  (led_type_e)i);
            ws_flash.ws_data[i].fields.is_on = true;
        }
    }
    update_flash();
}

void WS2812::led_type_changed() {
    if(rgb_control.rgb_type < LED_TYPE_NUM - 1) {
        rgb_control.rgb_type ++;
    }else {
        rgb_control.rgb_type = LED_Z;
    }
    rgb_control.need_tog = true;
}

void WS2812::led_tog_loop() {
    if(rgb_control.rgb_tog_cnt < RGB_TOG_NUM) {
        if(rgb_control.rgb_tog_tick < rgb_control.rgb_tog_delay) {
            rgb_control.rgb_tog_tick ++;
        }
        else {
            rgb_control.rgb_tog_tick = 0;
            led_troggle((led_type_e)rgb_control.rgb_type);
            rgb_control.rgb_tog_cnt ++;
        }
    }
    else {
        rgb_control.rgb_tog_cnt = 0;
        rgb_control.rgb_tog_tick = 0;
        rgb_control.need_tog = false;
    }
    
}

void WS2812::led_mod_change( led_type_e type) {
    if(type >= LED_TYPE_NUM) return;

    if(ws_flash.ws_data[type].fields.mode == MODE_BREATH) {
        setMode(MODE_LIGHT, type);
        show(ws_flash.ws_data[type].fields.color, type);
    }else if(ws_flash.ws_data[type].fields.mode == MODE_LIGHT) {
        setMode(MODE_PRESS, type);
        if(type == LED_Z)
        {
            show(0x000000, type);
        }
    }else {
        setMode(MODE_BREATH, type);
    }
}

// 回调函数
void HAL_TIM_PWM_PulseFinishedCallback(TIM_HandleTypeDef *htim) {
    if (htim == &htim2) {
        HAL_TIM_PWM_Stop_DMA(&htim2, TIM_CHANNEL_4);
    }
}



#include "key_scan.hpp"
#include "main.h"
#include "usbd_hid.h"
#include <cmsis_os2.h>
#include "bsp/delay.h"
#include "task/userTask.h"
extern USBD_HandleTypeDef hUsbDeviceFS;  // 外部声明USB设备句柄

const uint8_t keymaps[][MATRIX_ROWS][MATRIX_COLS] = {
{
{KC_ESC,  KC_1,    KC_2,   KC_3,  KC_4,  KC_5,   KC_6,   KC_7,   KC_8,   KC_9,    KC_0,    KC_MINS, KC_EQL,  KC_BSPC, KC_GRV},
{KC_TAB,  KC_Q,    KC_W,   KC_E,  KC_R,  KC_T,   KC_Y,   KC_U,   KC_I,   KC_O,    KC_P,    KC_LBRC, KC_RBRC, KC_BSLS, KC_DEL},
{KC_CAPS, KC_A,    KC_S,   KC_D,  KC_F,  KC_G,   KC_H,   KC_J,   KC_K,   KC_L,    KC_SCLN, KC_QUOT, KC_ENTER,KC_NO,   KC_INS},
{KC_LSFT, KC_Z,    KC_X,   KC_C,  KC_V,  KC_B,   KC_N,   KC_M,   KC_COMM,KC_DOT,  KC_SLSH, KC_RSFT, KC_NO,   KC_UP,   KC_SHELL},
{KC_LCTL, KC_LGUI, KC_LALT,KC_NO, KC_NO, KC_SPC, KC_NO,  KC_NO,  KC_NO,  MO_1,    KC_COPY, KC_PSTE, KC_LEFT, KC_DOWN, KC_RGHT}
},
{
{RGB_TOG, KC_F1,  KC_F2,  KC_F3,  KC_F4,  KC_F5,   KC_F6,  KC_F7,  KC_F8,  KC_F9,   KC_F10,   KC_F11,  KC_F12,  KC_NO,    KC_CAPS},
{KC_NO,   KC_NO,  KC_NO,  KC_NO,  KC_NO,  KC_NO,   KC_NO,  KC_NO,  KC_NO,  KC_NO,   KC_NO,    KC_NO,   KC_NO,   KC_NO,    KC_MUTE},
{KC_NO,   KC_NO,  KC_NO,  KC_NO,  KC_NO,  KC_NO,   KC_NO,  KC_NO,  KC_NO,  KC_NO,   KC_NO,    KC_NO,   KC_RSET, KC_NO,    KC_NO},
{KC_NO,   KC_NO,  KC_NO,  KC_NO,  KC_NO,  KC_NO,   KC_NO,  KC_NO,  KC_NO,  RGB_TYPE,RGB_COLOR,KC_NO,   KC_NO,   KC_VOL_U, KC_NO},
{KC_NO,   KC_NO,  KC_NO,  KC_NO,  KC_NO,  RGB_MOD, KC_NO,  KC_NO,  KC_NO,  KC_NO,   KC_COPY,  KC_PSTE, KC_BRI_D,KC_VOL_D, KC_BRI_U}
}
};


// 列和行的数组
uint16_t col_pins[MATRIX_COLS] = {COL0_Pin, COL1_Pin, COL2_Pin, COL3_Pin, COL4_Pin, COL5_Pin, COL6_Pin, 
    COL7_Pin, COL8_Pin, COL9_Pin, COL10_Pin, COL11_Pin, COL12_Pin, COL13_Pin, COL14_Pin};
GPIO_TypeDef* col_ports[MATRIX_COLS] = {COL0_GPIO_Port, COL1_GPIO_Port, COL2_GPIO_Port, COL3_GPIO_Port, COL4_GPIO_Port, 
    COL5_GPIO_Port, COL6_GPIO_Port, COL7_GPIO_Port, COL8_GPIO_Port, COL9_GPIO_Port, COL10_GPIO_Port, COL11_GPIO_Port, 
    COL12_GPIO_Port, COL13_GPIO_Port, COL14_GPIO_Port};
uint16_t row_pins[MATRIX_ROWS] = {ROW0_Pin, ROW1_Pin, ROW2_Pin, ROW3_Pin, ROW4_Pin};
GPIO_TypeDef* row_ports[MATRIX_ROWS] = {ROW0_GPIO_Port, ROW1_GPIO_Port, ROW2_GPIO_Port, ROW3_GPIO_Port, ROW4_GPIO_Port};



KeyScan::KeyScan() {
    for(int i = 0; i < 8; i++) {
        hid_report.buffer[i] = 0;
    }
    memset(&key_buff, 0, sizeof(key_buff));
    memset(&rgb_key, 0, sizeof(rgb_key));
    hid_report.report.report_id = REPORT_ID_KEY;
    audio_u.hid_audio.report_id = REPORT_ID_AUDIO;
    audio_u.hid_audio.key = 0;
    bri_u.hid_bri.report_id = REPORT_ID_BRI;
    bri_u.hid_bri.key = 0;
}

// 按键扫描函数
void KeyScan::operator()() {
    uint8_t key = 0;
    key_buff.key_count = 0;
    for (int col = 0; col < MATRIX_COLS; col++) {
        if(key_buff.key_count >= 14) break;
        // 设置当前列引脚为高电平
        HAL_GPIO_WritePin(col_ports[col], col_pins[col], GPIO_PIN_SET);
        delay_us(30); 
        // 读取行的状态
        for (int row = 0; row < MATRIX_ROWS; row++) {
            if(HAL_GPIO_ReadPin(row_ports[row], row_pins[row]) == GPIO_PIN_SET)
            {
                key = keymaps[0][row][col];
                if(key == MO_1)
                    key_buff.fn_pressed = true;
                key_buff.col[key_buff.key_count] = col;
                key_buff.row[key_buff.key_count] = row;
                key_buff.key_count ++;
                key_press_buffer[row][col] = true;
            }
            else
            {
                key_press_buffer[row][col] = false;
            }
        }

        // 恢复当前列引脚为低电平
        HAL_GPIO_WritePin(col_ports[col], col_pins[col], GPIO_PIN_RESET);
        delay_us(30); 
    }

    process_and_send_keys();
}

void KeyScan::process_and_send_keys() {
    bool have_custom_keys = false;
    uint8_t keys_index = 0;  // 普通按键的索引
    memset(rgb_key.rgb_array, 0, sizeof(rgb_key.rgb_array));//清空rgb数组
    // 遍历缓冲区中的按键
    for (int i = 0; i < key_buff.key_count; i++) {
        uint8_t key = keymaps[key_buff.fn_pressed][key_buff.row[i]][key_buff.col[i]];
        
        if (key_is_modifier(key)) {
            // 如果是修饰键，将其添加到修饰键字节中
            hid_report.report.modifier |= get_modifier_mask(key);
        } 
        else if(key > KC_CUSTOM)
        {
            have_custom_keys = true;
            process_custom_keys(key,&keys_index);
        }
        else if(key >= RGB_START && key <= RGB_NUM) {
            rgb_key.rgb_array[key - RGB_START] = true;
        }
        else{
            // 如果是普通按键，存入报告数组（最多6个按键）
            if (keys_index < 6) {
                hid_report.report.keys[keys_index++] = key;
            }
        }
    }
    if(!have_custom_keys)
    {
        report_cnt =0;
        bri_cnt[0] = 0;
        bri_cnt[1] = 0;
        audio_cnt[1] = 0;
        audio_cnt[0] = 0;
        audio_cnt[2] = 0;
    }
    if(last_report == 1 && report_cnt != 1){
        report_cnt = 3;
    }else if(last_report == 2 && report_cnt != 2){
        report_cnt = 4;
    }else{
        ;
    }
    send_report();
    process_rgb();
    update_msg();

}

void KeyScan::update_msg() {
    int key_index = 0;
    for (int r = 0; r < MATRIX_ROWS; r++) {
        for (int c = 0; c < MATRIX_COLS; c++) {
            if(key_press_buffer[r][c])
            {
                msg_key[key_index] = true;
            }
            if (keymaps[0][r][c] != KC_NO) {
                key_index ++;
            }
        }
    }
    msg_send();
}

void KeyScan::msg_send() {
    osMessageQueueReset(robotStruct.msgq.q_key_press);
    osMessageQueuePut(robotStruct.msgq.q_key_press,msg_key,1,0);
    memset(msg_key,0,sizeof(msg_key));
}

void KeyScan::send_report() {
    switch(report_cnt) {
        case 0:
            // 通过联合体的字节数组buffer发送USB HID报告
            USBD_HID_SendReport(&hUsbDeviceFS, hid_report.buffer, sizeof(hid_report.buffer));
            memset(&key_buff, 0, sizeof(key_buff));
            // 清空当前的修饰键和按键数组
            hid_report.report.modifier = 0;
            for (int i = 0; i < 6; i++) {
                hid_report.report.keys[i] = 0;
            }
            break;
        case 1:
            USBD_HID_SendReport(&hUsbDeviceFS, audio_u.buffer, sizeof(audio_u.buffer));
            audio_u.hid_audio.key = 0;
            break;
        case 2:
            USBD_HID_SendReport(&hUsbDeviceFS, bri_u.buffer, sizeof(bri_u.buffer));
            bri_u.hid_bri.key = 0;
            break;
        case 3:
            audio_u.hid_audio.key = 0;
            USBD_HID_SendReport(&hUsbDeviceFS, audio_u.buffer, sizeof(audio_u.buffer));
            report_cnt = 0;
            break;
        case 4:
            bri_u.hid_bri.key = 0;
            USBD_HID_SendReport(&hUsbDeviceFS, bri_u.buffer, sizeof(bri_u.buffer));
            report_cnt = 0;
            break;
        default:
            report_cnt = 0;
            break;
    }
    last_report = report_cnt;
}

void KeyScan::process_rgb()
{
    for(int i = 0; i < RGB_NUM - RGB_START; i++) {
        if(rgb_key.rgb_array[i]) {
            if(rgb_key.rgb_key_cnt[i] <= 100){
                rgb_key.rgb_key_cnt[i] ++;
            }else if(rgb_key.rgb_key_cnt[i] > 100 && rgb_key.rgb_key_cnt[i] <= 1000){
                osThreadFlagsSet(robotStruct.thread.rgb, (1 << i));
                rgb_key.rgb_key_cnt[i] = 1200;
            }else{
                ;
            }
        }
        else {
            rgb_key.rgb_key_cnt[i] = 0;
        }
    }
}

bool KeyScan::key_is_modifier(uint8_t key) {
    return (key >= KC_LCTL && key <= KC_RGUI);
}

uint8_t KeyScan::get_modifier_mask(uint8_t key) {
    switch (key) {
        case KC_LCTL:   return 0x01;
        case KC_LSFT:   return 0x02;
        case KC_LALT:   return 0x04;
        case KC_LGUI:   return 0x08;
        case KC_RCTL:   return 0x10;
        case KC_RSFT:   return 0x20;
        case KC_RALT:   return 0x40;
        case KC_RGUI:   return 0x80;
        default:        return 0x00;
    }
}


void KeyScan::process_custom_keys(uint8_t key, uint8_t* index) {
    switch (key)
    {
        case KC_PSTE:
            report_cnt =0;
            if(key_buff.fn_pressed) 
                hid_report.report.modifier |= get_modifier_mask(KC_RSFT);
            hid_report.report.modifier |= get_modifier_mask(KC_LCTL);
            hid_report.report.keys[*index ++] = KC_V;
            break;
        case KC_COPY:
            report_cnt =0;
            if(key_buff.fn_pressed) 
                hid_report.report.modifier |= get_modifier_mask(KC_RSFT);
            hid_report.report.modifier |= get_modifier_mask(KC_LCTL);
            hid_report.report.keys[*index ++] = KC_C;
            break;
        case KC_SHELL:
            report_cnt =0;
            hid_report.report.modifier |= get_modifier_mask(KC_LALT);
            hid_report.report.modifier |= get_modifier_mask(KC_LCTL);
            hid_report.report.keys[*index ++] = KC_T;
            break;
        case KC_RSET:
            HAL_NVIC_SystemReset();
            break;
        case KC_VOL_D:
            report_cnt = 1;
            audio_cnt[1] = 0;
            audio_cnt[2] = 0;
            bri_cnt[0] = 0;
            bri_cnt[1] = 0;
            if(audio_cnt[0] < 16000){
                audio_cnt[0] ++;
            }else {
                audio_cnt[0] = 0;
            }
            if(audio_cnt[0] == 3){
                audio_u.hid_audio.key = 0x02;
            }else if(audio_cnt[0] > 200){
                audio_u.hid_audio.key = 0x02;
            }else{
                audio_u.hid_audio.key = 0x00;
            }
            break;
        case KC_VOL_U:
            report_cnt = 1;
            audio_cnt[0] = 0;
            audio_cnt[2] = 0;
            bri_cnt[0] = 0;
            bri_cnt[1] = 0;
            if(audio_cnt[1] < 16000){
                audio_cnt[1] ++;
            }else {
                audio_cnt[1] = 0;
            }
            if(audio_cnt[1] == 3){
                audio_u.hid_audio.key = 0x01;
            }else if(audio_cnt[1] > 200){
                audio_u.hid_audio.key = 0x01;
            }else{
                audio_u.hid_audio.key = 0x00;
            }
            break;
        case KC_MUTE:
            report_cnt = 1;
            audio_cnt[1] = 0;
            audio_cnt[0] = 0;
            bri_cnt[0] = 0;
            bri_cnt[1] = 0;
            if(audio_cnt[2] == 0){
                audio_u.hid_audio.key = 0x04;
            }else {
                audio_u.hid_audio.key = 0x00;
                audio_cnt[2] = 300;
            }
            break;
        case KC_BRI_D:
            report_cnt = 2;
            bri_cnt[1] = 0;
            audio_cnt[1] = 0;
            audio_cnt[0] = 0;
            audio_cnt[2] = 0;
            if(bri_cnt[0] < 16000){
                bri_cnt[0] ++;
            }else{
                bri_cnt[0] = 0;
            }
            if(bri_cnt[0] == 3){
                bri_u.hid_bri.key = 0x02;
            }else if(bri_cnt[0] > 200){
                bri_u.hid_bri.key = 0x02;
            }else{
                bri_u.hid_bri.key = 0x00;
            }
            break;
        case KC_BRI_U:
            report_cnt = 2;
            bri_cnt[0] = 0;
            audio_cnt[1] = 0;
            audio_cnt[0] = 0;
            audio_cnt[2] = 0;
            if(bri_cnt[1] < 16000){
                bri_cnt[1] ++;
            }else{
                bri_cnt[1] = 0;
            }
            if(bri_cnt[1] == 3){
                bri_u.hid_bri.key = 0x01;
            }else if(bri_cnt[1] > 200){
                bri_u.hid_bri.key = 0x01;
            }else{
                bri_u.hid_bri.key = 0x00;
            }
            break;
        default:
            break;
    }
}


//test  =========```````````````` \\\\\\\\\`  -=-=-=-=-=-=-========
//===wwwwwwwwwwwwwww===qqqq
//========qqqqqweee
//===============
//


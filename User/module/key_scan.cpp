#include "key_scan.hpp"
#include "main.h"
#include "usbd_hid.h"
#include <cmsis_os2.h>
#include "bsp/delay.h"
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
{RGB_TOG, KC_F1,  KC_F2,  KC_F3,  KC_F4,  KC_F5,   KC_F6,  KC_F7,  KC_F8,  KC_F9,   KC_F10,  KC_F11,  KC_F12,  KC_NO,   KC_CAPS},
{KC_NO,   KC_NO,  KC_NO,  KC_NO,  RGB_RI, RGB_RD,  KC_NO,  KC_NO,  KC_NO,  KC_NO,   KC_NO,   KC_NO,   KC_NO,   KC_NO,   RGB_TOG},
{KC_NO,   KC_NO,  KC_NO,  KC_NO,  KC_NO,  RGB_GI,  RGB_GD, KC_NO,  KC_NO,  KC_NO,   KC_NO,   KC_NO,   KC_RSET, KC_NO,   KC_NO},
{KC_NO,   KC_NO,  KC_NO,  KC_NO,  KC_NO,  RGB_BI,  RGB_BD, KC_NO,  KC_NO,  KC_NO,   KC_NO,   KC_NO,   KC_NO,   RGB_LI,  KC_NO},
{KC_NO,   KC_NO,  KC_NO,  KC_NO,  KC_NO,  RGB_MOD, KC_NO,  KC_NO,  KC_NO,  KC_NO,   KC_COPY, KC_PSTE, KC_NO,   RGB_LD,  KC_NO}
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
}

// 按键扫描函数
void KeyScan::operator()() {
    uint8_t key = 0;
    key_buff.key_count = 0;
    for (int col = 0; col < MATRIX_COLS; col++) {
        if(key_buff.key_count >= 14) break;
        // 设置当前列引脚为高电平
        HAL_GPIO_WritePin(col_ports[col], col_pins[col], GPIO_PIN_SET);

        // 读取行的状态
        for (int row = 0; row < MATRIX_ROWS; row++) {
            if(HAL_GPIO_ReadPin(row_ports[row], row_pins[row]) == GPIO_PIN_SET)
            {
                key = keymaps[0][row][col];
                if(key == MO_1)
                    key_buff.fn_pressed = true;
                key_buff.col[key_buff.key_count] = col;
                key_buff.row[key_buff.key_count ++] = row;
            }
        }

        // 恢复当前列引脚为低电平
        HAL_GPIO_WritePin(col_ports[col], col_pins[col], GPIO_PIN_RESET);
        delay_us(60); 
    }

    process_and_send_keys();
}

void KeyScan::process_and_send_keys() {
    uint8_t keys_index = 0;  // 普通按键的索引
    // 遍历缓冲区中的按键
    for (int i = 0; i < key_buff.key_count; i++) {
        uint8_t key = keymaps[key_buff.fn_pressed][key_buff.row[i]][key_buff.col[i]];
        
        if (key_is_modifier(key)) {
            // 如果是修饰键，将其添加到修饰键字节中
            hid_report.report.modifier |= get_modifier_mask(key);
        } 
        else if(key >= KC_RSET)
        {
            process_custom_keys(key,&keys_index);
        }
        else if(key >= RGB_TOG && key <= RGB_LD) {
            // process_rgb(key);
        }
        else{
            // 如果是普通按键，存入报告数组（最多6个按键）
            if (keys_index < 6) {
                hid_report.report.keys[keys_index++] = key;
            }
        }
    }
    // 通过联合体的字节数组buffer发送USB HID报告
    USBD_HID_SendReport(&hUsbDeviceFS, hid_report.buffer, sizeof(hid_report.buffer));
    memset(&key_buff, 0, sizeof(key_buff));
    // 清空当前的修饰键和按键数组
    hid_report.report.modifier = 0;
    for (int i = 0; i < 6; i++) {
        hid_report.report.keys[i] = 0;
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
            if(key_buff.fn_pressed) 
                hid_report.report.modifier |= get_modifier_mask(KC_RSFT);
            hid_report.report.modifier |= get_modifier_mask(KC_LCTL);
            hid_report.report.keys[*index ++] = KC_V;
            break;
        case KC_COPY:
            if(key_buff.fn_pressed) 
                hid_report.report.modifier |= get_modifier_mask(KC_RSFT);
            hid_report.report.modifier |= get_modifier_mask(KC_LCTL);
            hid_report.report.keys[*index ++] = KC_C;
            break;
        case KC_SHELL:
            hid_report.report.modifier |= get_modifier_mask(KC_LALT);
            hid_report.report.modifier |= get_modifier_mask(KC_LCTL);
            hid_report.report.keys[*index ++] = KC_T;
            break;
        case KC_RSET:
            HAL_NVIC_SystemReset();
            break;
    }
}


//test  =========```````````````` \\\\\\\\\`  -=-=-=-=-=-=-========
//===wwwwwwwwwwwwwww===qqqq
//========qqqqqweeeeq=========
//===============
//


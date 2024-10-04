#pragma once

#include "stdint.h"

#define MATRIX_ROWS 5
#define MATRIX_COLS 15

typedef enum {
    KC_NO = 0x00,  // 无键值
    KC_ROLL_OVER = 0x01, // 键值溢出
    KC_POST_FAIL = 0x02, // 电源错误
    KC_UNDEFINED = 0x03, // 未定义

    // 数字行
    KC_A = 0x04,
    KC_B = 0x05,
    KC_C = 0x06,
    KC_D = 0x07,
    KC_E = 0x08,
    KC_F = 0x09,
    KC_G = 0x0A,
    KC_H = 0x0B,
    KC_I = 0x0C,
    KC_J = 0x0D,
    KC_K = 0x0E,
    KC_L = 0x0F,
    KC_M = 0x10,
    KC_N = 0x11,
    KC_O = 0x12,
    KC_P = 0x13,
    KC_Q = 0x14,
    KC_R = 0x15,
    KC_S = 0x16,
    KC_T = 0x17,
    KC_U = 0x18,
    KC_V = 0x19,
    KC_W = 0x1A,
    KC_X = 0x1B,
    KC_Y = 0x1C,
    KC_Z = 0x1D,

    KC_1 = 0x1E,
    KC_2 = 0x1F,
    KC_3 = 0x20,
    KC_4 = 0x21,
    KC_5 = 0x22,
    KC_6 = 0x23,
    KC_7 = 0x24,
    KC_8 = 0x25,
    KC_9 = 0x26,
    KC_0 = 0x27,

    KC_ENTER = 0x28,
    KC_ESC = 0x29,
    KC_BSPC = 0x2A,
    KC_TAB = 0x2B,
    KC_SPC = 0x2C,

    // 符号键
    KC_MINS = 0x2D,  // 减号 "-"
    KC_EQL  = 0x2E,  // 等号 "="
    KC_LBRC = 0x2F,  // 左方括号 "["
    KC_RBRC = 0x30,  // 右方括号 "]"
    KC_BSLS = 0x31,  // 反斜杠 "\"
    KC_SCLN = 0x33,  // 分号 ";"
    KC_QUOT = 0x34,  // 单引号 "'"
    KC_GRV  = 0x35,  // 反引号 "`"
    KC_COMM = 0x36,  // 逗号 ","
    KC_DOT  = 0x37,  // 句号 "."
    KC_SLSH = 0x38,  // 斜杠 "/"

    KC_CAPS = 0x39,
    KC_F1 = 0x3A,
    KC_F2 = 0x3B,
    KC_F3 = 0x3C,
    KC_F4 = 0x3D,
    KC_F5 = 0x3E,
    KC_F6 = 0x3F,
    KC_F7 = 0x40,
    KC_F8 = 0x41,
    KC_F9 = 0x42,
    KC_F10 = 0x43,
    KC_F11 = 0x44,
    KC_F12 = 0x45,

    KC_DEL = 0x4C,  // 删除键
    KC_INS = 0x49,  // 插入键

    KC_UP = 0x52,   // 方向键 上
    KC_DOWN = 0x51, // 方向键 下
    KC_LEFT = 0x50, // 方向键 左
    KC_RGHT = 0x4F, // 方向键 右

    // 其他自定义宏
    // MO_1 = 0xFF,    // 切换至层1的功能键
    // KC_MYCM = 0xFD, // 自定义键: 我的电脑快捷键
    // KC_COPY = 0xFE, // 自定义键: 复制
    // KC_PSTE = 0xFC,  // 自定义键: 粘贴

    KC_LCTL = 0x70, // 左Ctrl
    KC_LSFT = 0x71, // 左Shift
    KC_LALT = 0x72, // 左Alt
    KC_LGUI = 0x73, // 左GUI (Windows键)
    KC_RCTL = 0x74, // 右Ctrl
    KC_RSFT = 0x75, // 右Shift
    KC_RALT = 0x76, // 右Alt
    KC_RGUI = 0x77  // 右GUI (Windows键)
} keycodes_e;

// 修饰键的位定义
// #define KC_LCTL    (1 << 0)  // 左Ctrl
// #define KC_LSFT    (1 << 1)  // 左Shift
// #define KC_LALT    (1 << 2)  // 左Alt
// #define KC_LGUI    (1 << 3)  // 左GUI (Windows键)
// #define KC_RCTL    (1 << 4)  // 右Ctrl
// #define KC_RSFT    (1 << 5)  // 右Shift
// #define KC_RALT    (1 << 6)  // 右Alt
// #define KC_RGUI    (1 << 7)  // 右GUI (Windows键)

typedef enum
{
    RGB_TOG = 1,
    RGB_MOD,
    RGB_RI,
    RGB_GI,
    RGB_BI,
    RGB_RD,
    RGB_GD,
    RGB_BD,
    RGB_LI,
    RGB_LD,
    KC_RSET
}rgb_key_e;

// 定义联合体来存储HID按键报告
typedef union {
    struct {
        uint8_t modifier;  // 修饰键字节
        uint8_t reserved;  // 保留字节，总是0
        uint16_t keys[6];   // 同时按下的6个普通按键
    } report;
    uint8_t buffer[8];  // 字节数组，用于发送
} HID_Report_Union_TypeDef;

class KeyScan
{
public:
    KeyScan();
    void operator()();

private:
    bool key_is_modifier(uint8_t key);
    uint8_t get_modifier_mask(uint8_t key);
    void process_and_send_keys();

    uint8_t key_buff[14] = {0};
    uint8_t key_count = 0;
    HID_Report_Union_TypeDef hid_report; // 按键缓冲区,最多一次发6个普通按键
};



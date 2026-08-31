#ifndef KEY_H_
#define KEY_H_

#include "ti_msp_dl_config.h"

// 按键状态枚举
typedef enum {
    KEY_STATE_RELEASED,     // 按键释放
    KEY_STATE_DEBOUNCE,     // 消抖中
    KEY_STATE_PRESSED,      // 已按下（未达长按阈值）
    KEY_STATE_LONG_HOLD     // 已达到长按阈值（等待释放）
} Key_State;

// 按键处理结构体
typedef struct {
    uint32_t pin;                   // 引脚编号
    Key_State state;                // 当前状态
    uint16_t press_duration;        // 持续按下计数
    uint16_t long_press_threshold;  // 长按阈值（单位：扫描周期）
    uint8_t short_press_flag;       // 短按标志（需手动清除）
    uint8_t long_press_flag;        // 长按标志（需手动清除）
} Key_Handle;

// 初始化宏（单位：毫秒）
#define KEY_INIT(pin_val, long_ms) \
    {pin_val, KEY_STATE_RELEASED, 0, (uint16_t)((long_ms)/80), 0, 0}

// 函数声明
void Key_Scan(Key_Handle *key);
void Key_ClearFlags(Key_Handle *key);


extern Key_Handle my_key_1;

#endif /* KEY_H_ */
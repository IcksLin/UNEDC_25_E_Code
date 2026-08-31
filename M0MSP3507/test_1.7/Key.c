#include "Key.h"

#define DEBOUNCE_TICKS 1    // 消抖时间=1*80ms=160ms（可根据实际调整）
#define KEY_PRESSED_LEVEL 0  // 假设低电平为按下

Key_Handle my_key_1 = KEY_INIT(Key_Key_1_PIN,52);
Key_Handle my_key_2;

void Key_Scan(Key_Handle *key) {
    uint8_t current_state = (DL_GPIO_readPins(Key_PORT, key->pin) == KEY_PRESSED_LEVEL);
    
    switch(key->state) {
        // 状态1：按键释放
        case KEY_STATE_RELEASED:
            if(current_state) {
                key->state = KEY_STATE_DEBOUNCE;
                key->press_duration = 0;
            }
            break;
            
        // 状态2：消抖检测
        case KEY_STATE_DEBOUNCE:
            if(current_state) {
                if(++key->press_duration >= DEBOUNCE_TICKS) {
                    key->state = KEY_STATE_PRESSED;
                    key->press_duration = 0; // 重置计时
                }
            } else {
                key->state = KEY_STATE_RELEASED;
            }
            break;
            
        // 状态3：已按下（未达长按阈值）
        case KEY_STATE_PRESSED:
            if(!current_state) {
                key->short_press_flag = 1;  // 标记短按
                key->state = KEY_STATE_RELEASED;
            } 
            else if(++key->press_duration >= key->long_press_threshold) {
                key->state = KEY_STATE_LONG_HOLD; // 进入长按等待状态
            }
            break;
            
        // 状态4：长按等待释放
        case KEY_STATE_LONG_HOLD:
            if(!current_state) {
                key->long_press_flag = 1;  // ▲▲▲ 松开时才标记长按 ▲▲▲
                key->state = KEY_STATE_RELEASED;
            }
            break;
            
        default:
            key->state = KEY_STATE_RELEASED;
            break;
    }
}

// 清除所有标志位
void Key_ClearFlags(Key_Handle *key) {
    key->short_press_flag = 0;
    key->long_press_flag = 0;
}

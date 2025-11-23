#ifndef MY_INTERRUPT_H
#define MY_INTERRUPT_H

#include "main.h"
#include "OLED.h"
#include "fuzzypid.h"
#include "packet_parser.h"
#include "No_MCU_Gray_Sensor.h"

/*******按键状态变量定义(start)********/
#define KEY_NUM 4
#define KEY_DEBOUNCE_TIME 1 // 10ms消抖(10ms中断中)

// 按键引脚定义（根据实际修改）
#define KEY0_GPIO_Port GPIOF
#define KEY0_Pin GPIO_PIN_2
#define KEY1_GPIO_Port GPIOF
#define KEY1_Pin GPIO_PIN_3
#define KEY2_GPIO_Port GPIOF
#define KEY2_Pin GPIO_PIN_4
#define KEY3_GPIO_Port GPIOF
#define KEY3_Pin GPIO_PIN_5

typedef enum {
    KEY_RELEASED = 0,
    KEY_DEBOUNCE,
    KEY_PRESSED
} KeyState;

typedef struct {
    KeyState state;
    uint16_t counter;
    uint8_t  short_press_flag;
} KeyInfo;

extern KeyInfo keys[KEY_NUM];
extern int x_move,y_move;
extern uint16_t x_angle;
extern uint16_t y_angle;

//灰度传感器数据
extern float queueData[CHANNEL_NUMBER];
//超声波
extern float distance_cm;

/********按键状态变量定义(end)*******/
/*********云台按键控制变量(start)****/

/*********云台按键控制变量(end)******/






#endif /* MY_INTERRUPT_H */

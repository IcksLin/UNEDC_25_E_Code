#ifndef MY_INTERRUPT_H
#define MY_INTERRUPT_H

#include "main.h"
#include "OLED.h"
#include "fuzzypid.h"
#include "IncrementalPID.h"
#include "packet_parser.h"
#include "menu.h"
/*******按键状态变量定义(start)********/
#define KEY_NUM 4
#define KEY_DEBOUNCE_TIME 1 // 10ms消抖(10ms中断中)


extern int x_move,y_move;
extern uint16_t x_angle;
extern uint16_t y_angle;
/********按键状态变量定义(end)*******/
/*********云台按键控制变量(start)****/

/*********云台按键控制变量(end)******/






#endif /* MY_INTERRUPT_H */

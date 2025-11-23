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
/************************云台按键控制变量(start)*********************************/
// 数值限制宏
#define constrain(value, min, max) ((value) < (min) ? (min) : ((value) > (max) ? (max) : (value)))

// 系统参数定义（根据实际调整）
#define PIXEL_DEADZONE       5       // 像素死区阈值
#define PIXEL_TO_STEP_RATIO  0.5f    // 像素到步数的转换系数
#define MAX_PIXEL_DIFF       400     // 最大有效像素偏差
#define MIN_SPEED           2000      // 最小速度(Hz)
#define MAX_STEPS           500000     // 单次最大步数
// 步进电机控制结构体
typedef struct {
    // 运动控制参数
    volatile int32_t stepper_index;    // 剩余步数（正数表示CW，负数表示CCW）
    volatile uint32_t current_speed;   // 当前速度（Hz，脉冲频率）
    uint32_t max_speed;               // 最大速度（Hz）
    uint32_t acceleration;            // 加速度（Hz/s）
    
    // 硬件相关参数
    TIM_HandleTypeDef *htim;          // 关联的定时器
    uint32_t tim_channel;             // 定时器通道
    GPIO_TypeDef *dir_port;           // 方向端口
    uint16_t dir_pin;                 // 方向引脚
    uint8_t dir_cw_level;             // CW方向电平
    
    // 状态标志
    volatile uint8_t is_moving;       // 运动状态标志
    uint32_t last_speed_update;       // 最后速度更新时间戳
} StepperMotor;

void Stepper_Control(StepperMotor *motor);
void Stepper_Init(StepperMotor *motor, TIM_HandleTypeDef *htim, uint32_t tim_channel,
                GPIO_TypeDef *dir_port, uint16_t dir_pin, uint8_t dir_cw_level);
void CalculateStepperSpeed(StepperMotor *motor, int32_t pixel_diff);

extern StepperMotor StepperMotor_x;
extern StepperMotor StepperMotor_y;

/*************************************云台按键控制变量(end)****************************************/






#endif /* MY_INTERRUPT_H */

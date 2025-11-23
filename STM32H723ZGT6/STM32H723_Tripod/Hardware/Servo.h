// servo.h
#ifndef __SERVO_H__
#define __SERVO_H__

#include "main.h"
#include <string.h>
#include <stdbool.h>

// 舵机参数（MG996R）
#define SERVO_MIN_PULSE     500     // 0.5ms 脉冲宽度（0°）
#define SERVO_MAX_PULSE     2500    // 2.5ms 脉冲宽度（180°）
#define SERVO_RANGE         (SERVO_MAX_PULSE - SERVO_MIN_PULSE)

typedef struct{
    float x;
    float y;
} Servo_Control;

extern Servo_Control Onto;
extern uint16_t onto_duty;

// 初始化舵机控制
void Servo_Init(TIM_HandleTypeDef *htim, uint32_t channel1, uint32_t channel2);

// --- 独立控制函数 ---
// 基础动态角度控制（0~1000 → 0°~180°）
void Servo1_SetAngle_Dynamic(uint16_t value);
void Servo2_SetAngle_Dynamic(uint16_t value);

// 相对角度控制（基于初始值，-120~120）
void Servo1_SetAngle_Relative(int16_t angle_offset);
void Servo2_SetAngle_Relative(int16_t angle_offset);

// 对称动态角度控制（基于初始值，-x~x）
void Servo1_SetAngle_Symmetric(int16_t value, int16_t range);
void Servo2_SetAngle_Symmetric(int16_t value, int16_t range);

// 设置初始值（可选）
void Servo1_SetInitialPulse(uint16_t pulse);
void Servo2_SetInitialPulse(uint16_t pulse);

#endif /* __SERVO_H__ */
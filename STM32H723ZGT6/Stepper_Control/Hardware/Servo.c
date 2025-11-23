// #include "servo.h"
// #include <stdlib.h>
// static TIM_HandleTypeDef *s_htim;
// static uint32_t s_channel1, s_channel2;
// static uint16_t s_initial_pulse1 = 1500; // 舵机1初始脉冲（中位1.5ms）
// static uint16_t s_initial_pulse2 = 1500; // 舵机2初始脉冲

// Servo_Control Onto;


// // 初始化舵机控制
// void Servo_Init(TIM_HandleTypeDef *htim, uint32_t channel1, uint32_t channel2) {
//     s_htim = htim;
//     s_channel1 = channel1;
//     s_channel2 = channel2;
//     Onto.x = 1500;
//     Onto.y = 1500;
// }

// // --- 舵机1控制函数 ---
// void Servo1_SetAngle_Dynamic(uint16_t value) {
//     value = (value > 1000) ? 1000 : value;
//     uint16_t pulse = SERVO_MIN_PULSE + (value * SERVO_RANGE) / 1000;
//     __HAL_TIM_SET_COMPARE(s_htim, s_channel1, pulse);
// }

// void Servo1_SetAngle_Relative(int16_t angle_offset) {
//     angle_offset = (angle_offset < -120) ? -120 : (angle_offset > 120) ? 120 : angle_offset;
//     uint16_t pulse = s_initial_pulse1 + (angle_offset * 11); // 1°≈11.11计数
//     pulse = (pulse < SERVO_MIN_PULSE) ? SERVO_MIN_PULSE : 
//             (pulse > SERVO_MAX_PULSE) ? SERVO_MAX_PULSE : pulse;
//     __HAL_TIM_SET_COMPARE(s_htim, s_channel1, pulse);
// }

// void Servo1_SetAngle_Symmetric(int16_t value, int16_t range) {
//     value = (value < -range) ? -range : (value > range) ? range : value;
//     uint16_t delta = (SERVO_RANGE * abs(value)) / (2 * range);
//     uint16_t pulse = (value >= 0) ? s_initial_pulse1 + delta : s_initial_pulse1 - delta;
//     pulse = (pulse < SERVO_MIN_PULSE) ? SERVO_MIN_PULSE : 
//             (pulse > SERVO_MAX_PULSE) ? SERVO_MAX_PULSE : pulse;
//     __HAL_TIM_SET_COMPARE(s_htim, s_channel1, pulse);
// }

// void Servo1_SetInitialPulse(uint16_t pulse) {
//     s_initial_pulse1 = pulse;
// }

// // --- 舵机2控制函数（逻辑相同）---
// void Servo2_SetAngle_Dynamic(uint16_t value) {
//     value = (value > 1000) ? 1000 : value;
//     uint16_t pulse = SERVO_MIN_PULSE + (value * SERVO_RANGE) / 1000;
//     __HAL_TIM_SET_COMPARE(s_htim, s_channel2, pulse);
// }

// void Servo2_SetAngle_Relative(int16_t angle_offset) {
//     angle_offset = (angle_offset < -120) ? -120 : (angle_offset > 120) ? 120 : angle_offset;
//     uint16_t pulse = s_initial_pulse2 + (angle_offset * 11);
//     pulse = (pulse < SERVO_MIN_PULSE) ? SERVO_MIN_PULSE : 
//             (pulse > SERVO_MAX_PULSE) ? SERVO_MAX_PULSE : pulse;
//     __HAL_TIM_SET_COMPARE(s_htim, s_channel2, pulse);
// }

// void Servo2_SetAngle_Symmetric(int16_t value, int16_t range) {
//     value = (value < -range) ? -range : (value > range) ? range : value;
//     uint16_t delta = (SERVO_RANGE * abs(value)) / (2 * range);
//     uint16_t pulse = (value >= 0) ? s_initial_pulse2 + delta : s_initial_pulse2 - delta;
//     pulse = (pulse < SERVO_MIN_PULSE) ? SERVO_MIN_PULSE : 
//             (pulse > SERVO_MAX_PULSE) ? SERVO_MAX_PULSE : pulse;
//     __HAL_TIM_SET_COMPARE(s_htim, s_channel2, pulse);
// }

// void Servo2_SetInitialPulse(uint16_t pulse) {
//     s_initial_pulse2 = pulse;
// }
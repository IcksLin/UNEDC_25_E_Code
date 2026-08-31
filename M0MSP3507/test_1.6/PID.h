#ifndef PID_H
#define PID_H

#include "ti_msp_dl_config.h"
#include "stdbool.h"

/*
 * PID控制器头文件（增强版）
 * 在原有基础上扩展了主流高级功能，包括积分分离、反向控制、微分低通滤波、使能开关、先行项等。
 * 所有变量名尽量保持原有风格，保证兼容性和耦合性。
 * — Copilot 中文注释
 */

// 常用宏定义
#define LIMIT(val, min, max) (((val) < (min)) ? (min) : ((val) > (max)) ? (max) : (val))
#define ABS(x) ((x) > 0 ? (x) : -(x))

typedef struct {
    float Kp, Ki, Kd;
    float Ti;                 // 积分时间常数
    float integral;
    float prev_error, last_error, before_last_error;
    float error_filter;       // 死区阈值
    float delta_error;
    float output_max, output_min;
    float output;
    float integral_max, integral_min;

    // ---- 扩展功能 ----
    float derivative_filter;  // 微分低通滤波系数（0~1，0最大平滑）
    float derivative_last;    // 上次微分项（用于滤波）
    float feedforward;        // 先行项
    bool  enable_feedforward; // 先行项开关
    bool  reverse_action;     // 反向控制开关
    bool  enable;             // PID使能开关
    bool  anti_windup;        // 积分分离/抗积分饱和
    float windup_zone;        // 积分分离死区
} PID;

// 全局PID对象
extern PID pid_x;
extern PID pid_y;
extern PID pid_r;
extern PID pid_l;
extern PID track;
// 基础接口
void PID_Init(PID* pid,
              float Kp, float Ti, float Kd,
              float error_filter,
              float output_max,
              float output_min,
              float integral_max,
              float integral_min);

void PID_SetTi(PID* pid, float Ti);
float PID_Control(PID* pid, float target, float current_value);

void PID_Reset(PID **pid, const uint8_t len);
void PID_InitAll(void);
void PID_SetIndex(PID* pid, float kp, float kd, float ki_change, float dt);

// ---- 新增高级接口 ----
void PID_SetEnable(PID* pid, bool enable);                     // 使能/禁用PID控制器
void PID_SetReverse(PID* pid, bool reverse_action);            // 设置反向控制
void PID_SetFeedforward(PID* pid, float ff, bool enable_ff);   // 设置先行项及开关
void PID_SetDerivativeFilter(PID* pid, float filter_coeff);    // 设置微分低通滤波
void PID_SetAntiWindup(PID* pid, bool enable, float windup_zone); // 积分分离死区

#endif
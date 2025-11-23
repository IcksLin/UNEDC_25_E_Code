#include "PID.h"
#include <stdio.h>

// 全局PID对象定义
PID pid_x;
PID pid_y;

// car pid
PID pid_r;
PID pid_l;

// PID参数初始化
void PID_Init(PID *pid,
              float Kp, float Ti, float Kd,
              float error_filter,
              float output_max,
              float output_min,
              float integral_max,
              float integral_min)
{
    pid->Kp = Kp;
    pid->Ti = Ti;
    pid->Ki = Kp * Ti * 2.56f; // 积分增益，可根据实际调整
    pid->Kd = Kd;
    pid->integral = 0.0f;
    pid->prev_error = 0.0f;
    pid->last_error = 0.0f;
    pid->before_last_error = 0.0f;
    pid->error_filter = error_filter;
    pid->delta_error = 0.0f;
    pid->output_max = output_max;
    pid->output_min = output_min;
    pid->output = 0.0f;
    pid->integral_max = integral_max;
    pid->integral_min = integral_min;

    // 扩展参数初始化
    pid->derivative_filter = 1.0f; // 默认无滤波
    pid->derivative_last = 0.0f;
    pid->feedforward = 0.0f;
    pid->enable_feedforward = false;
    pid->reverse_action = false;
    pid->enable = true;
    pid->anti_windup = false;
    pid->windup_zone = 0.0f;
}

// 设置积分时间常数
void PID_SetTi(PID *pid, float Ti)
{
    pid->Ti = LIMIT(Ti, 0.01f, 100.0f); // 防止Ti为0
    pid->Ki = pid->Kp / pid->Ti;
}

// 使能/禁用PID
void PID_SetEnable(PID *pid, bool enable)
{
    pid->enable = enable;
    if (!enable)
    {
        pid->integral = 0.0f;
        pid->output = 0.0f;
    }
}

// 反向控制
void PID_SetReverse(PID *pid, bool reverse_action)
{
    pid->reverse_action = reverse_action;
}

// 设定先行项及开关
void PID_SetFeedforward(PID *pid, float ff, bool enable_ff)
{
    pid->feedforward = ff;
    pid->enable_feedforward = enable_ff;
}

// 设置微分低通滤波系数
void PID_SetDerivativeFilter(PID *pid, float filter_coeff)
{
    if (filter_coeff < 0.0f)
        filter_coeff = 0.0f;
    if (filter_coeff > 1.0f)
        filter_coeff = 1.0f;
    pid->derivative_filter = filter_coeff;
}

// 设置积分分离（抗积分饱和）及死区
void PID_SetAntiWindup(PID *pid, bool enable, float windup_zone)
{
    pid->anti_windup = enable;
    pid->windup_zone = windup_zone;
}

// 传统PID控制器（增强版）
float PID_Control(PID *pid, float target, float current_value)
{
    if (!pid->enable)
        return 0.0f;

    float new_error;
    new_error = target - current_value;
    if (pid->reverse_action)
        new_error = -new_error;

    pid->before_last_error = pid->last_error;
    pid->last_error = pid->prev_error;
    pid->prev_error = new_error;
    pid->delta_error = pid->prev_error - pid->last_error;

    // 死区处理
    if (ABS(pid->prev_error) <= pid->error_filter)
    {
        pid->prev_error = 0.0f;
    }

    // 积分分离/抗积分饱和
    if (pid->anti_windup && (ABS(pid->prev_error) > pid->windup_zone))
    {
        // 误差大于分离死区时不积分
    }
    else
    {
        pid->integral += pid->prev_error;
        if (pid->integral > pid->integral_max)
            pid->integral = pid->integral_max;
        if (pid->integral < pid->integral_min)
            pid->integral = pid->integral_min;
    }

    // 微分项低通滤波（抑制高频噪声）
    float derivative_raw = pid->prev_error + pid->before_last_error - 2 * pid->last_error;
    float derivative = pid->derivative_filter * derivative_raw + (1.0f - pid->derivative_filter) * pid->derivative_last;
    pid->derivative_last = derivative;

    // 增量式PID公式
    pid->output = pid->Kp * pid->prev_error + pid->Ki * pid->integral + pid->Kd * derivative;

    // 先行项
    if (pid->enable_feedforward)
        pid->output += pid->feedforward;

    pid->output = LIMIT(pid->output, pid->output_min, pid->output_max);

    return pid->output;
}

// 重置PID控制器
void PID_Reset(PID **pid, const uint8_t len)
{
    uint8_t i;
    for (i = 0; i < len; i++)
    {
        pid[i]->integral = 0.0f;
        pid[i]->prev_error = 0.0f;
        pid[i]->last_error = 0.0f;
        pid[i]->before_last_error = 0.0f;
        pid[i]->delta_error = 0.0f;
        pid[i]->output = 0.0f;
        pid[i]->derivative_last = 0.0f;
    }
}

// PID参数初始化（可用于速度环等应用）
void PID_InitAll(void)
{
    const float dt = 0.01f;
    PID_Init(&pid_x, 0.730f, dt, 0.02,
             5.0f,
             300.0f, -300.0f,
             20000.0f, -20000.0f);

    PID_Init(&pid_y, 0.730f, dt, 0.02,
             5.0f,
             1000.0f, -1000.0f,
             20000.0f, -20000.0f);
    PID_Init(&pid_r, 2.49f, dt, 0.27,
             10.0f,
             1000.0f, -1000.0f,
             9102.0f, -9102.0f);

    PID_Init(&pid_l, 2.49f, dt, 0.27,
             10.0f,
             1000.0f, -1000.0f,
             9102.0f, -9102.0f);
    // 可选：设置高级参数
    PID_SetDerivativeFilter(&pid_x, 0.7f);   // 微分滤波
    PID_SetAntiWindup(&pid_x, true, 10.0f);  // 积分分离
    PID_SetFeedforward(&pid_x, 30.0f, true); // 先行项
    PID_SetReverse(&pid_x, false);           // 正向控制

    PID_SetDerivativeFilter(&pid_y, 0.7f);
    PID_SetAntiWindup(&pid_y, true, 10.0f);
    PID_SetFeedforward(&pid_y, 30.0f, true);
    PID_SetReverse(&pid_y, false);
}

// 设置PID参数
void PID_SetIndex(PID *pid, float kp, float kd, float ki_change, float dt)
{
    pid->Kp = kp;
    pid->Kd = kd;
    pid->Ki = ki_change;
}
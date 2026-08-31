#include "My_Interrupt.h"
#include "stdlib.h"


/********************************stepper motor(start)********************************/
StepperMotor StepperMotor_x;
StepperMotor StepperMotor_y;

/**
  * @brief  步进电机定时中断处理函数
  * @param  motor: 步进电机结构体指针
  * @retval None
  */
void Stepper_Control(StepperMotor *motor) {
    // 只在有步数需要执行时处理
    if(motor->stepper_index != 0) {
        // 设置方向
        HAL_GPIO_WritePin(motor->dir_port, motor->dir_pin, 
            (motor->stepper_index > 0) ? motor->dir_cw_level : !motor->dir_cw_level);
        
        // 生成脉冲（通过PWM模式）
        __HAL_TIM_SET_COMPARE(motor->htim, motor->tim_channel, 
            SystemCoreClock / motor->current_speed / 2); // 50%占空比
        
        // 更新步数（向零逼近）
        motor->stepper_index += (motor->stepper_index > 0) ? -1 : 1;
        
        // 更新运动状态
        motor->is_moving = (motor->stepper_index != 0);
    } else {
        // 停止PWM输出
        __HAL_TIM_SET_COMPARE(motor->htim, motor->tim_channel, 0);
        motor->is_moving = 0;
    }
    
}

/**
  * @brief  根据视觉反馈计算步进电机速度
  * @param  motor: 步进电机结构体指针
  * @param  pixel_diff: 视觉模块检测到的像素偏差
  * @retval None
  */
void CalculateStepperSpeed(StepperMotor *motor, int32_t pixel_diff) {
    // 死区过滤（忽略小偏差）
    if(abs(pixel_diff) < PIXEL_DEADZONE) {
        motor->stepper_index = 0;
        return;
    }
    
    // 计算目标步数（比例控制）
    int32_t target_steps = (int32_t)(pixel_diff * PIXEL_TO_STEP_RATIO);
    
    // 限制最大步数（防止过冲）
    target_steps = constrain(target_steps, -MAX_STEPS, MAX_STEPS);
    
    // 更新步数（保留符号）
    motor->stepper_index = target_steps;
    
    // 计算速度（基于偏差的绝对值）
    float speed_ratio = fminf(1.0f, (float)abs(pixel_diff) / MAX_PIXEL_DIFF);
    uint32_t target_speed = motor->max_speed * speed_ratio;
    
    // 应用速度限制
    motor->current_speed = constrain(target_speed, MIN_SPEED, motor->max_speed);
    
    // 更新定时器频率
    uint32_t period = (SystemCoreClock / motor->current_speed) - 1;
    __HAL_TIM_SET_AUTORELOAD(motor->htim, period);
    
    // 标记为运动中
    motor->is_moving = 1;
    
    // 重新使能定时器（确保运行）
    HAL_TIM_Base_Start_IT(motor->htim);
    HAL_TIM_PWM_Start(motor->htim, motor->tim_channel);
}

void Stepper_Init(StepperMotor *motor, TIM_HandleTypeDef *htim, uint32_t tim_channel,
                GPIO_TypeDef *dir_port, uint16_t dir_pin, uint8_t dir_cw_level) {
    // 初始化步进电机结构体
    motor->stepper_index = 0;
    motor->current_speed = 0;
    motor->max_speed = 500000; // 默认最大速度
    motor->acceleration = 100000; // 默认加速度
    motor->htim = htim;
    motor->tim_channel = tim_channel;
    motor->dir_port = dir_port;
    motor->dir_pin = dir_pin;
    motor->dir_cw_level = dir_cw_level;
    motor->is_moving = 0;
    motor->last_speed_update = HAL_GetTick();
}

/********************************stepper motor(end)********************************/




void HAL_TIM_PeriodElapsedCallback(TIM_HandleTypeDef *htim)
{
    if (htim->Instance == TIM6) //10ms
    {
        //按键扫描
        scan_keys();
    }
    if(htim->Instance == TIM24) 
    {
        Stepper_Control(&StepperMotor_x);
    }
    if(htim->Instance == TIM3)  
    {
        Stepper_Control(&StepperMotor_y);
    }
}











#include "Stepper.h"

/********************************stepper motor(start)********************************/
StepperMotor StepperMotor_x;
StepperMotor StepperMotor_y;

///**
//  * @brief  根据像素差计算动态加速度曲线
//  * @param  pixel_diff 当前像素偏差(绝对值)
//  * @retval 动态加速度缩放因子(0.1~2.0)
//  */
//float CalculateDynamicAccel(StepperMotor *motor, uint32_t pixel_diff) {
//    // 1. 进入2倍死区时极速减速（原0.1改为0.3保留一定速度）
//    if (pixel_diff <= motor->deadzone_2x) {
//        return 0.3f;  // 提升最低加速度，避免过度减速
//    }
//
//    // 2. 修改减速段为指数曲线（缩短减速时间）
//    else if (pixel_diff <= motor->decel_start_threshold) {
//        float normalized = (float)(pixel_diff - motor->deadzone_2x) /
//                         (motor->decel_start_threshold - motor->deadzone_2x);
//        return 0.3f + 0.7f * powf(normalized, 0.5f); // 指数增长(0.3~1.0)
//    }
//
//    // 3. 远距离激进加速（保持不变）
//    else {
//        float ratio = (float)(pixel_diff - motor->decel_start_threshold) /
//                     (MAX_PIXEL_DIFF - motor->decel_start_threshold);
//        return 1.0f + 1.0f * ratio; // 1.0~2.0线性增长
//    }
//}

/**
  * @brief  根据像素差计算激进加速度曲线
  * @param  motor: 步进电机结构体指针
  * @param  pixel_diff 当前像素偏差(绝对值)
  * @retval 动态加速度缩放因子(0.5~4.0)
  */
float CalculateDynamicAccel(StepperMotor *motor, uint32_t pixel_diff) {
    // 1. 死区内保持最低加速度
    if (pixel_diff <= motor->deadzone_2x) {
        return 0.5f;  // 保留一定基础加速度
    }

    // 2. 中距离区域：立方曲线快速上升
    else if (pixel_diff <= motor->decel_start_threshold) {
        float normalized = (float)(pixel_diff - motor->deadzone_2x) /
                         (motor->decel_start_threshold - motor->deadzone_2x);
        return 0.5f + 1.5f * powf(normalized, 3.0f); // 0.5~2.0立方增长
    }

    // 3. 远距离：超激进加速
    else {
        float ratio = (float)(pixel_diff - motor->decel_start_threshold) /
                     (MAX_PIXEL_DIFF - motor->decel_start_threshold);
        return 2.0f + 2.0f * powf(ratio, 0.5f); // 2.0~4.0平方根增长
    }
}

/**
  * @brief  激进版速度计算
  * @param  motor: 步进电机结构体指针
  * @param  pixel_diff: 视觉模块检测到的像素偏差
  * @retval None
  */
void CalculateStepperSpeed(StepperMotor *motor, int32_t pixel_diff) {
    uint32_t abs_diff = abs(pixel_diff);

    // 死区过滤（缩小死区范围）
    if(abs_diff < PIXEL_DEADZONE/2) {  // 死区减半
        motor->stepper_index = 0;
        return;
    }

    // 计算激进加速度
    motor->dynamic_accel_scale = CalculateDynamicAccel(motor, abs_diff);

    // 超激进步数计算（增加过冲系数）
    int32_t target_steps = (int32_t)(pixel_diff * PIXEL_TO_STEP_RATIO *
                                   motor->dynamic_accel_scale * 1.2f); // 增加20%过冲
    target_steps = constrain(target_steps, -MAX_STEPS*2, MAX_STEPS*2); // 放宽步数限制
    motor->stepper_index = target_steps;

    // 速度计算（指数响应）
    float speed_ratio = fminf(1.0f, (float)abs_diff / (MAX_PIXEL_DIFF*0.7f)); // 提前达到最大速度
    uint32_t target_speed = MIN_SPEED + (motor->max_speed*1.5f - MIN_SPEED) *
                          powf(speed_ratio, 1.5f - motor->dynamic_accel_scale/3.0f);

    motor->current_speed = constrain(target_speed, MIN_SPEED, motor->max_speed*1.5f);

    // 激进定时器设置（允许更高频率）
    uint32_t period = (SystemCoreClock / motor->current_speed) - 1;
    __HAL_TIM_SET_AUTORELOAD(motor->htim, period);

    motor->is_moving = 1;
    HAL_TIM_Base_Start_IT(motor->htim);
    HAL_TIM_PWM_Start(motor->htim, motor->tim_channel);
}


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

///**
//  * @brief  根据视觉反馈计算步进电机速度
//  * @param  motor: 步进电机结构体指针
//  * @param  pixel_diff: 视觉模块检测到的像素偏差
//  * @retval None
//  */
//void CalculateStepperSpeed(StepperMotor *motor, int32_t pixel_diff) {
//    uint32_t abs_diff = abs(pixel_diff);
//
//    // 死区过滤
//    if(abs_diff < PIXEL_DEADZONE) {
//        motor->stepper_index = 0;
//        return;
//    }

//    // 计算动态加速度
//    motor->dynamic_accel_scale = CalculateDynamicAccel(motor, abs_diff);
//
//    // 计算目标步数（带动态缩放）
//    int32_t target_steps = (int32_t)(pixel_diff * PIXEL_TO_STEP_RATIO * motor->dynamic_accel_scale);
//    target_steps = constrain(target_steps, -MAX_STEPS, MAX_STEPS);
//    motor->stepper_index = target_steps;

//    // 速度计算（动态响应）
//    float speed_ratio = fminf(1.0f, (float)abs_diff / MAX_PIXEL_DIFF);
//    uint32_t target_speed = MIN_SPEED + (motor->max_speed - MIN_SPEED) *
//                           powf(speed_ratio, 2 - motor->dynamic_accel_scale); // 加速度影响速度曲线形状
//
//    motor->current_speed = constrain(target_speed, MIN_SPEED, motor->max_speed);
//
//    // 更新定时器
//    uint32_t period = (SystemCoreClock / motor->current_speed) - 1;
//    __HAL_TIM_SET_AUTORELOAD(motor->htim, period);
//
//    motor->is_moving = 1;
//    HAL_TIM_Base_Start_IT(motor->htim);
//    HAL_TIM_PWM_Start(motor->htim, motor->tim_channel);
//}

void Stepper_Init(StepperMotor *motor, TIM_HandleTypeDef *htim, uint32_t tim_channel,
                GPIO_TypeDef *dir_port, uint16_t dir_pin, uint8_t dir_cw_level, uint32_t max_speed) {
    // 初始化步进电机结构体
    motor->stepper_index = 0;
    motor->current_speed = 0;
    motor->max_speed = max_speed; // 使用传入的最大速度
    motor->htim = htim;
    motor->tim_channel = tim_channel;
    motor->dir_port = dir_port;
    motor->dir_pin = dir_pin;
    motor->dir_cw_level = dir_cw_level;
    motor->is_moving = 0;
    motor->last_speed_update = HAL_GetTick();
     motor->dynamic_accel_scale = 1.0f;
    motor->decel_start_threshold = MAX_PIXEL_DIFF * 0.3f; // 建议值
    motor->deadzone_2x = PIXEL_DEADZONE * 1.1;             // 2倍死区

 motor->dynamic_accel_scale = 1.5f;  // 默认更高的加速度
    motor->decel_start_threshold = MAX_PIXEL_DIFF * 0.2f; // 更早开始减速
    motor->deadzone_2x = PIXEL_DEADZONE * 1.05; // 更窄的缓冲死区

    // 提高最大速度限制（确保硬件支持）
    motor->max_speed = constrain(max_speed * 1.3f, 0, max_speed);
}

/********************************stepper motor(end)********************************/

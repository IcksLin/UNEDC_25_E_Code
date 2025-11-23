#include "My_Interrupt.h"

float Yaw_a;
float Last_Yaw;
float Current_Yaw;

void HAL_TIM_PeriodElapsedCallback(TIM_HandleTypeDef *htim)
{
    if (htim->Instance == TIM24)
    { // 50ms
        Current_Yaw = getYaw();
        Yaw_a = (Current_Yaw - Last_Yaw)/0.05f;
        Last_Yaw = Current_Yaw;
    }
    if (htim->Instance == TIM23)
    { // 10ms
        scan_keys();
    }
    if (htim->Instance == TIM15)
    {
        Stepper_Control(&StepperMotor_x);
    }
    if (htim->Instance == TIM3)
    {
        Stepper_Control(&StepperMotor_y);
    }
}

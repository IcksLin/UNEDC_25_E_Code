#include "My_Interrupt.h"

void HAL_TIM_PeriodElapsedCallback(TIM_HandleTypeDef *htim)
{
    if (htim->Instance == TIM24)
    { // 50ms
      // Motor_GetSpeed(&current_speed);
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

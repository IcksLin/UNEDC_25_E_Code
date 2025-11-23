#include "My_Interrupt.h"

//按键变量
KeyInfo keys[KEY_NUM];
const GPIO_TypeDef* KEY_PORT[KEY_NUM] = {KEY0_GPIO_Port, KEY1_GPIO_Port, KEY2_GPIO_Port, KEY3_GPIO_Port};
const uint16_t      KEY_PIN [KEY_NUM] = {KEY0_Pin,       KEY1_Pin,       KEY2_Pin,       KEY3_Pin      };

int x_move,y_move;

extern ADC_HandleTypeDef hadc1;
float queueData[CHANNEL_NUMBER];


uint16_t x_angle = 1520;
uint16_t y_angle = 1750;

//超声波模块
uint32_t start_time;
uint32_t end_time;
float distance_cm;
extern TIM_HandleTypeDef htim23;//服务计时的32位定时器

/*********按键函数(start)*********/
//按键的检测结果（即按键是否被短按）存储在 keys[i].short_press_flag 变量中,该标志置位需要手动清零
void scan_keys(void) {
    for (uint8_t i = 0; i < KEY_NUM; i++) {
        uint8_t pin_level = (HAL_GPIO_ReadPin((GPIO_TypeDef*)KEY_PORT[i], KEY_PIN[i]) == GPIO_PIN_SET) ? 1 : 0; // 按下为1

        switch (keys[i].state) {
        case KEY_RELEASED:
            if (pin_level) {
                keys[i].state = KEY_DEBOUNCE;
                keys[i].counter = 0;
            }
            break;
        case KEY_DEBOUNCE:
            if (pin_level) {
                if (++keys[i].counter >= KEY_DEBOUNCE_TIME) {
                    keys[i].state = KEY_PRESSED;
                }
            } else {
                keys[i].state = KEY_RELEASED;
            }
            break;
        case KEY_PRESSED:
            if (!pin_level) {
                keys[i].short_press_flag = 1; // 产生短按事件
                keys[i].state = KEY_RELEASED;
            }
            break;
        default:
            keys[i].state = KEY_RELEASED;
            break;
        }
    }
}

/*********按键函数(end)***********/

void HAL_TIM_PeriodElapsedCallback(TIM_HandleTypeDef *htim)
{
    if (htim->Instance == TIM6) //10ms
    {
        //按键扫描
        scan_keys();
        //八路灰度ADC采样
        GraySensor_Update(&hadc1);
    }
    if (htim->Instance == TIM7)//100ms
    {
        //超声波模块激发
        HAL_GPIO_WritePin(GPIOA,GPIO_PIN_10,GPIO_PIN_SET);
        Delay_us(12);
        HAL_GPIO_WritePin(GPIOA,GPIO_PIN_10,GPIO_PIN_RESET);
    }
}

void HAL_GPIO_EXTI_Callback(uint16_t GPIO_Pin) {
    if (GPIO_Pin == GPIO_PIN_9) {
       
        if (HAL_GPIO_ReadPin(GPIOA, GPIO_PIN_9) == GPIO_PIN_SET) {
            start_time = __HAL_TIM_GetCounter(&htim23);  // 上升沿记录开始时间
            // OLED_ShowString(5*16,3*16,"UP",OLED_8X16);
            // OLED_Update();
        } else {
            end_time = __HAL_TIM_GET_COUNTER(&htim23);   // 下降沿记录结束时间
            // OLED_ShowString(5*16,3*16,"DOWN",OLED_8X16);
            // OLED_Update();
            if (start_time>end_time)
            {
                distance_cm = (float)((4294967295 - start_time) + end_time + 1);
               
                /* code */
            }else{
                distance_cm = (float)(end_time - start_time);
            }
            
            distance_cm = distance_cm * 0.0343 / 2;  // 计算距离
        }
    }
}










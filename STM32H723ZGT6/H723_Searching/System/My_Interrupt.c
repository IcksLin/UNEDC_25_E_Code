#include "My_Interrupt.h"
int x_move,y_move;

uint16_t x_angle = 1500;
uint16_t y_angle = 1500;

/*********按键函数(end)***********/

void HAL_TIM_PeriodElapsedCallback(TIM_HandleTypeDef *htim)
{
    if (htim->Instance == TIM6) //10ms
    {
        //按键扫描
        scan_keys();
        
        if (blue_brick.x!=999&&blue_brick.y!=999)
        {
            /* code */
            copy_blue.x = blue_brick.x;
            copy_blue.y = blue_brick.y;
        }
        
        // if(green_point.x!=999&&green_point.y!=999&&copy_red.x!=999&&copy_red.y!=999){
        //         fuzzy_pid_control(&pid_x,copy_red.x,green_point.x);
        //         fuzzy_pid_control(&pid_y,copy_red.y,green_point.y);
                
        //     }else{
        //     pid_x.output = pid_y.output = 0;
        // }

        if(copy_blue.x!=999&&copy_blue.y!=999){
                fuzzy_pid_control(&pid_x,320,copy_blue.x);
                fuzzy_pid_control(&pid_y,240,copy_blue.y);
                
            }else{
                pid_y.output = pid_y.output = 0;
        }

       
    }
}











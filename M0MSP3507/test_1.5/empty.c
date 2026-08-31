/*
 * Copyright (c) 2021, Texas Instruments Incorporated
 * All rights reserved.
 *
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions
 * are met:
 *
 * *  Redistributions of source code must retain the above copyright
 *    notice, this list of conditions and the following disclaimer.
 *
 * *  Redistributions in binary form must reproduce the above copyright
 *    notice, this list of conditions and the following disclaimer in the
 *    documentation and/or other materials provided with the distribution.
 *
 * *  Neither the name of Texas Instruments Incorporated nor the names of
 *    its contributors may be used to endorse or promote products derived
 *    from this software without specific prior written permission.
 *
 * THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS"
 * AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO,
 * THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR
 * PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT OWNER OR
 * CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL,
 * EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO,
 * PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS;
 * OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY,
 * WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR
 * OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE,
 * EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
 */

#include "ti_msp_dl_config.h"
#include "OLED.h"
#include "Motor.h"
#include "gray_detection.h"
#include "PID.h"
#include "Key.h"

int main(void)
{
    SYSCFG_DL_init();
    //电机初始化
    Motor_Init();
    //编码器中断开启
    NVIC_EnableIRQ(Encoder_INT_IRQN);
    //圈数控制器初始化
    start_flag = 0;
    
    

    //采样中断开启0.05s,(在所有硬件资源初始化完成之后再初始化)
    NVIC_EnableIRQ(TIMER_0_INST_INT_IRQN);
    DL_TimerG_startCounter(TIMER_0_INST);
    //pid控制器初始化
    PID_Init(&pid_r,2.5,0.05,0.6,4,400,-400,875,-875);
    PID_Init(&pid_l,2.5,0.05,0.6,4,400,-400,875,-875);

    OLED_Init();
    uint32_t tick = 0;
    while (1) {
        tick++;
        OLED_Clear();
        // Set_Motor_Speed(0,0);
        
        set_speed.speed_L = pid_l.output;
        set_speed.speed_R = pid_r.output;

        // set_speed.speed_L = 50;
        // set_speed.speed_R = 50;

        Set_Motor_Speed(&set_speed);    //设置获取的速度
        Gray_ReadAll_Used();                   //读取使用的灰度通道
        Gray_MapToParabola();                  //灰度通道映射至偏移量

        //可视化部分
        // OLED_DisplayGrayValues();   //灰度通道可视化

        OLED_ShowNum(0, 0, turn_counts, 2, OLED_6X8);
        OLED_ShowString(0, 4*8, "distance:", OLED_6X8);
        OLED_ShowSignedNum(3*8, 4*8, current_distance.distance_of_car, 6, OLED_6X8);

        OLED_ShowString(0, 5*8, "L:", OLED_6X8);
        OLED_ShowString(0, 6*8, "R:", OLED_6X8);
        OLED_ShowSignedNum(3*8, 5*8, current_speed.speed_L, 3, OLED_6X8);
        OLED_ShowSignedNum(3*8, 6*8, current_speed.speed_R, 3, OLED_6X8);
        OLED_ShowSignedNum(16*5, 5*8, onto, 3, OLED_6X8);
        OLED_Update();
 
    }
}


//编码器中断
void GROUP1_IRQHandler(void)//外部中断
{
    if(DL_GPIO_getEnabledInterruptStatus(Encoder_PORT,Encoder_RIGHT_P_PIN))
    {
        if(DL_GPIO_readPins(Encoder_PORT,Encoder_RIGHT_D_PIN))
        {
            current_speed.pluse_num_left++;
        }
        else
        {
            current_speed.pluse_num_left--;
        }
        DL_GPIO_clearInterruptStatus(Encoder_PORT, Encoder_RIGHT_P_PIN);
    }
    if(DL_GPIO_getEnabledInterruptStatus(Encoder_PORT,Encoder_LEFT_P_PIN))
    {
        if(DL_GPIO_readPins(Encoder_PORT,Encoder_LEFT_D_PIN))
        {
            current_speed.pluse_num_right++;
        }
        else
        {
            current_speed.pluse_num_right--;
        }
        DL_GPIO_clearInterruptStatus(Encoder_PORT, Encoder_LEFT_P_PIN);
    }
    
}

void TIMER_0_INST_IRQHandler(void)//定时器中断
{
    //按键扫描
    Key_Scan(&my_key_1);

    Motor_GetSpeed(&current_speed);
   
    //循迹
    if(start_flag == 1){
        tracking_mode(&track);

        //若圈数达到，推出循迹
        if(current_distance.distance_of_car>ONE_TURE*turn_counts){
            start_flag = 0;
        }
    }
    else {
        PID_Control(&pid_r, 0, current_speed.speed_R);
        PID_Control(&pid_l, 0, current_speed.speed_L);
    
    }

    
    
    //短按设置圈数
   if(my_key_1.short_press_flag) {
        Key_ClearFlags(&my_key_1);
        turn_counts++;
        if(turn_counts>5){
            turn_counts=0;
        }
    }
    
    //长按启动循迹
    if(my_key_1.long_press_flag) {
        Key_ClearFlags(&my_key_1);  // ▲▲▲ 必须手动清除 ▲▲▲
        current_distance.distance_of_car = 0;
        current_distance.left = 0;
        current_distance.rigt = 0;
        start_flag = 1;

    }
    
    

   
}
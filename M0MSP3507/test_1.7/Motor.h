#ifndef __MOTOR_H_
#define __MOTOR_H_

#include "ti_msp_dl_config.h"
#include "PID.h"

#define ONE_TURE 12944    //一圈的脉冲计数，通过脉冲数来计算圈数
// int count_times;          //目前圈数
int start_flag;           //为1启动循迹  

typedef struct {
    int speed_R;
    int speed_L;

    int pluse_num_right;
    int pluse_num_left;
    
}Motor_Speed;

typedef struct{
    int32_t rigt;
    int32_t left;
    int32_t distance_of_car;
} Distance;


extern Motor_Speed current_speed;
extern Motor_Speed set_speed;     //用于记录设置速度的结构体
extern int onto;
extern int turn_counts;

extern Distance current_distance;


void Motor_Init(void);
void Set_Motor_Speed(Motor_Speed* speed);
void Motor_GetSpeed(Motor_Speed*Speed);
void tracking_mode(PID* tracking);


#endif

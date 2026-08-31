#ifndef __MOTOR_H_
#define __MOTOR_H_

#include "ti_msp_dl_config.h"
#include "PID.h"
typedef struct {
    int speed_R;
    int speed_L;

    int pluse_num_right;
    int pluse_num_left;
    
}Motor_Speed;

extern Motor_Speed current_speed;
extern Motor_Speed set_speed;     //用于记录设置速度的结构体
extern int onto;


void Motor_Init(void);
void Set_Motor_Speed(Motor_Speed* speed);
void Motor_GetSpeed(Motor_Speed*Speed);
void tracking_mode(PID* tracking);


#endif

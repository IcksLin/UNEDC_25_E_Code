#include "Motor.h"
#include "PID.h"
#include "gray_detection.h"
int onto;
int turn_counts = 0;

Motor_Speed current_speed; //用于记录当前速度的结构体
Motor_Speed set_speed;     //用于记录设置速度的结构体
Distance current_distance;

void Motor_Init(void){
    DL_TimerG_startCounter(MOTOR_R_INST);
    DL_TimerG_startCounter(MOTOR_L_INST);
    DL_TimerG_setCaptureCompareValue(MOTOR_R_INST,0,DL_TIMER_CC_0_INDEX);
    DL_TimerG_setCaptureCompareValue(MOTOR_R_INST,0,DL_TIMER_CC_1_INDEX);

    DL_TimerG_setCaptureCompareValue(MOTOR_L_INST,0,DL_TIMER_CC_0_INDEX);
    DL_TimerG_setCaptureCompareValue(MOTOR_L_INST,0,DL_TIMER_CC_1_INDEX);
    
}

void Set_Motor_Speed(Motor_Speed* speed){
    
    
    if(speed->speed_R >=0 ){
        DL_TimerG_setCaptureCompareValue(MOTOR_L_INST,speed->speed_R,DL_TIMER_CC_1_INDEX);
        DL_TimerG_setCaptureCompareValue(MOTOR_L_INST,0,DL_TIMER_CC_0_INDEX);
    }
    else{
        DL_TimerG_setCaptureCompareValue(MOTOR_L_INST,0,DL_TIMER_CC_1_INDEX);
        DL_TimerG_setCaptureCompareValue(MOTOR_L_INST,-speed->speed_R,DL_TIMER_CC_0_INDEX);
    }

    if(speed->speed_L<=0){
        DL_TimerG_setCaptureCompareValue(MOTOR_R_INST,0,DL_TIMER_CC_0_INDEX);
        DL_TimerG_setCaptureCompareValue(MOTOR_R_INST,-speed->speed_L,DL_TIMER_CC_1_INDEX);
    }
    else{
        DL_TimerG_setCaptureCompareValue(MOTOR_R_INST,0,DL_TIMER_CC_1_INDEX);
        DL_TimerG_setCaptureCompareValue(MOTOR_R_INST,speed->speed_L,DL_TIMER_CC_0_INDEX);
    }

}

void Motor_GetSpeed(Motor_Speed*Speed){
    Speed->speed_L = -current_speed.pluse_num_left;
    current_distance.left+=(-current_speed.pluse_num_left);

    current_speed.pluse_num_left = 0;

    Speed->speed_R = current_speed.pluse_num_right;
    current_distance.rigt+=current_speed.pluse_num_right;

    current_speed.pluse_num_right = 0;

    current_distance.distance_of_car = (current_distance.left+current_distance.rigt)/2;

}

void tracking_mode(PID* tracking){


    onto = Gray_CalculateError();
    if(ABS(onto)<=44){
    PID_Control(&pid_r, 80+0.7*onto, current_speed.speed_R);
    PID_Control(&pid_l, 80-0.7*onto, current_speed.speed_L);
    PID_Control(tracking , 0, -onto);

    }else {
        PID_Control(&pid_r, 50, current_speed.speed_R);
        PID_Control(&pid_l, 0, current_speed.speed_L);
        PID_Control(tracking , 0, -onto);

    
    }

   



}


#ifndef __NO_MCU_GRAY_SENSOR_
#define __NO_MCU_GRAY_SENSOR_
#include "main.h"

#define CHANNEL_NUMBER 8
// 3个GPIO引脚用于通道选择编码
#define GRAY_SENSOR_A0_PIN    GPIO_PIN_1
#define GRAY_SENSOR_A0_PORT   GPIOB
#define GRAY_SENSOR_A1_PIN    GPIO_PIN_0
#define GRAY_SENSOR_A1_PORT   GPIOB
#define GRAY_SENSOR_A2_PIN    GPIO_PIN_0
#define GRAY_SENSOR_A2_PORT   GPIOC

// 灰度传感器模拟输出引脚（接ADC）
#define GRAY_SENSOR_OUT_PIN   GPIO_PIN_1
#define GRAY_SENSOR_OUT_PORT  GPIOC

#define QUEUE_DEPTH  16  // 队列深度（可调整）

// 滤波类型枚举
typedef enum {
    FILTER_NONE = 0,    // 无滤波
    FILTER_MOVING_AVG,  // 移动平均
    FILTER_MEDIAN       // 中值滤波
} GrayFilterType;

// 队列结构体
typedef struct {
    float data[CHANNEL_NUMBER][QUEUE_DEPTH]; 
    float filtered[CHANNEL_NUMBER];  // 滤波后数据
    uint16_t front;                 
    uint16_t rear;                  
    uint16_t count;
    GrayFilterType filter_type;      // 当前滤波模式
    uint8_t filter_window;           // 滤波窗口大小(3/5/7)
} GraySensorQueue;

extern GraySensorQueue grayQueue;

void Init_ADC(ADC_HandleTypeDef*hadc);
void GraySensor_Init(void);
void GraySensor_Update(ADC_HandleTypeDef* hadc);
void GraySensor_SetFilter(GrayFilterType type, uint8_t window_size);
uint8_t GraySensor_GetData(float (*output)[CHANNEL_NUMBER]);
void GraySensor_GetFilteredData(float output[CHANNEL_NUMBER]);

#endif

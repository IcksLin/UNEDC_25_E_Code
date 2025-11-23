#include "No_MCU_Gray_Sensor.h"
#include <string.h>
#include <stdio.h>
#include <stdlib.h>
#include "Delay.h"

static float Sensor_Channel_Data[CHANNEL_NUMBER];
static float Sensor_Channel_black_value[CHANNEL_NUMBER];
static float Sensor_Channel_Data_Cache[CHANNEL_NUMBER][3];
GraySensorQueue grayQueue;

//ADC初始化
void Init_ADC(ADC_HandleTypeDef*hadc){
    Delay_ns(10);
    HAL_ADC_Start(hadc);
}



//可由HAL库自动生成并初始化
void GraySensor_Init(void) {
    // 初始化GPIO
    GPIO_InitTypeDef GPIO_InitStruct = {0};
    
    // 配置通道选择引脚为输出
    GPIO_InitStruct.Pin = GRAY_SENSOR_A0_PIN | GRAY_SENSOR_A1_PIN | GRAY_SENSOR_A2_PIN;
    GPIO_InitStruct.Mode = GPIO_MODE_OUTPUT_PP;
    GPIO_InitStruct.Pull = GPIO_NOPULL;
    GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_LOW;
    HAL_GPIO_Init(GRAY_SENSOR_A0_PORT, &GPIO_InitStruct);
    
    // 配置模拟输入引脚
    GPIO_InitStruct.Pin = GRAY_SENSOR_OUT_PIN;
    GPIO_InitStruct.Mode = GPIO_MODE_ANALOG;
    HAL_GPIO_Init(GRAY_SENSOR_OUT_PORT, &GPIO_InitStruct);
}

//相关滤波算法
static float Apply_MovingAvg(float* buffer, uint8_t size) {
    float sum = 0;
    for(uint8_t i=0; i<size; i++) {
        sum += buffer[i];
    }
    return sum / size;
}

static int Float_Compare(const void* a, const void* b) {
    float diff = *(const float*)a - *(const float*)b;
    return (diff > 0) ? 1 : ((diff < 0) ? -1 : 0);
}

static float Apply_MedianFilter(float* buffer, uint8_t size) {
    float temp[7]; // 最大支持7点中值
    memcpy(temp, buffer, size*sizeof(float));
    qsort(temp, size, sizeof(float), Float_Compare);
    return temp[size/2]; // 取中值
}

static void Select_Channel(uint8_t ch) {
    // 用3个GPIO输出二进制编码（ch 0-7）
    HAL_GPIO_WritePin(GRAY_SENSOR_A0_PORT, GRAY_SENSOR_A0_PIN, (ch & 0x01));
    HAL_GPIO_WritePin(GRAY_SENSOR_A1_PORT, GRAY_SENSOR_A1_PIN, (ch & 0x02));
    HAL_GPIO_WritePin(GRAY_SENSOR_A2_PORT, GRAY_SENSOR_A2_PIN, (ch & 0x04));
    Delay_us(1);  // 等待信号稳定
}

void GraySensor_Update(ADC_HandleTypeDef* hadc) {
    static uint8_t current_ch = 0;
    static float ch_history[CHANNEL_NUMBER][7] = {0};
    static uint8_t hist_idx[CHANNEL_NUMBER] = {0};
    
    Select_Channel(current_ch);
    
    HAL_ADC_Start(hadc);
    if(HAL_ADC_PollForConversion(hadc, 10) == HAL_OK) {
        float raw = HAL_ADC_GetValue(hadc);
        
        // 更新历史数据（用于滤波）
        ch_history[current_ch][hist_idx[current_ch]++] = raw;
        if(hist_idx[current_ch] >= grayQueue.filter_window) {
            hist_idx[current_ch] = 0;
        }
        
        // 应用滤波
        float filtered_val = raw;
        switch(grayQueue.filter_type) {
            case FILTER_MOVING_AVG:
                filtered_val = Apply_MovingAvg(ch_history[current_ch], 
                                             grayQueue.filter_window);
                break;
            case FILTER_MEDIAN:
                filtered_val = Apply_MedianFilter(ch_history[current_ch],
                                                grayQueue.filter_window);
                break;
            default:
                break;
        }
        
        // 存入队列（修复点：增加队列计数管理）
        if(grayQueue.count < QUEUE_DEPTH) {
            grayQueue.data[current_ch][grayQueue.rear] = filtered_val;
            grayQueue.rear = (grayQueue.rear + 1) % QUEUE_DEPTH;
            grayQueue.count++;  // 修复遗漏的计数递增
        } else {
            // 队列满时覆盖最旧数据
            grayQueue.data[current_ch][grayQueue.front] = filtered_val;
            grayQueue.front = (grayQueue.front + 1) % QUEUE_DEPTH;
        }
        
        grayQueue.filtered[current_ch] = filtered_val;
    }
    HAL_ADC_Stop(hadc);
    
    current_ch = (current_ch + 1) % CHANNEL_NUMBER;
}
void GraySensor_SetFilter(GrayFilterType type, uint8_t window_size) {
    grayQueue.filter_type = type;
    grayQueue.filter_window = (window_size <= 7) ? window_size : 7;
}

//获取滤波后数值
void GraySensor_GetFilteredData(float output[CHANNEL_NUMBER]) {
    memcpy(output, grayQueue.filtered, CHANNEL_NUMBER*sizeof(float));
}

uint8_t GraySensor_GetData(float (*output)[CHANNEL_NUMBER]) {
    if(grayQueue.count == 0) return 0;
    
    // 读取最早的一组数据
    for(int i=0; i<CHANNEL_NUMBER; i++) {
        (*output)[i] = grayQueue.data[i][grayQueue.front];
    }
    
    // 更新队列指针
    grayQueue.front = (grayQueue.front + 1) % QUEUE_DEPTH;
    grayQueue.count--;
    
    return 1;
}

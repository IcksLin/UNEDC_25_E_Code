// #include "No_MCU_Gray_Sensor.h"
// #include <string.h>
// #include <stdlib.h>

// #define MAX_FILTER_WINDOW 7

// uint16_t white_background[CHANNEL_NUMBER] = {0}; // 白色背景下的ADC值
// uint16_t black_background[CHANNEL_NUMBER] = {0}; // 黑色背景下的ADC值

// GraySensorQueue grayQueue;
// float queueData[CHANNEL_NUMBER] = {0}; // 读取数组，储存滤波后的原始数据
// float threshold[CHANNEL_NUMBER] = {1417, 1430, 1465, 1427, 1441, 1396, 1400, 1425}; //    校准阈值数据
// uint8_t gray_result[CHANNEL_NUMBER] = {0}; // 二值化结果
// uint8_t copy_result[CHANNEL_NUMBER] = {0}; // 用于复制数据的结果数组

// uint8_t calibrate_process_white = 0;
// uint8_t calibrate_process_black = 0;

// // ADC初始化
// void Init_ADC(ADC_HandleTypeDef* hadc) {
//     Delay_ns(10);
//     HAL_ADC_Start(hadc);
// }

// // GPIO & ADC引脚初始化
// void GraySensor_Init(void) {
//     GPIO_InitTypeDef GPIO_InitStruct = {0};

//     // 配置通道选择引脚为输出
//     GPIO_InitStruct.Pin = GRAY_SENSOR_A0_PIN | GRAY_SENSOR_A1_PIN | GRAY_SENSOR_A2_PIN;
//     GPIO_InitStruct.Mode = GPIO_MODE_OUTPUT_PP;
//     GPIO_InitStruct.Pull = GPIO_NOPULL;
//     GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_LOW;
//     HAL_GPIO_Init(GRAY_SENSOR_A0_PORT, &GPIO_InitStruct);

//     // 配置模拟输入引脚
//     GPIO_InitStruct.Pin = GRAY_SENSOR_OUT_PIN;
//     GPIO_InitStruct.Mode = GPIO_MODE_ANALOG;
//     HAL_GPIO_Init(GRAY_SENSOR_OUT_PORT, &GPIO_InitStruct);
// }

// // 滤波算法
// static float Apply_MovingAvg(float* buffer, uint8_t size) {
//     float sum = 0;
//     for(uint8_t i = 0; i < size; i++) {
//         sum += buffer[i];
//     }
//     return sum / size;
// }

// static int Float_Compare(const void* a, const void* b) {
//     float diff = *(const float*)a - *(const float*)b;
//     return (diff > 0) ? 1 : ((diff < 0) ? -1 : 0);
// }

// static float Apply_MedianFilter(float* buffer, uint8_t size) {
//     float temp[MAX_FILTER_WINDOW];
//     memcpy(temp, buffer, size * sizeof(float));
//     qsort(temp, size, sizeof(float), Float_Compare);
//     return temp[size/2];
// }

// // 通道选择
// static void Select_Channel(uint8_t ch) {
//     HAL_GPIO_WritePin(GRAY_SENSOR_A0_PORT, GRAY_SENSOR_A0_PIN, (ch & 0x01) ? GPIO_PIN_SET : GPIO_PIN_RESET);
//     HAL_GPIO_WritePin(GRAY_SENSOR_A1_PORT, GRAY_SENSOR_A1_PIN, (ch & 0x02) ? GPIO_PIN_SET : GPIO_PIN_RESET);
//     HAL_GPIO_WritePin(GRAY_SENSOR_A2_PORT, GRAY_SENSOR_A2_PIN, (ch & 0x04) ? GPIO_PIN_SET : GPIO_PIN_RESET);
//     Delay_us(1);
// }

// void GraySensor_Update(ADC_HandleTypeDef* hadc) {
//     static uint8_t current_ch = 0;
//     static float ch_history[CHANNEL_NUMBER][MAX_FILTER_WINDOW] = {0};
//     static uint8_t hist_idx[CHANNEL_NUMBER] = {0};

//     Select_Channel(current_ch);

//     HAL_ADC_Start(hadc);
//     if(HAL_ADC_PollForConversion(hadc, 10) == HAL_OK) {
//         float raw = HAL_ADC_GetValue(hadc);

//         // 更新历史数据
//         ch_history[current_ch][hist_idx[current_ch]++] = raw;
//         if(hist_idx[current_ch] >= grayQueue.filter_window) {
//             hist_idx[current_ch] = 0;
//         }

//         // 滤波处理
//         float filtered_val = raw;
//         switch(grayQueue.filter_type) {
//             case FILTER_MOVING_AVG:
//                 filtered_val = Apply_MovingAvg(ch_history[current_ch], grayQueue.filter_window);
//                 break;
//             case FILTER_MEDIAN:
//                 filtered_val = Apply_MedianFilter(ch_history[current_ch], grayQueue.filter_window);
//                 break;
//             default:
//                 break;
//         }

//         // 存入队列
//         if(grayQueue.count < QUEUE_DEPTH) {
//             grayQueue.data[current_ch][grayQueue.rear] = filtered_val;
//             grayQueue.rear = (grayQueue.rear + 1) % QUEUE_DEPTH;
//             grayQueue.count++;
//         } else {
//             // 队列满时覆盖最旧数据
//             grayQueue.data[current_ch][grayQueue.front] = filtered_val;
//             grayQueue.front = (grayQueue.front + 1) % QUEUE_DEPTH;
//         }

//         grayQueue.filtered[current_ch] = filtered_val;
//         queueData[current_ch] = filtered_val; // 供校准用
//     }
//     HAL_ADC_Stop(hadc);

//     current_ch = (current_ch + 1) % CHANNEL_NUMBER;
// }

// void GraySensor_SetFilter(GrayFilterType type, uint8_t window_size) {
//     grayQueue.filter_type = type;
//     grayQueue.filter_window = (window_size <= MAX_FILTER_WINDOW) ? window_size : MAX_FILTER_WINDOW;
// }

// // 获取滤波后数值
// void GraySensor_GetFilteredData(float output[CHANNEL_NUMBER]) {
//     memcpy(output, grayQueue.filtered, CHANNEL_NUMBER * sizeof(float));
// }

// // 取出一组（队列最早）数据
// uint8_t GraySensor_GetData(float (*output)[CHANNEL_NUMBER]) {
//     if(grayQueue.count == 0) return 0;
//     for(int i = 0; i < CHANNEL_NUMBER; i++) {
//         (*output)[i] = grayQueue.data[i][grayQueue.front];
//     }
//     grayQueue.front = (grayQueue.front + 1) % QUEUE_DEPTH;
//     grayQueue.count--;
//     return 1;
// }

// // 校准白色背景（需在白色背景下调用）
// void CalibrateWhite(void) {
//     float sum[CHANNEL_NUMBER] = {0};
//     for (int i = 0; i < SAMPLE_TIMES; i++) {
//         for (int ch = 0; ch < CHANNEL_NUMBER; ch++) {
//             sum[ch] += queueData[ch];
//         }
//         Delay_ms(10);
//     }
//     for (int ch = 0; ch < CHANNEL_NUMBER; ch++) {
//         white_background[ch] = (uint16_t)(sum[ch] / SAMPLE_TIMES);
//     }
//     calibrate_process_white = 1;
// }

// // 校准黑色背景（需在黑色背景下调用）
// void CalibrateBlack(void) {
//     float sum[CHANNEL_NUMBER] = {0};
//     for (int i = 0; i < SAMPLE_TIMES; i++) {
//         for (int ch = 0; ch < CHANNEL_NUMBER; ch++) {
//             sum[ch] += queueData[ch];
//         }
//         Delay_ms(10);
//     }
//     for (int ch = 0; ch < CHANNEL_NUMBER; ch++) {
//         black_background[ch] = (uint16_t)(sum[ch] / SAMPLE_TIMES);
//     }
//     calibrate_process_black = 1;
// }

// // 归一化灰度值（0=纯黑，1=纯白）
// float NormalizeGrayValue(uint16_t raw_adc, uint8_t ch) {
//     if (white_background[ch] == black_background[ch]) {
//         return 0.5f;
//     }
//     float normalized = (float)(raw_adc - black_background[ch]) / (float)(white_background[ch] - black_background[ch]);
//     if (normalized > 1.0f) return 1.0f;
//     if (normalized < 0.0f) return 0.0f;
//     return normalized;
// }

// // 二值化
// void GrayToBinary(float adc_values[CHANNEL_NUMBER], uint8_t result[CHANNEL_NUMBER]) {
//     for (int ch = 0; ch < CHANNEL_NUMBER; ch++) {
//         result[ch] = ((uint16_t)adc_values[ch] < threshold[ch]) ? 1 : 0;
//     }
// }

// // 阈值计算
// void CalculateWeightedThreshold(uint16_t white[CHANNEL_NUMBER], uint16_t black[CHANNEL_NUMBER], float k) {
//     if(calibrate_process_white && calibrate_process_black) {
//         for (int ch = 0; ch < CHANNEL_NUMBER; ch++) {
//             threshold[ch] = black[ch] * (1.0f - k) + (white[ch] - black[ch]) * k;
//         }
//         calibrate_process_white = 0;
//         calibrate_process_black = 0;
//     }
// }


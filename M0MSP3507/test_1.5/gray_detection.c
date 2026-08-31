#include "gray_detection.h"
#include "OLED.h"
#include "PID.h"


uint8_t gray_values_used[GRAY_USED_CH_NUM] = {0};   //10路灰度值
int mapped_values[GRAY_USED_CH_NUM] = {0};          //映射后的值
uint8_t gray_status_flags = 0;

static int last_valid_error = 0;                     //上次的灰度偏差值
static uint8_t lost_line_flag = 0;                  //丢线标志位



// x坐标表，步长为2，中心两个为0
static const int gray_x[GRAY_USED_CH_NUM] = {-8, -6, -4, -2, 0, 0, 2, 4, 6, 8};

void Gray_ReadAll_Used(void)
{
    for (int i = 0; i < GRAY_USED_CH_NUM; ++i)
    {
        gray_values_used[i] = DL_GPIO_readPins(gray_channels_used[i].port, gray_channels_used[i].pin) ? 1 : 0;
    }
}

void Gray_MapToParabola(void)
{
    for (int i = 0; i < GRAY_USED_CH_NUM; ++i)
    {
        if (gray_values_used[i])
        {
            int32_t val = gray_x[i];
            if (val == 0) {
                // 中心特殊处理：根据位置赋予最小有效值
                mapped_values[i] = (i < GRAY_USED_CH_NUM/2) ? -1 : 1; // 左侧0赋-1，右侧0赋1
            } else {
                // 原抛物线映射
                int32_t sign = (val >= 0) ? 1 : -1;
                val = sign * ((val * val * 4) + (ABS(val) * 3));
                mapped_values[i] = (val > 255) ? 255 : ((val < -255) ? -255 : (int)val);
            }
        }
        else
        {
            mapped_values[i] = 0;
        }
    }
}

void OLED_DisplayGrayValues(void)
{
    OLED_Clear();
    OLED_ShowString(0, 0, "GRAY:", OLED_6X8);

    for (int i = 0; i < GRAY_USED_CH_NUM; ++i)
    {
        uint8_t x = 15 * (i % 8);
        uint8_t y = 8 + 8 * (i / 8);
        OLED_ShowSignedNum(x, y, mapped_values[i], 2, OLED_6X8); // 显示映射值
    }

    OLED_ShowString(0, 24, "X:-8 -6 -4 -2 0 0 2 4", OLED_6X8);
    OLED_ShowString(0, 32, "X:6 8", OLED_6X8);
}

// 状态记录结构体
typedef struct {
    int last_valid_error;   // 最后一次有效误差值
    uint8_t lost_count;     // 连续丢线计数器
    uint8_t was_tracking;   // 上次是否在追踪状态
} Gray_State;

static Gray_State gray_state = {0, 0, 0};

int Gray_CalculateError(void)
{
    int active_count = 0;
    int left_dark = 0;
    int right_dark = 0;
    int sum = 0;
    int last_active_index = -1;

    // 统计有效传感器
    for (int i = 0; i < GRAY_USED_CH_NUM; ++i) {
        if (mapped_values[i] != 0) {
            sum += mapped_values[i];
            active_count++;
            last_active_index = i;
            if (gray_x[i] < 0) left_dark++;
            else right_dark++;
        }
    }

    // 状态更新逻辑
    if (active_count == 0) {
        // 丢线状态处理
        gray_state.lost_count++;
        gray_state.was_tracking = 0;
        
        // 连续3次丢线则左急转
        if (gray_state.lost_count >= 3) {
            return -250; // 特殊左转指令
        }
        return gray_state.last_valid_error;
    }
    else {
        // 正常追踪状态
        int current_error = 0;
        
        // 直角拐弯判断
        if (left_dark >= 3) {
            current_error = (mapped_values[0] != 0) ? -255 : -200;
        }
        else if (right_dark >= 3) {
            current_error = (mapped_values[GRAY_USED_CH_NUM-1] != 0) ? 255 : 200;
        }
        // 正常循迹处理
        else if (active_count == 1) {
            current_error = sum;
        }
        else if (active_count == 2) {
            for (int i = 0; i < GRAY_USED_CH_NUM; ++i) {
                if (mapped_values[i] != 0 && i != last_active_index) {
                    if (ABS(gray_x[i] - gray_x[last_active_index]) == 2) {
                        current_error = (sum >> 1);
                        break;
                    }
                    return 0;
                }
            }
        }

        // 状态重置
        if (!gray_state.was_tracking) {
            gray_state.lost_count = 0;
            gray_state.was_tracking = 1;
        }
        gray_state.last_valid_error = current_error;
        return current_error;
    }
}


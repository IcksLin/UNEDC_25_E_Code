#ifndef _GRAY_SENSOR_H_
#define _GRAY_SENSOR_H_

#include <stdint.h>
#include "ti_msp_dl_config.h"

#define GRAY_USED_CH_NUM 10
#define EDGE_LEFT_ACTIVATED  0x01
#define EDGE_RIGHT_ACTIVATED 0x02


typedef struct {
    void *port;
    uint32_t pin;
} GrayChannel_t;

extern uint8_t gray_status_flags;

static const GrayChannel_t gray_channels_used[GRAY_USED_CH_NUM] = {
    {GRAY_CH_0_PORT,  GRAY_CH_0_PIN},   // 0
    {GRAY_CH_2_PORT,  GRAY_CH_2_PIN},   // 2
    {GRAY_CH_3_PORT,  GRAY_CH_3_PIN},   // 3
    {GRAY_CH_4_PORT,  GRAY_CH_4_PIN},   // 4
    {GRAY_CH_5_PORT,  GRAY_CH_5_PIN},   // 5
    {GRAY_CH_6_PORT,  GRAY_CH_6_PIN},   // 6
    {GRAY_CH_7_PORT,  GRAY_CH_7_PIN},   // 7
    {GRAY_CH_8_PORT,  GRAY_CH_8_PIN},   // 8
    {GRAY_CH_9_PORT,  GRAY_CH_9_PIN},   // 9
    {GRAY_CH_11_PORT, GRAY_CH_11_PIN},  // 11
};

extern uint8_t gray_values_used[GRAY_USED_CH_NUM];
extern int mapped_values[GRAY_USED_CH_NUM];

void Gray_ReadAll_Used(void);
void Gray_MapToParabola(void);
void OLED_DisplayGrayValues(void);
int Gray_CalculateError(void);


#endif // _GRAY_SENSOR_H_
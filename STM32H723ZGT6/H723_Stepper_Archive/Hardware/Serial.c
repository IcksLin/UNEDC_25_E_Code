// usart.c
#include "Serial.h"
#include "OLED.h"
extern UART_HandleTypeDef huart9;
// extern UART_HandleTypeDef huart8;
// --- 一级缓存区（中断专用）---
static volatile uint8_t s_rx_buffer[MAX_PACKET_LENGTH];
static volatile uint16_t s_rx_index = 0;
static volatile uint8_t s_rx_byte;  // 单字节接收缓存

// --- 二级缓存区（环形队列）---
static Packet_t s_packet_queue[PACKET_QUEUE_SIZE];
static volatile uint16_t s_queue_head = 0;
static volatile uint16_t s_queue_tail = 0;
static volatile uint8_t s_queue_count = 0;

// 启动中断接收
void UART_StartReceiveIT(void) {
    HAL_UART_Receive_IT(&huart9, (uint8_t*)&s_rx_byte, 1);
}

// 从二级缓存区获取包（主循环调用）
bool UART_GetPacket(Packet_t* packet) {
    if (s_queue_count == 0) return false;
    // 复制数据包
    uint16_t idx = s_queue_tail;
    memcpy(packet->data, s_packet_queue[idx].data, s_packet_queue[idx].length);
    packet->length = s_packet_queue[idx].length;

    // 更新队列指针
    s_queue_tail = (s_queue_tail + 1) % PACKET_QUEUE_SIZE;
    s_queue_count--;

    return true;
}

// 清空接收缓冲区
void UART_ClearRxBuffer(void) {
    s_rx_index = 0;
}

/// 中断回调函数
void HAL_UART_RxCpltCallback(UART_HandleTypeDef *huart) {
  
    if (huart == &huart9) {
        static bool receiving_packet = false;
        // OLED_ShowString(5*16,2*16,"Recive",OLED_8X16);
        // 检测包头
        if (s_rx_byte == '@') {
            receiving_packet = true;
            s_rx_index = 0;
            s_rx_buffer[s_rx_index++] = s_rx_byte;
        }

        // 正在接收数据包
        else if (receiving_packet) {
            s_rx_buffer[s_rx_index++] = s_rx_byte;

            // 检测包尾（至少2字节才能判断）
            if (s_rx_index >= 2 && 
                s_rx_buffer[s_rx_index-2] == '\r' && 
                s_rx_buffer[s_rx_index-1] == '\n') {
                receiving_packet = false;

                // 如果队列未满，将包存入二级缓存区
                if (s_queue_count < PACKET_QUEUE_SIZE) {
                    uint16_t idx = s_queue_head;
                    memcpy(s_packet_queue[idx].data, (uint8_t*)s_rx_buffer, s_rx_index);
                    s_packet_queue[idx].length = s_rx_index;
                    s_queue_head = (s_queue_head + 1) % PACKET_QUEUE_SIZE;
                    s_queue_count++;
                }

                s_rx_index = 0; // 重置一级缓存区
            }

            // 防止一级缓存区溢出
            if (s_rx_index >= MAX_PACKET_LENGTH) {
                receiving_packet = false;
                s_rx_index = 0;
            }
        }

        // 重启中断接收
        HAL_UART_Receive_IT(&huart9, (uint8_t*)&s_rx_byte, 1);
    }
}
// usart.h
#ifndef __SERIAL_H__
#define __SERIAL_H__

#include "main.h"
#include <string.h>
#include <stdbool.h>

// 数据包界定符
#define PACKET_START_CHAR   '@'    // 包头
#define PACKET_END_STR      "\r\n" // 包尾
#define MAX_PACKET_LENGTH   128    // 单包最大长度
#define PACKET_QUEUE_SIZE   5      // 二级缓存区队列容量

// 数据包结构
typedef struct {
    uint8_t data[MAX_PACKET_LENGTH];
    uint16_t length;
} Packet_t;

extern Packet_t received_packet; // 用于接收解析的数据包


// 全局变量声明
void UART_StartReceiveIT(void);
bool UART_GetPacket(Packet_t* packet); // 从二级缓存区获取包
void UART_ClearRxBuffer(void);         // 清空接收缓冲区

#endif /* __USART_H__ */
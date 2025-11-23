// packet_parser.h
#ifndef __PACKET_PARSER_H__
#define __PACKET_PARSER_H__

#include <stdbool.h>
#include "main.h"

// 定义坐标结构体
typedef struct {
    int x;
    int y;
} Point_t;

// 全局存储数组（根据实际需求调整大小）
extern Point_t rect_coords[4];  // 存储RECT的4个坐标
extern Point_t red_point;       // 存储RED的1个坐标
extern Point_t green_point;     // 存储GREEN的1个坐标
extern Point_t target_point;
extern Point_t copy_red;
extern Point_t blue_brick;
extern Point_t copy_blue;


extern int x_move,y_move;


// 数据包解析函数
bool ParsePacket(const char* packet);

#endif /* __PACKET_PARSER_H__ */

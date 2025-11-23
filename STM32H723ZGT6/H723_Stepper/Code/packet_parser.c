// packet_parser.c
#include "packet_parser.h"
#include <string.h>
#include <stdio.h>
#include <stdlib.h>

// 全局存储数组定义
Point_t rect_coords[4];
Point_t rect_coords_pair[2];
Point_t red_point;
Point_t green_point;
Point_t target_point;
Point_t blue_brick;
Point_t copy_red;
Point_t copy_blue;

// 解析数据包（示例输入: "@RECT:(385,458),(150,189),(383,484),(278,191)\r\n"）
bool ParsePacket(const char *packet)
{
    if (packet[0] != '@')
        return false;

    // 根据前缀选择解析逻辑
    if (strncmp(packet, "@ROI:", 5) == 0)
    {
        // 解析RECT的4个坐标
        const char *ptr = packet + 5; // 跳过"@RECT:"
        for (int i = 0; i < 4; i++)
        {
            if (sscanf(ptr, "(%d,%d)", &rect_coords[i].x, &rect_coords[i].y) != 2)
            {
                return false;
            }
            ptr = strchr(ptr + 1, '('); // 查找下一个坐标
            if (!ptr && i < 3)
                return false; // 不足4个坐标则报错
        }
        return true;
    }
    else if (strncmp(packet, "@RED:", 5) == 0)
    {
        // 解析RED的1个坐标
        return (sscanf(packet + 5, "(%d,%d)", &red_point.x, &red_point.y) == 2);
    }
    else if (strncmp(packet, "@LASER:", 7) == 0)
    {
        // 解析GREEN的1个坐标
        return (sscanf(packet + 7, "(%d,%d)", &green_point.x, &green_point.y) == 2);
    }
    else if (strncmp(packet, "@BLUE:", 6) == 0)
    {
        // 解析GREEN的1个坐标
        return (sscanf(packet + 6, "(%d,%d)", &blue_brick.x, &blue_brick.y) == 2);
    }
    else if (strncmp(packet, "@RECT:", 6) == 0)
    {
        // 解析RECT的2个坐标
        const char *ptr = packet + 6; // 跳过"@RECT:"
        for (int i = 0; i < 2; i++)
        {
            if (sscanf(ptr, "(%d,%d)", &rect_coords_pair[i].x, &rect_coords_pair[i].y) != 2)
            {
                return false;
            }
            ptr = strchr(ptr + 1, '('); // 查找下一个坐标
            if (!ptr && i < 1)
                return false; // 不足2个坐标则报错
        }
        return true;
    }
    else if (strncmp(packet, "@LASER", 6) == 0)
    {
        // 解析LASER的1个坐标
        return (sscanf(packet + 6, "(%d,%d)", &target_point.x, &target_point.y) == 2);
    }

    return false; // 未知类型
}

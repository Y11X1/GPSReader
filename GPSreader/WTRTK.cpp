#include "WTRTK.h"
#include "GPSstr.h"  
#include <stdio.h>
#include <string.h>

// 分析WTRTK信息
// buf:接收到的WTRTK数据缓冲区首地址
void _WTRTK_Analysis(WTRTK_INFO* wtrtk_info, u1* buf)
{
    u1* p1;
    u1 dx = 0;
    u1 posx;
    u4 temp;
    double raw_val;

    // 1. 定位帧头
    p1 = (u1*)strstr((const char*)buf, "$WTRTK");
    if (p1 == NULL) {
        return;  // 没有找到有效帧
    }

    // ========== 解析差分距离（字段1-4） ==========
    // 字段1: 差分X距离
    posx = _Find_Pos(p1, 1);
    if (posx != 0xFF) {
        temp = _Str2num(p1 + posx, &dx);
        wtrtk_info->diff_x = (f4)temp / (f4)_Pow(10, dx);
    }

    // 字段2: 差分Y距离
    posx = _Find_Pos(p1, 2);
    if (posx != 0xFF) {
        temp = _Str2num(p1 + posx, &dx);
        wtrtk_info->diff_y = (f4)temp / (f4)_Pow(10, dx);
    }

    // 字段3: 差分Z距离
    posx = _Find_Pos(p1, 3);
    if (posx != 0xFF) {
        temp = _Str2num(p1 + posx, &dx);
        wtrtk_info->diff_z = (f4)temp / (f4)_Pow(10, dx);
    }

    // 字段4: 差分R距离
    posx = _Find_Pos(p1, 4);
    if (posx != 0xFF) {
        temp = _Str2num(p1 + posx, &dx);
        wtrtk_info->diff_r = (f4)temp / (f4)_Pow(10, dx);
    }

    // ========== 解析姿态角（字段5-7） ==========
    // 字段5: 余度X
    posx = _Find_Pos(p1, 5);
    if (posx != 0xFF) {
        temp = _Str2num(p1 + posx, &dx);
        wtrtk_info->angle_x = (f4)temp / (f4)_Pow(10, dx);
    }

    // 字段6: 余度Y
    posx = _Find_Pos(p1, 6);
    if (posx != 0xFF) {
        temp = _Str2num(p1 + posx, &dx);
        wtrtk_info->angle_y = (f4)temp / (f4)_Pow(10, dx);
    }

    // 字段7: 余度Z
    posx = _Find_Pos(p1, 7);
    if (posx != 0xFF) {
        temp = _Str2num(p1 + posx, &dx);
        wtrtk_info->angle_z = (f4)temp / (f4)_Pow(10, dx);
    }

    // ========== 解析状态与信号（字段8-12） ==========
    // 字段8: 定向状态
    posx = _Find_Pos(p1, 8);
    if (posx != 0xFF) {
        temp = _Str2num(p1 + posx, &dx);
        wtrtk_info->orient_status = (u1)temp;
    }

    // 字段9: 保留位1
    posx = _Find_Pos(p1, 9);
    if (posx != 0xFF) {
        temp = _Str2num(p1 + posx, &dx);
        wtrtk_info->reserved1 = (u1)temp;
    }

    // 字段10: 保留位2
    posx = _Find_Pos(p1, 10);
    if (posx != 0xFF) {
        temp = _Str2num(p1 + posx, &dx);
        wtrtk_info->reserved2 = (u1)temp;
    }

    // 字段11: 信号质量
    posx = _Find_Pos(p1, 11);
    if (posx != 0xFF) {
        temp = _Str2num(p1 + posx, &dx);
        wtrtk_info->signal_quality = (u1)temp;
    }

    // 字段12: 通讯数据量
    posx = _Find_Pos(p1, 12);
    if (posx != 0xFF) {
        temp = _Str2num(p1 + posx, &dx);
        wtrtk_info->comm_volume = (u1)temp;
    }

    // ========== 解析航向与定位（字段13-14） ==========
    // 字段13: 运动航向角
    posx = _Find_Pos(p1, 13);
    if (posx != 0xFF) {
        temp = _Str2num(p1 + posx, &dx);
        wtrtk_info->move_heading = (f4)temp / (f4)_Pow(10, dx);
    }

    // 字段14: 定位标志
    posx = _Find_Pos(p1, 14);
    if (posx != 0xFF) {
        temp = _Str2num(p1 + posx, &dx);
        wtrtk_info->loc_flag = (u1)temp;
    }

    // ========== 解析位置数据（字段15-17） ==========
    // 字段15: 纬度（已经是十进制）
    posx = _Find_Pos(p1, 15);
    if (posx != 0xFF) {
        // 注意：WTRTK的纬度直接是十进制，不是NMEA格式
        temp = _Str2num(p1 + posx, &dx);
        wtrtk_info->latitude = (f8)temp / (f8)_Pow(10, dx);
    }

    // 字段16: 经度（已经是十进制）
    posx = _Find_Pos(p1, 16);
    if (posx != 0xFF) {
        temp = _Str2num(p1 + posx, &dx);
        wtrtk_info->longitude = (f8)temp / (f8)_Pow(10, dx);
    }

    // 字段17: 高度
    posx = _Find_Pos(p1, 17);
    if (posx != 0xFF) {
        temp = _Str2num(p1 + posx, &dx);
        wtrtk_info->altitude = (f4)temp / (f4)_Pow(10, dx);
    }

    // ========== 解析定向专用数据（字段18-19） ==========
    // 字段18: 定向航向角（4GA专用）
    posx = _Find_Pos(p1, 18);
    if (posx != 0xFF) {
        temp = _Str2num(p1 + posx, &dx);
        wtrtk_info->orient_heading = (f4)temp / (f4)_Pow(10, dx);
    }

    // 字段19: 俯仰角（4GA专用）
    posx = _Find_Pos(p1, 19);
    if (posx != 0xFF) {
        temp = _Str2num(p1 + posx, &dx);
        wtrtk_info->pitch_angle = (f4)temp / (f4)_Pow(10, dx);
    }
}
#ifndef _WTRTK_H_
#define _WTRTK_H_

#include "gps_info.h" 

// WTRTK数据包结构体（共17个字段）
typedef struct {
    // 差分数据（4个距离）
    f4 diff_x;          // 差分X距离（米）
    f4 diff_y;          // 差分Y距离（米）
    f4 diff_z;          // 差分Z距离（米）
    f4 diff_r;          // 差分R距离（米）

    // 姿态数据（3个余度角）
    f4 angle_x;         // 余度X（度）
    f4 angle_y;         // 余度Y（度）
    f4 angle_z;         // 余度Z（度）

    // 状态与信号
    u1 orient_status;   // 定向状态（0-5）
    u1 reserved1;       // 保留位1
    u1 reserved2;       // 保留位2
    u1 signal_quality;  // 无线信号质量
    u1 comm_volume;     // 无线通讯数据量（byte）

    // 导航数据
    f4 move_heading;    // 运动航向角（0-360°）
    u1 loc_flag;        // 定位标志（0-2）

    // 位置数据
    f8 latitude;        // 纬度（十进制度）
    f8 longitude;       // 经度（十进制度）
    f4 altitude;        // GPS高度（米）

    // 定向专有用（4GA）
    f4 orient_heading;  // 定向航向角
    f4 pitch_angle;     // 俯仰角
} WTRTK_INFO;

// WTRTK解析函数
void _WTRTK_Analysis(WTRTK_INFO* wtrtk_info, u1* buf);

#endif

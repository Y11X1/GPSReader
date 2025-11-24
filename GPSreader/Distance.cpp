#define _USE_MATH_DEFINES 
#include "Distance.h"
#include <math.h>


#define EARTH_RADIUS 6371000.0  // 地球平均半径（米）

// 角度转弧度
#define DEG_TO_RAD(x) ((x) * M_PI / 180.0)

double calculateDistance(double lat1_deg, double lon1_deg,
    double lat2_deg, double lon2_deg)
{
    // 转换为弧度
    double lat1 = DEG_TO_RAD(lat1_deg);
    double lon1 = DEG_TO_RAD(lon1_deg);
    double lat2 = DEG_TO_RAD(lat2_deg);
    double lon2 = DEG_TO_RAD(lon2_deg);

    // 计算经纬度差值
    double dlat = lat2 - lat1;
    double dlon = lon2 - lon1;

    // Haversine公式
    double a = sin(dlat / 2) * sin(dlat / 2) +
        cos(lat1) * cos(lat2) *
        sin(dlon / 2) * sin(dlon / 2);
    double c = 2 * atan2(sqrt(a), sqrt(1 - a));

    return EARTH_RADIUS * c;
}

// 到达判断（距离阈值5米）
int isArrived(double currentLat, double currentLon,
    double targetLat, double targetLon,double Distance)
{
    double distance = calculateDistance(currentLat, currentLon,
        targetLat, targetLon);
    return (distance < Distance);  // 返回1表示到达
}
#ifndef _DISTANCE_H_
#define _DISTANCE_H_

// 计算两点间球面距离（米）
double calculateDistance(double lat1_deg, double lon1_deg,
    double lat2_deg, double lon2_deg);

// 检查是否到达目标点（距离<5米）
int isArrived(double currentLat, double currentLon,
    double targetLat, double targetLon);

#endif

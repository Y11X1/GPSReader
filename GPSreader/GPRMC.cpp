#include "GPRMC.h"
#include "GPSstr.h"
#include <stdio.h>

//分析GPRMC信息
//buf:接收到的GPS数据缓冲区首地址
void _GPRMC_Analysis(GPRMC_INFO* gps_rmc_info, u1* buf)
{
	u1* p1;
	u1 dx = 0;
	u1 posx;
	u4 temp;

	p1 = (u1*)strstr((const char*)buf, "$GNRMC");				// "$GPRMC"
	if (p1 == NULL) {
		return;
	}

	posx = _Find_Pos(p1, 1);									// 得到UTC时间
	if (posx != 0xFF) {
		int raw_time = _Str2num(p1 + posx, &dx);
		temp = raw_time / _Pow(10, dx);			// 得到UTC时间,去掉ms	 	
		gps_rmc_info->utc_time.hour = temp / 10000;
		gps_rmc_info->utc_time.min = (temp / 100) % 100;
		gps_rmc_info->utc_time.sec = temp % 100;
		gps_rmc_info->utc_time.ssec = raw_time % _Pow(10, dx);
		//printf("GPS UTC 小时：%d 分钟：%d 秒：%d 毫秒：%d \n\r",
		//	gps_rmc_info->utc_time.hour,
		//	gps_rmc_info->utc_time.min,
		//	gps_rmc_info->utc_time.sec,
		//	gps_rmc_info->utc_time.ssec);
	}

	posx = _Find_Pos(p1, 2);									// 得到定位状态
	if (posx != 0xFF) {
		gps_rmc_info->gps_state = *(p1 + posx);
		//printf("GPS卫星状态：%c \n\r", gps_rmc_info->gps_state);
	}

	posx = _Find_Pos(p1, 3);									// 得到纬度值
	if (posx != 0xFF) {
		temp = _Str2num(p1 + posx, &dx);
		gps_rmc_info->latitude_value = (f8)temp / (f8)_Pow(10, dx);
		//printf("GPS纬度值：%.5f \n\r", gps_rmc_info->latitude_value);
	}

	posx = _Find_Pos(p1, 4);									// 南纬还是北纬
	if (posx != 0xFF) {
		gps_rmc_info->latitude = *(p1 + posx);
		//printf("GPS纬度：%c \n\r", gps_rmc_info->latitude);
	}

	posx = _Find_Pos(p1, 5);									// 得到经度值
	if (posx != 0xFF) {
		temp = _Str2num(p1 + posx, &dx);
		gps_rmc_info->longtitude_value = (f8)temp / (f8)_Pow(10, dx);
		//printf("GPS经度值：%.5f \n\r", gps_rmc_info->longtitude_value);
	}

	posx = _Find_Pos(p1, 6);									// 东经还是西经
	if (posx != 0xFF) {
		gps_rmc_info->longtitude = *(p1 + posx);
		//printf("GPS经度：%c \n\r", gps_rmc_info->longtitude);
	}

	posx = _Find_Pos(p1, 7);									// 速度
	if (posx != 0xFF) {
		temp = _Str2num(p1 + posx, &dx);
		gps_rmc_info->speed = (f4)temp / (f4)_Pow(10, dx);
		//printf("GPS速度值：%.3f \n\r", gps_rmc_info->speed);
	}

	posx = _Find_Pos(p1, 8);									// 地面航向
	if (posx != 0xFF) {
		temp = _Str2num(p1 + posx, &dx);
		gps_rmc_info->azimuth_angle = (f4)temp / (f4)_Pow(10, dx);
		//printf("GPS速度值：%.3f \n\r", gps_rmc_info->azimuth_angle);
	}

	posx = _Find_Pos(p1, 9);									//得到UTC日期
	if (posx != 0xFF)
	{
		temp = _Str2num(p1 + posx, &dx);		 				//得到UTC日期
		gps_rmc_info->utc_time.date = temp / 10000;
		gps_rmc_info->utc_time.month = (temp / 100) % 100;
		gps_rmc_info->utc_time.year = 2000 + temp % 100;
		//	printf("GPS UTC 年：%d 月：%d 日：%d \n\r",
		//	gps_rmc_info->utc_time.year,
		//	gps_rmc_info->utc_time.month,
		//	gps_rmc_info->utc_time.date);
	}
}

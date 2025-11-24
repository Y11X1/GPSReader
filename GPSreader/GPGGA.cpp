#include "GPGGA.h"
#include "GPSstr.h"
#include <stdio.h>
#include <stdlib.h>
//分析GPGGA信息
//buf:接收到的GPS数据缓冲区首地址
void _GPGGA_Analysis(GPGGA_INFO* gps_gga_info, u1* buf)
{
	u1* p1;
	u1 dx = 0;
	u1 posx;
	u4 temp;

	p1 = (u1*)strstr((const char*)buf, "$GNGGA");				// "$GPRMC"
	if (p1 == NULL) {
		return;
	}

	posx = _Find_Pos(p1, 1);									// 得到UTC时间
	if (posx != 0xFF) {
		int raw_time = _Str2num(p1 + posx, &dx);
		temp = raw_time / _Pow(10, dx);		// 得到UTC时间,去掉ms	 	
		gps_gga_info->utc_time.hour = temp / 10000;
		gps_gga_info->utc_time.min = (temp / 100) % 100;
		gps_gga_info->utc_time.sec = temp % 100;
		gps_gga_info->utc_time.ssec = _Str2num(p1 + posx, &dx) % _Pow(10, dx);
		//printf("GPS UTC 小时：%d 分钟：%d 秒：%d 毫秒：%d \n\r",
		//	gps_gga_info->utc_time.hour,
		//	gps_gga_info->utc_time.min,
		//	gps_gga_info->utc_time.sec,
		//	gps_gga_info->utc_time.ssec);
	}

	posx = _Find_Pos(p1, 2);									// 得到纬度值
	if (posx != 0xFF) {
		temp = _Str2num(p1 + posx, &dx);
		gps_gga_info->latitude_value = (f8)temp / (f8)_Pow(10, dx);
	/*	printf("GPS纬度值：%.5f \n\r", gps_gga_info->latitude_value);*/
	}

	posx = _Find_Pos(p1, 3);									// 南纬还是北纬
	if (posx != 0xFF) {
		gps_gga_info->latitude = *(p1 + posx);
		//printf("GPS纬度：%c \n\r", gps_gga_info->latitude);
	}

	posx = _Find_Pos(p1, 4);									// 得到经度值
	if (posx != 0xFF) {
		temp = _Str2num(p1 + posx, &dx);
		gps_gga_info->longtitude_value = (f8)temp / (f8)_Pow(10, dx);
	/*	printf("GPS经度值：%.5f \n\r", gps_gga_info->longtitude_value);*/
	}

	posx = _Find_Pos(p1, 5);									// 东经还是西经
	if (posx != 0xFF) {
		gps_gga_info->longitude = *(p1 + posx);
		/*printf("GPS经度：%c \n\r", gps_gga_info->longitude);*/
	}

	posx = _Find_Pos(p1, 6);									// GPS状态
	if (posx != 0xFF) {
		temp = _Str2num(p1 + posx, &dx);
		gps_gga_info->gps_state = temp;
	/*	printf("GPS状态值：%d \n\r", gps_gga_info->gps_state);*/
	}

	posx = _Find_Pos(p1, 7);									// 卫星数量
	if (posx != 0xFF) {
		temp = _Str2num(p1 + posx, &dx);
		gps_gga_info->sate_num = temp;
	/*	printf("GPS卫星数量：%d \n\r", gps_gga_info->sate_num);*/
	}

	posx = _Find_Pos(p1, 8);									// HDOP水平精度因子
	if (posx != 0xFF) {
		temp = _Str2num(p1 + posx, &dx);
		gps_gga_info->hdop = (f4)temp / (f4)_Pow(10, dx);
	/*	printf("HDOP水平精度因子：%f \n\r", gps_gga_info->hdop);*/
	}

	posx = _Find_Pos(p1, 9);									// 海拔高度
	if (posx != 0xFF) {
		temp = _Str2num(p1 + posx, &dx);
		gps_gga_info->altitude = (f4)temp / (f4)_Pow(10, dx);
		/*printf("海拔高度：%f \n\r", gps_gga_info->altitude);*/
	}
}

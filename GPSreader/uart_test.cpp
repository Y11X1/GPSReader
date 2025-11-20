#include <stdlib.h>
#include <string.h>
#include <stdio.h>
#include "gps_info.h" 				   
#include "stdarg.h"	 
#include "math.h"
#include <windows.h>
#include <stdio.h>
#include <conio.h>

#define comPort L"COM3"  //串口号

u1 gpggaUpdated = 0;  // 1表示GPGGA刚解析完
u1 gprmcUpdated = 0;  // 1表示GPRMC刚解析完


GPS_ASCLLINFO Ascll_Info[2] = {
	ASCLL_DATA_GPGGA, 0x06, RCV_GPGGA_HEAD,
	ASCLL_DATA_GPRMC, 0x06, RCV_GPRMC_HEAD

};
BlkUartRcv blkUartRcv;
GPGGA_INFO gga_info;
GPRMC_INFO gmc_info;

//从buf里面得到第num个逗号所在的位置
//返回值:0~0xFE,代表逗号所在位置的偏移.
//       0xFF,代表不存在第num个逗号							  
u1 _Find_Pos(u1* buf, u1 u1Num)
{
	u1* p = buf;
	while (u1Num)
	{
		if (*buf == '*' || *buf < ' ' || *buf>'z')return 0xFF;//遇到'*'或者非法字符,则不存在第num个逗号
		if (*buf == ',')u1Num--;
		buf++;
	}
	return buf - p;
}

//m^n函数
//返回值:m^n次方.
u4 _Pow(u1 m, u1 n)
{
	u4 result = 1;
	while (n--)result *= m;
	return result;
}
//str转换为数字,以','或者'*'结束
//buf:数字存储区
//dx:小数点位数,返回给调用函数
//返回值:转换后的数值
int _Str2num(u1* buf, u1* dx)
{
	u1* p = buf;
	u4 ires = 0, fres = 0;
	u1 ilen = 0, flen = 0, i;
	u1 mask = 0;
	double res;
	while (1) //得到整数和小数的长度
	{
		if (*p == '-') { mask |= 0x02; p++; }//是负数
		if (*p == ',' || (*p == '*'))break;//遇到结束了
		if (*p == '.') { mask |= 0x01; p++; }//遇到小数点了
		else if (*p > '9' || (*p < '0'))	//有非法字符
		{
			ilen = 0;
			flen = 0;
			break;
		}
		if (mask & 0x01)flen++;
		else ilen++;
		p++;
	}
	if (mask & 0x02)buf++;		//去掉负号
	for (i = 0; i < ilen; i++)	//得到整数部分数据
	{
		ires += _Pow(10, ilen - 1 - i) * (buf[i] - '0');
	}
	if (flen > 5)flen = 5;	//最多取5位小数
	*dx = flen;	 			//小数点位数
	for (i = 0; i < flen; i++)	//得到小数部分数据
	{
		fres += _Pow(10, flen - 1 - i) * (buf[ilen + 1 + i] - '0');
	}
	res = ires * _Pow(10, flen) + fres;
	if (mask & 0x02)res = -res;
	return res;
}

//分析GPRMC信息
//buf:接收到的GPS数据缓冲区首地址
void _GPRMC_Analysis(GPRMC_INFO* gps_rmc_info, u1* buf)
{
	u1* p1;
	u1 dx = 0;
	u1 posx;
	u4 temp;

	p1 = (u1*)strstr((const char*)buf, "$GPRMC");				// "$GPRMC"
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
		printf("GPS UTC 小时：%d 分钟：%d 秒：%d 毫秒：%d \n\r",
			gps_rmc_info->utc_time.hour,
			gps_rmc_info->utc_time.min,
			gps_rmc_info->utc_time.sec,
			gps_rmc_info->utc_time.ssec);
	}

	posx = _Find_Pos(p1, 2);									// 得到定位状态
	if (posx != 0xFF) {
		gps_rmc_info->gps_state = *(p1 + posx);
		printf("GPS卫星状态：%c \n\r", gps_rmc_info->gps_state);
	}

	posx = _Find_Pos(p1, 3);									// 得到纬度值
	if (posx != 0xFF) {
		temp = _Str2num(p1 + posx, &dx);
		gps_rmc_info->latitude_value = (f8)temp / (f8)_Pow(10, dx);
		printf("GPS纬度值：%.5f \n\r", gps_rmc_info->latitude_value);
	}

	posx = _Find_Pos(p1, 4);									// 南纬还是北纬
	if (posx != 0xFF) {
		gps_rmc_info->latitude = *(p1 + posx);
		printf("GPS纬度：%c \n\r", gps_rmc_info->latitude);
	}

	posx = _Find_Pos(p1, 5);									// 得到经度值
	if (posx != 0xFF) {
		temp = _Str2num(p1 + posx, &dx);
		gps_rmc_info->longtitude_value = (f8)temp / (f8)_Pow(10, dx);
		printf("GPS经度值：%.5f \n\r", gps_rmc_info->longtitude_value);
	}

	posx = _Find_Pos(p1, 6);									// 东经还是西经
	if (posx != 0xFF) {
		gps_rmc_info->longtitude = *(p1 + posx);
		printf("GPS经度：%c \n\r", gps_rmc_info->longtitude);
	}

	posx = _Find_Pos(p1, 7);									// 速度
	if (posx != 0xFF) {
		temp = _Str2num(p1 + posx, &dx);
		gps_rmc_info->speed = (f4)temp / (f4)_Pow(10, dx);
		printf("GPS速度值：%.3f \n\r", gps_rmc_info->speed);
	}

	posx = _Find_Pos(p1, 8);									// 地面航向
	if (posx != 0xFF) {
		temp = _Str2num(p1 + posx, &dx);
		gps_rmc_info->azimuth_angle = (f4)temp / (f4)_Pow(10, dx);
		printf("GPS速度值：%.3f \n\r", gps_rmc_info->azimuth_angle);
	}

	posx = _Find_Pos(p1, 9);									//得到UTC日期
	if (posx != 0xFF)
	{
		temp = _Str2num(p1 + posx, &dx);		 				//得到UTC日期
		gps_rmc_info->utc_time.date = temp / 10000;
		gps_rmc_info->utc_time.month = (temp / 100) % 100;
		gps_rmc_info->utc_time.year = 2000 + temp % 100;
		printf("GPS UTC 年：%d 月：%d 日：%d \n\r",
			gps_rmc_info->utc_time.year,
			gps_rmc_info->utc_time.month,
			gps_rmc_info->utc_time.date);
	}
}


//分析GPGGA信息
//buf:接收到的GPS数据缓冲区首地址
void _GPGGA_Analysis(GPGGA_INFO* gps_gga_info, u1* buf)
{
	u1* p1;
	u1 dx = 0;
	u1 posx;
	u4 temp;

	p1 = (u1*)strstr((const char*)buf, "$GPGGA");				// "$GPRMC"
	if (p1 == NULL) {
		return;
	}

	posx = _Find_Pos(p1, 1);									// 得到UTC时间
	if (posx != 0xFF) {
		temp = _Str2num(p1 + posx, &dx) / _Pow(10, dx);		// 得到UTC时间,去掉ms	 	
		gps_gga_info->utc_time.hour = temp / 10000;
		gps_gga_info->utc_time.min = (temp / 100) % 100;
		gps_gga_info->utc_time.sec = temp % 100;
		gps_gga_info->utc_time.ssec = _Str2num(p1 + posx, &dx) % _Pow(10, dx);
		printf("GPS UTC 小时：%d 分钟：%d 秒：%d 毫秒：%d \n\r",
			gps_gga_info->utc_time.hour,
			gps_gga_info->utc_time.min,
			gps_gga_info->utc_time.sec,
			gps_gga_info->utc_time.ssec);
	}

	posx = _Find_Pos(p1, 2);									// 得到纬度值
	if (posx != 0xFF) {
		temp = _Str2num(p1 + posx, &dx);
		gps_gga_info->latitude_value = (f8)temp / (f8)_Pow(10, dx);
		printf("GPS纬度值：%.5f \n\r", gps_gga_info->latitude_value);
	}

	posx = _Find_Pos(p1, 3);									// 南纬还是北纬
	if (posx != 0xFF) {
		gps_gga_info->latitude = *(p1 + posx);
		printf("GPS纬度：%c \n\r", gps_gga_info->latitude);
	}

	posx = _Find_Pos(p1, 4);									// 得到经度值
	if (posx != 0xFF) {
		temp = _Str2num(p1 + posx, &dx);
		gps_gga_info->longtitude_value = (f8)temp / (f8)_Pow(10, dx);
		printf("GPS经度值：%.5f \n\r", gps_gga_info->longtitude_value);
	}

	posx = _Find_Pos(p1, 5);									// 东经还是西经
	if (posx != 0xFF) {
		gps_gga_info->longitude = *(p1 + posx);
		printf("GPS经度：%c \n\r", gps_gga_info->longitude);
	}

	posx = _Find_Pos(p1, 6);									// GPS状态
	if (posx != 0xFF) {
		temp = _Str2num(p1 + posx, &dx);
		gps_gga_info->gps_state = temp;
		printf("GPS状态值：%d \n\r", gps_gga_info->gps_state);
	}

	posx = _Find_Pos(p1, 7);									// 卫星数量
	if (posx != 0xFF) {
		temp = _Str2num(p1 + posx, &dx);
		gps_gga_info->sate_num = temp;
		printf("GPS卫星数量：%d \n\r", gps_gga_info->sate_num);
	}

	posx = _Find_Pos(p1, 8);									// HDOP水平精度因子
	if (posx != 0xFF) {
		temp = _Str2num(p1 + posx, &dx);
		gps_gga_info->hdop = (f4)temp / (f4)_Pow(10, dx);
		printf("HDOP水平精度因子：%f \n\r", gps_gga_info->hdop);
	}

	posx = _Find_Pos(p1, 9);									// 海拔高度
	if (posx != 0xFF) {
		temp = _Str2num(p1 + posx, &dx);
		gps_gga_info->altitude = (f4)temp / (f4)_Pow(10, dx);
		printf("海拔高度：%f \n\r", gps_gga_info->altitude);
	}
}


void _ResetUartRcvBuff()
{
	memset(&blkUartRcv, 0, sizeof(BlkUartRcv));
}

void GN_UartRcvGPSInfo(u1 u1Data)
{
	int check, sum, hour, min, sec;
	BlkUartRcv* pblkUartRcv = &blkUartRcv;
	u1* pu1Buff = pblkUartRcv->u1RcvBuff;
	u1	u1size = 0;
	char u1utc[20];
	if ((u1Data == ASCLL_DATA_$) &&
		(pblkUartRcv->u1State == RCV_STATE_IDIE)) {
		pblkUartRcv->u1State = RCV_STATE_START;
	}

	if (pblkUartRcv->u1DataLen >= MAX_RCV_SIZE) {
		_ResetUartRcvBuff();
	}

	switch (pblkUartRcv->u1State) {
	case RCV_STATE_START:
		pu1Buff[pblkUartRcv->u1DataLen++] = u1Data;
		if (u1Data == '\n') {
			for (u1size = 0; u1size < sizeof(Ascll_Info) / sizeof(GPS_ASCLLINFO); u1size++) {
				if (CMP_SUCCESS == memcmp(&pu1Buff[0], Ascll_Info[u1size].u1AscllName, Ascll_Info[u1size].u1sizeof)) {
					_switchState(Ascll_Info[u1size].u1Head);
				}
			}
		}
		//	if (pu1Buff[0] == '$' && pu1Buff[1] == 'G' && pu1Buff[4] == 'G' && pu1Buff[5] == 'A') {
		//		_switchState(RCV_GPGGA_HEAD);
		//	}
		//	else if (pu1Buff[0] == '$' && pu1Buff[1] == 'G' && pu1Buff[4] == 'R' && pu1Buff[5] == 'C') {
		//		_switchState(RCV_GPRMC_HEAD);
		//	}
		break;
	case RCV_GPGGA_HEAD:

		//printf("-----GPGGA------ \r\n");
		//printf("GPGGA:%s \r\n", pu1Buff);
		_GPGGA_Analysis(&gga_info, pu1Buff);	// GPGGA解析函数

		gpggaUpdated = 1;

		_ResetUartRcvBuff();

		if (u1Data == '$') {
			pblkUartRcv->u1State = RCV_STATE_START;
			pblkUartRcv->u1RcvBuff[pblkUartRcv->u1DataLen++] = u1Data;
		}
		break;
	case RCV_GPRMC_HEAD:

		//printf("-----GPRMC------ \r\n");
		//printf("GPRMC:%s \r\n", pu1Buff);
		_GPRMC_Analysis(&gmc_info, pu1Buff);	// GPRMC解析函数
		gprmcUpdated = 1;
		_ResetUartRcvBuff();
		break;
	default:
		break;
	}
END:
	return;
}

//int main()
//{
//	// $GPGGA,105547.00,3959.99990484,N,11559.73608378,E,1,10,0.9,555.1075,M,-9.2296,M,,*7A\r\n
//	// $GPRMC,105546.00,A,3959.99990614,N,11559.73608463,E,0.004,300.7,140622,5.7,W,A*22\r\n
//	u1 nema_gga[] = "$GPRMC,105546.000,A,3959.99990614,N,11559.73608463,E,0.004,300.7,140622,5.7,W,A*22\r\n $GPGGA,105547.00,3959.99990484,N,11559.73608378,E,1,10,0.9,555.1075,M,-9.2296,M,,*7A\r\n";
//
//	memset(&gmc_info, 0x00, sizeof(gmc_info));
//	int i = sizeof(nema_gga);
//
//	for (i = 0; i < sizeof(nema_gga); i++) {
//		GN_UartRcvGPSInfo(nema_gga[i]);
//	}
//
//	getchar();
//	return 0x00;
//}


int main()
{
	// 1. 定义串口参数
	const int baudRate = 38400;            // 设置波特率

	// 2. 打开串口
	HANDLE hSerial = CreateFile(
		comPort,				// 串口名称
		GENERIC_READ,          // 只读
		0,                     // 不共享
		NULL,                  // 默认安全属性
		OPEN_EXISTING,         // 打开已存在的设备
		0,                     // 普通文件属性
		NULL                   // 无模板
	);

	if (hSerial == INVALID_HANDLE_VALUE) {
		printf("错误：无法打开串口 %s\n");
		printf("请检查：\n");
		printf("1. 串口号是否正确（设备管理器中查看）\n");
		printf("2. 串口是否被其他程序占用\n");
		return 1;
	}

	// 3. 配置串口参数
	DCB dcb = { 0 };				//初始化结构体全为0
	dcb.DCBlength = sizeof(dcb);	//设置结构体长度

	if (GetCommState(hSerial, &dcb))
		printf("成功获取串口状态\n");
	else{
		printf("错误：获取串口状态失败\n");
		CloseHandle(hSerial);
		return 1;
	}

	dcb.BaudRate = baudRate;      // 波特率
	dcb.ByteSize = 8;             // 数据位
	dcb.StopBits = ONESTOPBIT;    // 1个停止位
	dcb.Parity = NOPARITY;        // 无校验位

	if (SetCommState(hSerial, &dcb)) 
		printf("成功设置串口参数\n");
	else{
		printf("错误：设置串口参数失败\n");
		CloseHandle(hSerial);
		return 1;
	}

	// 4. 设置超时（避免卡死）
	COMMTIMEOUTS timeouts = { 0 };
	timeouts.ReadIntervalTimeout = 50;      // 读取间隔超时:两个字节间隔超过50ms就停止
	timeouts.ReadTotalTimeoutConstant = 50; // 总读超时:从第一次读开始，超过50ms就停止

	if (!SetCommTimeouts(hSerial, &timeouts)) {
		printf("错误：设置超时失败\n");
	}

	// 5. 循环读取并处理数据
	printf("正在接收GPS数据（按任意键退出）...\n\n");

	char buffer[256];  // 接收缓冲区
	DWORD bytesRead;

	while (!_kbhit()) {  // 检测键盘是否有按键
		// 读取数据
		if (ReadFile(hSerial, buffer, sizeof(buffer) - 1, &bytesRead, NULL)) {	/* 参数1：串口句柄（从哪个串口读）;
																				   参数2：数据存到哪里；
																				   参数3：最多读多少字节（留一个给'\0'）；
																				   参数4：返回实际读了多少参数；
																				   参数5：异步模式（NULL表示同步等数据）*/
			if (bytesRead > 0) {
				buffer[bytesRead] = '\0';  // 添加字符串结束符

				//// 打印原始数据（可选）
				//printf("%s", buffer);
				//fflush(stdout);  // 立即显示

				// 逐个字符传递给GPS解析函数
				for (size_t i = 0; i < bytesRead; i++) {
					GN_UartRcvGPSInfo(buffer[i]);
				}
				if (gpggaUpdated) {
					printf("GPGGA 时间:%02d:%02d:%02d 纬度:%.5f%c 经度:%.5f%c 状态:%d 卫星:%d\n",
						gga_info.utc_time.hour, gga_info.utc_time.min, gga_info.utc_time.sec,
						gga_info.latitude_value, gga_info.latitude,
						gga_info.longtitude_value, gga_info.longitude,
						gga_info.gps_state, gga_info.sate_num);
					fflush(stdout);
					gpggaUpdated = 0;  // 重置标志
				}

				if (gprmcUpdated) {
					printf("GPRMC 时间:%02d:%02d:%02d 纬度:%.5f%c 经度:%.5f%c 速度:%.3f\n",
						gmc_info.utc_time.hour, gmc_info.utc_time.min, gmc_info.utc_time.sec,
						gmc_info.latitude_value, gmc_info.latitude,
						gmc_info.longtitude_value, gmc_info.longtitude,
						gmc_info.speed);
					fflush(stdout);
					gprmcUpdated = 0;  // 重置标志
				}

			}
		}
		else {
			printf("读取串口错误\n");
			break;
		}

		//Sleep(10);  // 短暂休眠避免CPU占用过高
	}

	// 6. 关闭串口
	CloseHandle(hSerial);
	printf("\n程序已退出\n");

	return 0;
}
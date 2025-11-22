#include <windows.h>
#include <stdio.h>
#include <conio.h>
#include "gps_info.h"
#include "GPS_Rcv.h"
#include "Distance.h"  
#include "GPSstr.h"

#define comPort L"COM3"  //串口号
#define baudRate 38400  //波特率



// 定义目标点
const double TARGET_LAT = 39.999999;   // 目标纬度（十进制度）
const double TARGET_LON = 115.999999;  // 目标经度（十进制度）

double lat = convertNMEAToDegrees(gga_info.latitude_value);
double lon = convertNMEAToDegrees(gga_info.longtitude_value);
double distance = calculateDistance(lat, lon, TARGET_LAT, TARGET_LON);

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
	// 1. 定义串口参数:设置串口号和波特率（放到宏定义了）

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
		printf("错误：无法打开串口 %s\n",comPort);
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
	else {
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
	else {
		printf("错误：设置串口参数失败\n");
		CloseHandle(hSerial);
		return 1;
	}

	// 4. 设置超时
	// 事件模式下，ReadFile超时可以设为0（立即返回）
	COMMTIMEOUTS timeouts = { 0 };
	timeouts.ReadIntervalTimeout = 0;
	timeouts.ReadTotalTimeoutConstant = 0;
	SetCommTimeouts(hSerial, &timeouts);

	// 5. 读取并处理数据
	
	// 关键：设置事件掩码
	DWORD dwCommMask = EV_RXCHAR;  // 只监听"数据到达"事件  触发条件：接收到字符并放入输入缓冲区（最常用）
	if (!SetCommMask(hSerial, EV_RXCHAR)) {
		printf("错误：设置事件掩码失败\n");
		CloseHandle(hSerial);
		return 1;
	}

	printf("正在接收GPS数据（事件驱动模式，按任意键退出）...\n\n");

	char buffer[256];

	// Double Word（双字节），是Windows定义的32位无符号整数类型
	DWORD bytesRead;
	DWORD dwEvent;  // 存储触发的事件

	//使用WaitCommEvent（事件驱动）
	while (!_kbhit()) {  // 检测键盘是否有按键
		// 读取数据
		if (WaitCommEvent(hSerial, &dwEvent, NULL)) {	//参数1：串口句柄；参数2：返回发生的事件；参数3：NULL表示同步操作
			if (dwEvent & EV_RXCHAR) {	//事件触发后，确认是数据到达事件
				do {
					if (ReadFile(hSerial, buffer, sizeof(buffer) - 1, &bytesRead, NULL)) {
						if (bytesRead > 0) {
							buffer[bytesRead] = '\0';

							for (size_t i = 0; i < bytesRead; i++) {
								GN_UartRcvGPSInfo(buffer[i]);
							}
							//if (gpggaUpdated) {
							//	printf("GNGGA 时间:%02d:%02d:%02d 纬度:%.5f%c 经度:%.5f%c 状态:%d 卫星:%d\n",
							//		gga_info.utc_time.hour, gga_info.utc_time.min, gga_info.utc_time.sec,
							//		gga_info.latitude_value, gga_info.latitude,
							//		gga_info.longtitude_value, gga_info.longitude,
							//		gga_info.gps_state, gga_info.sate_num);
							if (gpggaUpdated) {
								double lat = convertNMEAToDegrees(gga_info.latitude_value);
								double lon = convertNMEAToDegrees(gga_info.longtitude_value);
								double distance = calculateDistance(lat, lon, TARGET_LAT, TARGET_LON);
								printf("当前位置: %.5f, %.5f 距离目标: %.2f米\n", lat, lon, distance);
								// 到达判断
							if (isArrived(lat, lon,TARGET_LAT, TARGET_LON)) {
								printf("*** 已到达目的地 ***\n");
								}
								fflush(stdout);
								gpggaUpdated = 0;  // 重置标志
							}

							//if (gprmcUpdated) {
							//	printf("GNRMC 时间:%02d:%02d:%02d 纬度:%.5f%c 经度:%.5f%c 速度:%.3f\n",
							//		gmc_info.utc_time.hour, gmc_info.utc_time.min, gmc_info.utc_time.sec,
							//		gmc_info.latitude_value, gmc_info.latitude,
							//		gmc_info.longtitude_value, gmc_info.longtitude,
							//		gmc_info.speed);
							if (gprmcUpdated) {
								double lat = convertNMEAToDegrees(gmc_info.latitude_value);
								double lon = convertNMEAToDegrees(gmc_info.longtitude_value);
								double distance = calculateDistance(lat, lon, TARGET_LAT, TARGET_LON);
								printf("当前位置: %.5f, %.5f 距离目标: %.2f米\n", lat, lon, distance);
								// 到达判断
								if (isArrived(lat, lon,TARGET_LAT, TARGET_LON)) {
									printf("*** 已到达目的地 ***\n");
								}
								fflush(stdout);
								gprmcUpdated = 0;  // 重置标志
							}

						}
					}
				} while (bytesRead > 0); // 继续读取，直到没有数据
			}
		}
			else {
				printf("等待串口事件错误\n");
				break;
			}
	}
	// 6. 关闭串口
	CloseHandle(hSerial);
	printf("\n程序已退出\n");
	return 0;
}
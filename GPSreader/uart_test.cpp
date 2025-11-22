#include <windows.h>
#include <stdio.h>
#include <conio.h>
#include "gps_info.h"
#include "GPS_Rcv.h"

#define comPort L"COM3"  //串口号
#define baudRate 38400  //波特率

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
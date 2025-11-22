#ifndef _GPS_RCV_H_
#define _GPS_RCV_H_

#include "gps_info.h"

// 全局变量声明（外部链接）
extern BlkUartRcv blkUartRcv;
extern GPGGA_INFO gga_info;
extern GPRMC_INFO gmc_info;
extern u1 gpggaUpdated;
extern u1 gprmcUpdated;
extern GPS_ASCLLINFO Ascll_Info[2];

// 接收相关函数
void _ResetUartRcvBuff(void);
void GN_UartRcvGPSInfo(u1 u1Data);

#endif
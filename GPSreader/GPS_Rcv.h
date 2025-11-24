#ifndef _GPS_RCV_H_
#define _GPS_RCV_H_

#include "gps_info.h"
#include "WTRTK.h"

// 全局变量声明（外部链接）
extern BlkUartRcv blkUartRcv;
extern GPGGA_INFO gga_info;
extern GPRMC_INFO gmc_info;
extern u1 gpggaUpdated;
extern u1 gprmcUpdated;
extern GPS_ASCLLINFO Ascll_Info[3];

// 新增WTRTK全局变量
extern WTRTK_INFO wtrtk_info;
extern u1 wtrtkUpdated;

// 接收相关函数
void _ResetUartRcvBuff(void);
void GN_UartRcvGPSInfo(u1 u1Data);

#endif
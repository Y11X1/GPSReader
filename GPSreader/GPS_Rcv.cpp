#include "GPS_Rcv.h"
#include "GPRMC.h"
#include "GPGGA.h"
#include <string.h>
#include <stdio.h>

// 全局变量定义（这里分配内存）
BlkUartRcv blkUartRcv;
GPGGA_INFO gga_info;
GPRMC_INFO gmc_info;
u1 gpggaUpdated = 0;
u1 gprmcUpdated = 0;
GPS_ASCLLINFO Ascll_Info[2] = {
    ASCLL_DATA_GPGGA, 0x06, RCV_GPGGA_HEAD,
    ASCLL_DATA_GPRMC, 0x06, RCV_GPRMC_HEAD
};

// 重置接收缓冲区
void _ResetUartRcvBuff()
{
    memset(&blkUartRcv, 0, sizeof(BlkUartRcv));
}

// GPS接收状态机
void GN_UartRcvGPSInfo(u1 u1Data)   // 每收到一个字节就被调用一次
{
    BlkUartRcv* pblkUartRcv = &blkUartRcv;
    u1* pu1Buff = pblkUartRcv->u1RcvBuff;
    u1 u1size = 0;

    if ((u1Data == ASCLL_DATA_$) &&
        (pblkUartRcv->u1State == RCV_STATE_IDIE)) {
        pblkUartRcv->u1State = RCV_STATE_START;
    }

    if (pblkUartRcv->u1DataLen >= MAX_RCV_SIZE) {
        _ResetUartRcvBuff();
    }

    switch (pblkUartRcv->u1State) { // 根据"当前状态"决定做什么
    case RCV_STATE_START:
        pu1Buff[pblkUartRcv->u1DataLen++] = u1Data;
        if (u1Data == '\n') {        // 收到换行符 → 触发状态转换
            for (u1size = 0; u1size < sizeof(Ascll_Info) / sizeof(GPS_ASCLLINFO); u1size++) {
                if (CMP_SUCCESS == memcmp(&pu1Buff[0], Ascll_Info[u1size].u1AscllName, Ascll_Info[u1size].u1sizeof)) {
                    _switchState(Ascll_Info[u1size].u1Head);
                }
            }
        }
        break;
    case RCV_GPGGA_HEAD:
        _GPGGA_Analysis(&gga_info, pu1Buff);
        gpggaUpdated = 1;
        _ResetUartRcvBuff();
        if (u1Data == '$') {
            pblkUartRcv->u1State = RCV_STATE_START;
            pblkUartRcv->u1RcvBuff[pblkUartRcv->u1DataLen++] = u1Data;
        }
        break;
    case RCV_GPRMC_HEAD:
        _GPRMC_Analysis(&gmc_info, pu1Buff);
        gprmcUpdated = 1;
        _ResetUartRcvBuff();
        break;
    default:
        break;
    }
}
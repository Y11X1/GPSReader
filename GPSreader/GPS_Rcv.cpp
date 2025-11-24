#include "GPS_Rcv.h"
#include "GPRMC.h"
#include "GPGGA.h"
#include "WTRTK.h"
#include <string.h>
#include <stdio.h>

// 全局变量定义（这里分配内存）
BlkUartRcv blkUartRcv;
GPGGA_INFO gga_info;
GPRMC_INFO gmc_info;
u1 gpggaUpdated = 0;
u1 gprmcUpdated = 0;
GPS_ASCLLINFO Ascll_Info[3] = {
    "$GNGGA", 0x06, RCV_GPGGA_HEAD,
    "$GNRMC", 0x06, RCV_GPRMC_HEAD,
    "$WTRTK", 0x06, RCV_WTRTK_HEAD
};

// 新增WTRTK全局变量
WTRTK_INFO wtrtk_info;
u1 wtrtkUpdated = 0;

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

    if ((u1Data == '$') &&
        (pblkUartRcv->u1State == RCV_STATE_IDIE)) {
        pblkUartRcv->u1State = RCV_STATE_START;
    }

    if (pblkUartRcv->u1DataLen >= MAX_RCV_SIZE) {
        _ResetUartRcvBuff();
    }

    switch (pblkUartRcv->u1State) { // 根据"当前状态"决定做什么

    case RCV_STATE_START:
        pu1Buff[pblkUartRcv->u1DataLen++] = u1Data;
        // 检查是否收到足够的字符来识别消息类型
        if (pblkUartRcv->u1DataLen >= 6) {  // 至少需要6个字符来识别"$WTRTK"
            // 检查WTRTK（优先检查，因为它最长）
            if (memcmp(pu1Buff, "$WTRTK", 6) == 0) {
                pblkUartRcv->u1State = RCV_WTRTK_HEAD;
            }
            // 检查GNGGA
            else if (memcmp(pu1Buff, "$GNGGA", 6) == 0) {
                pblkUartRcv->u1State = RCV_GPGGA_HEAD;
            }
            // 检查GNRMC
            else if (memcmp(pu1Buff, "$GNRMC", 6) == 0) {
                pblkUartRcv->u1State = RCV_GPRMC_HEAD;
            }
        }

        // 如果收到换行符但还没有识别消息类型，重置
        if (u1Data == '\n') {
            if (pblkUartRcv->u1State == RCV_STATE_START) {
                _ResetUartRcvBuff();
            }
        }
        break;

    case RCV_GPGGA_HEAD:
        pu1Buff[pblkUartRcv->u1DataLen++] = u1Data;
        if (u1Data == '\n') {
            _GPGGA_Analysis(&gga_info, pu1Buff);
            gpggaUpdated = 1;
            _ResetUartRcvBuff();
        }
        else if (u1Data == '$') {
            // 新消息开始
            _ResetUartRcvBuff();
            pblkUartRcv->u1State = RCV_STATE_START;
            pblkUartRcv->u1RcvBuff[pblkUartRcv->u1DataLen++] = u1Data;
        }
        break;

    case RCV_GPRMC_HEAD:
        pu1Buff[pblkUartRcv->u1DataLen++] = u1Data;
        if (u1Data == '\n') {
            _GPRMC_Analysis(&gmc_info, pu1Buff);
            gprmcUpdated = 1;
            _ResetUartRcvBuff();
        }
        else if (u1Data == '$') {
            _ResetUartRcvBuff();
            pblkUartRcv->u1State = RCV_STATE_START;
            pblkUartRcv->u1RcvBuff[pblkUartRcv->u1DataLen++] = u1Data;
        }
        break;

    case RCV_WTRTK_HEAD:  // 新增状态

        pu1Buff[pblkUartRcv->u1DataLen++] = u1Data;
        if (u1Data == '\n') {
            _WTRTK_Analysis(&wtrtk_info, pu1Buff);
            wtrtkUpdated = 1;
            _ResetUartRcvBuff();
        }
        else if (u1Data == '$') {
            // 新消息开始
            _ResetUartRcvBuff();
            pblkUartRcv->u1State = RCV_STATE_START;
            pblkUartRcv->u1RcvBuff[pblkUartRcv->u1DataLen++] = u1Data;
        }
        break;

    default:
        _ResetUartRcvBuff();
        break;
    }
}
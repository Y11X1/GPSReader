#ifndef _GN_GPSINFO_H__
#define _GN_GPSINFO_H__

typedef   int				i4;
typedef   unsigned int		u4;
typedef   short             i2;
typedef   unsigned short    u2;
typedef   signed char		i1;
typedef   unsigned char     u1;
typedef   float             f4;
typedef   double			f8;


#define  ASCLL_DATA_$           '$'
#define  ASCLL_DATA_GPGGA       "$GNGGA"
#define  ASCLL_DATA_GPRMC       "$GNRMC"


#define  MAX_RCV_SIZE			1024*5	// 最大接收缓冲区 5KB
#define  GPGGA_HEAD_LEN			0x06	// GGA头长度6字节
#define  CMP_SUCCESS			0x00	// 比较成功
#define  CMP_ERROR				0x01	 // 比较失败

#define     _switchState(a)         pblkUartRcv->u1State = a

enum
{
	RCV_STATE_IDIE = 0,	 // 空闲状态（没收到数据）
	RCV_STATE_START,	// 收到$符号
	RCV_GPGGA_HEAD,		// 正在识别GGA头部
	RCV_GPRMC_HEAD		// 正在识别RMC头部		//对应的是接受过程的四个状态
};

typedef struct
{
	u1	u1AscllName[10];	 // 语句名称，如"$GNGGA"
	u1	u1sizeof;			// 长度
	u1	u1Head;				// 对应状态（如RCV_GPGGA_HEAD）
}GPS_ASCLLINFO;
typedef struct UartRcvTag
{
	u1 u1RcvBuff[MAX_RCV_SIZE];	// 5KB大缓冲区
	u1 u1State = 0;			   // 当前状态（默认IDIE）
	u1 u1DataLen;				// 已接收数据长度
	u1 u1Len;					// 预期数据长度（可能不用）
}BlkUartRcv;

//UTC时间信息
typedef struct
{
	u2 year;	//年
	u1 month;	//月
	u1 date;	//日
	u1 hour; 	//小时
	u1 min; 	//分钟
	u1 sec; 	//秒钟
	u1 ssec;	//毫秒
}gps_utc_time;

typedef struct
{
	gps_utc_time	utc_time;			// UTC时间
	f8	latitude_value;					// 纬度
	u1	latitude;						// 纬度半球
	f8	longtitude_value;				// 经度
	u1	longitude;						// 经度半球
	u1	gps_state;						// GPS状态 0=未定位，1=非差分定位，2=差分定位，6=正在估算
	u1	sate_num;						// 解算位置卫星数量
	f4	hdop;							// HDOP水平精度因子
	f4	altitude;						// 海拔高度
}GPGGA_INFO;



typedef struct
{
	gps_utc_time utc_time;		// UTC时间
	u1 gps_state;				// 定位状态
	f8 latitude_value;			// 纬度
	u1 latitude;				// 纬度半球
	f8 longtitude_value;		// 经度
	u1 longtitude;				// 经度半球
	f4 speed;					// 地面速率
	f4 azimuth_angle;			// 地面航向

}GPRMC_INFO;

int _Str2num(u1* buf, u1* dx);	//字符串转数字函数

void _GPRMC_Analysis(GPRMC_INFO* gps_rmc_info, u1* buf);	//解析GPRMC信息函数
void _GPGGA_Analysis(GPGGA_INFO* gps_gga_info, u1* buf);	//解析GPGGA信息函数

#endif


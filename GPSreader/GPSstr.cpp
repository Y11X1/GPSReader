#include "GPSstr.h"
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
	return (u1)(buf - p);
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

double convertNMEAToDegrees(double nmeaValue)
{
	int degrees = (int)(nmeaValue / 100);      // 提取度部分
	double minutes = nmeaValue - degrees * 100; // 提取分部分
	return degrees + minutes / 60.0;           // 转换为十进制度
}
#ifndef _GPS_STR_H_
#define _GPS_STR_H_

#include "gps_info.h"
#include <string.h> 

// ×Ö·û´®¹¤¾ßº¯Êý
u1 _Find_Pos(u1* buf, u1 u1Num);
u4 _Pow(u1 m, u1 n);
int _Str2num(u1* buf, u1* dx);

double convertNMEAToDegrees(double nmeaValue);
#endif

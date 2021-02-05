//
// Created by ka on 2021/1/13.
//

#include "TimeStamp.h"
#include <chrono>


//将时间转换为字符串形式方便打印或者输出
string Timestamp::ToString()
{
    char buf[64] = {0};
    int64_t seconds = m_microSecondsSinceEpoch / MicroSecondsPerSecond;
    int64_t microseconds = m_microSecondsSinceEpoch % MicroSecondsPerSecond;
    snprintf(buf,sizeof(buf)-1,"%lld.%06lld",(long long int)seconds,(long long int)microseconds);
    return buf;
}

//根据c++11标准的时间戳获取到当前的时间
Timestamp Timestamp::now()
{
    chrono::time_point<chrono::system_clock,chrono::microseconds> now = chrono::time_point_cast<chrono::microseconds>(
            chrono::system_clock::now());

    int64_t microSeconds = now.time_since_epoch().count();

    Timestamp time(microSeconds);
    return time;
}

//获取一个无效的时间
Timestamp Timestamp::invalid()
{
    return Timestamp();
}

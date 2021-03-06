/*************************************************************************
> File Name: Timestamp.cpp
> Author: pengjunjie
> Mail: 1002398145@qq.com
> Created Time: 2018年12月23日 星期日 10时51分31秒
************************************************************************/

#include "Timestamp.h"

#include <stdio.h>
#include <sys/time.h>
#include <unistd.h>
#include <inttypes.h>

using namespace pallette;

std::string Timestamp::toString() const
{
    char buf[32] = { 0 };
    int64_t seconds = microSeconds_ / kMicroSecondsPerSecond;
    int64_t microSeconds = microSeconds_ % kMicroSecondsPerSecond;
    //PRId64表示64位整数,主要是为了同时支持32位和64位操作系统
    snprintf(buf, sizeof(buf) - 1, "%" PRId64 ".%06" PRId64 "", seconds, microSeconds);
    return buf;
}

std::string Timestamp::toFormatTedString(bool showMicroSeconds) const
{
    char buf[64] = { 0 };
    time_t seconds = static_cast<time_t>(microSeconds_ / kMicroSecondsPerSecond);
    struct tm tm_time;
    gmtime_r(&seconds, &tm_time);    //gmtime_r是gmtime的线程安全版

    if (showMicroSeconds)
    {
        int microSeconds = static_cast<int>(microSeconds_ % kMicroSecondsPerSecond);
        snprintf(buf, sizeof(buf), "%4d-%02d-%02d %02d:%02d:%02d.%03d",
            tm_time.tm_year + 1900, tm_time.tm_mon + 1, tm_time.tm_mday,
            tm_time.tm_hour, tm_time.tm_min, tm_time.tm_sec, microSeconds);
    }
    else
    {
        snprintf(buf, sizeof(buf), "%4d-%02d-%02d %02d:%02d:%02d",
            tm_time.tm_year + 1900, tm_time.tm_mon + 1, tm_time.tm_mday,
            tm_time.tm_hour, tm_time.tm_min, tm_time.tm_sec);
    }

    return buf;
}

Timestamp Timestamp::now()
{
    struct timeval tv;
    gettimeofday(&tv, NULL);
    int64_t seconds = tv.tv_sec;
    return Timestamp(seconds * kMicroSecondsPerSecond + tv.tv_usec);
}

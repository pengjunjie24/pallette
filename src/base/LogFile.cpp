/*************************************************************************
> File Name: LogFile.cpp
> Author: pengjunjie
> Mail: 1002398145@qq.com
> Created Time: 2018年12月31日 星期一 11时13分42秒
************************************************************************/

#include "LogFile.h"

#include "FileUtil.h"
#include "ProcessInfo.h"

#include <stdio.h>

using namespace pallette;

LogFile::LogFile(const std::string& basename, int rollSize,
    int flushInterval, int checkEveryN)
    : basename_(basename)
    , rollSize_(rollSize)
    , flushInterval_(flushInterval)
    , checkEveryN_(checkEveryN)
    , startOfPeriod_(0)
    , lastRoll_(0)
    , lastFlush_(0)
{
    rollFile();
}
LogFile::~LogFile()
{
}

void LogFile::append(const char* logline, int len)
{
    std::unique_lock<std::mutex> lock(mutex_);
    file_->append(logline, len);
    if (file_->writtenBytes() > rollSize_)
    {
        rollFile();
    }
    else
    {
        ++count_;
        if (count_ >= checkEveryN_)
        {
            count_ = 0;
            time_t now = ::time(NULL);
            time_t thisPeriod_ = now / kRollPerSeconds_ * kRollPerSeconds_;
            if (thisPeriod_ != startOfPeriod_)//天数改变了
            {
                rollFile();
            }
            else if (now - lastFlush_ > flushInterval_)
            {
                lastFlush_ = now;
                file_->flush();
            }
        }
    }

}
void LogFile::flush()
{
    std::unique_lock<std::mutex> lock(mutex_);
    file_->flush();
}

bool LogFile::rollFile()
{
    time_t now = 0;
    std::string filename = getLogFileName(basename_, &now);
    time_t start = now / kRollPerSeconds_ * kRollPerSeconds_;//转换为天为单位

    if (now > lastRoll_)
    {
        lastRoll_ = now;
        lastFlush_ = now;
        startOfPeriod_ = start;
        file_.reset(new file_util::AppendFile(filename));
        return true;
    }
    return false;
}

std::string LogFile::getLogFileName(const std::string& basename, time_t* now)
{
    std::string filename;
    filename.reserve(basename.size() + 64);
    filename = basename;
    if (filename.at(filename.size() - 1) != '/')
    {
        filename += '/';
    }

    char timebuf[32] = { 0 };
    struct tm nowTime;
    *now = time(NULL);
    gmtime_r(now, &nowTime);
    strftime(timebuf, sizeof(timebuf), "%Y%m%d-%H%M%S_", &nowTime);
    filename += timebuf;

    filename += process_info::processname();

    char pidbuf[32] = { 0 };
    snprintf(pidbuf, sizeof(pidbuf), "_%d", process_info::pid());
    filename += pidbuf;
    filename += ".log";

    return filename;
}
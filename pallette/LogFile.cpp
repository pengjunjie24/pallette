/*************************************************************************
> File Name: LogFile.cpp
> Author: pengjunjie
> Mail: 1002398145@qq.com
> Created Time: 2018年12月31日 星期一 11时13分42秒
************************************************************************/

#include <pallette/LogFile.h>

#include <pallette/FileUtil.h>
#include <pallette/CurrentThread.h>
#include <pallette/ProcessInfo.h>

#include <stdio.h>

using namespace pallette;

LogFile::LogFile(const std::string& basename, int rollSize,
    bool threadSafe, int flushInterval, int checkEveryN)
    : basename_(basename)
    , rollSize_(rollSize)
    , flushInterval_(flushInterval)
    , checkEveryN_(checkEveryN)
    , count_(0)
    , mutex_(threadSafe ? new std::mutex : NULL)
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
    if (mutex_)
    {
        std::lock_guard<std::mutex> lock(*mutex_);
        appendUnloked(logline, len);
    }
    else
    {
        appendUnloked(logline, len);
    }


}
void LogFile::flush()
{
    if (mutex_)
    {
        std::lock_guard<std::mutex> lock(*mutex_);
        file_->flush();
    }
    else
    {
        file_->flush();
    }
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

void LogFile::appendUnloked(const char* logline, int len)
{
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

std::string LogFile::getLogFileName(const std::string& basename, time_t* now)
{
    std::string filename;
    filename.reserve(basename.size() + 64);
    filename = basename;

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
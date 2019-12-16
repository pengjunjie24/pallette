/*************************************************************************
> File Name: LogFile.h
> Author: pengjunjie
> Mail: 1002398145@qq.com
> Created Time: 2018年12月31日 星期一 11时13分29秒
************************************************************************/

#ifndef PALLETTE_LOGFILE_H
#define PALLETTE_LOGFILE_H

#include <pallette/noncopyable.h>
#include <string>
#include <mutex>
#include <memory>
#include <time.h>

namespace pallette
{
    namespace file_util
    {
        class AppendFile;
    }
    //日志文件的设置类，正常情况下日志的滚动按照天数来滚动的，2种情况下会检测日志是否滚动
    //1.写入字符大于rollSize_(最大可写入字符)
    //2.当行数为checkEveryN_的倍数时, 进行检测天数是否改变(当第1个条件满足后就不会检测第2个条件了)
    class LogFile : noncopyable
    {
    public:
        LogFile(const std::string& basename,
            int rollSize,
            bool threadSafe = true,
            int flushInterval = 3,
            int checkEveryN = 1024);
        ~LogFile();

        void append(const char* logline, int len);
        void flush();//内存中的日志刷新到文件
        bool rollFile();//日志滚动

    private:
        void appendUnloked(const char* logline, int len);
        static std::string getLogFileName(const std::string& basename, time_t* now);

        const std::string basename_;//文件名称
        const int rollSize_;//判断日志是否需要滚动的长度
        const int flushInterval_;//日志写入时间间隔
        const int checkEveryN_;//日志条数checkEveryN_倍数时进行检测
        int count_;//记录日志条数
        std::unique_ptr <std::mutex> mutex_;
        time_t startOfPeriod_;//开始记录日志时间(一般为每天0点)
        time_t lastRoll_;//上次滚动日志文件时间
        time_t lastFlush_;//上次日志写入文件时间
        std::unique_ptr<file_util::AppendFile>file_;//操作文件的对象

        const static int kRollPerSeconds_ = 60 * 60 * 24;//一天经过的秒数
    };
}

#endif
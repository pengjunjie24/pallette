/*************************************************************************
  > File Name: Logger.h
  > Author: pengjunjie
  > Mail: 1002398145@qq.com
  > Created Time: 2018年12月23日 星期日 10时22分11秒
 ************************************************************************/

#ifndef PALLETTE_Logger_H
#define PALLETTE_Logger_H

#include <pallette/Timestamp.h>
#include <pallette/LogStream.h>

namespace pallette
{
    class TimeZone;

    class Logger
    {
    public:
        enum ENUM_LOGLEVEL
        {
            TRACE,
            DEBUG,
            INFO,
            WARN,
            ERROR,
            FATAL,
            NUM_LOG_LEVELS,
        };

        typedef void(*OutputFunc)(const char*, int);//日志输出函数
        typedef void(*FlushFunc)();//日志刷新函数

        Logger(const char*, int, ENUM_LOGLEVEL, const char*, bool systemError = false);
        ~Logger();

        std::string formatTime();//格式化时间
        LogStream& stream();//返回日志缓冲，向缓冲写入日志

        static ENUM_LOGLEVEL logLevel();//日志打印等级
        static void setLogLevel(ENUM_LOGLEVEL);//设置日志打印等级
        static void setOutput(OutputFunc);//设置日志输出函数
        static void setFlush(FlushFunc);//设置日志刷新函数
        static void setTimeZone(const TimeZone& tz);//设置时区

    private:
        Timestamp time_;
        ENUM_LOGLEVEL level_;
        int line_;
        const char* func_;
        std::string basename_;
        LogStream logBuffer_;//日志流
    };

    extern Logger::ENUM_LOGLEVEL gLogLevel;
    inline Logger::ENUM_LOGLEVEL Logger::logLevel(){ return gLogLevel; }

    const char* strerror_rl(int saveErrno);//对strerror_r的封装

#define LOG_TRACE if(pallette::Logger::logLevel() <= pallette::Logger::TRACE) \
    pallette::Logger(__FILE__, __LINE__, pallette::Logger::TRACE, __func__).stream()//使用匿名对象，调用完后就析构，不会使日志反序

#define LOG_DEBUG if(pallette::Logger::logLevel() <= pallette::Logger::DEBUG) \
    pallette::Logger(__FILE__, __LINE__, pallette::Logger::DEBUG, __func__).stream()

#define LOG_INFO if(pallette::Logger::logLevel() <= pallette::Logger::INFO) \
    pallette::Logger(__FILE__, __LINE__, pallette::Logger::INFO, __func__).stream()

#define LOG_WARN if(pallette::Logger::logLevel() <= pallette::Logger::WARN) \
    pallette::Logger(__FILE__, __LINE__, pallette::Logger::WARN, __func__).stream()

#define LOG_ERROR if(pallette::Logger::logLevel() <= pallette::Logger::ERROR) \
    pallette::Logger(__FILE__, __LINE__, pallette::Logger::ERROR, __func__).stream()

#define LOG_FATAL if(pallette::Logger::logLevel() <= pallette::Logger::FATAL) \
    pallette::Logger(__FILE__, __LINE__, pallette::Logger::FATAL, __func__).stream()

#define LOG_SYSERR if(pallette::Logger::logLevel() <= pallette::Logger::ERROR) \
    pallette::Logger(__FILE__, __LINE__, pallette::Logger::ERROR, __func__, true).stream()

#define LOG_SYSFATAL if(pallette::Logger::logLevel() <= pallette::Logger::FATAL) \
    pallette::Logger(__FILE__, __LINE__, pallette::Logger::FATAL, __func__, true).stream()
}

#endif
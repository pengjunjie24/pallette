/*************************************************************************
  > File Name: Logging.h
  > Author: pengjunjie
  > Mail: 1002398145@qq.com
  > Created Time: 2018年12月23日 星期日 10时22分11秒
 ************************************************************************/

#ifndef PALLETTE_LOGGING_H
#define PALLETTE_LOGGING_H

#include "Timestamp.h"
#include "LogStream.h"

namespace pallette
{
    class Logging
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

        Logging(const char*, int, ENUM_LOGLEVEL, const char*, bool systemError = false);
        ~Logging();

        LogStream& stream();//返回日志缓冲，向缓冲写入日志

        static ENUM_LOGLEVEL logLevel();//日志打印等级
        static void setLogLevel(ENUM_LOGLEVEL);//设置日志打印等级
        static void setOutput(OutputFunc);//设置日志输出函数
        static void setFlush(FlushFunc);//设置日志刷新函数

    private:
        Timestamp time_;
        ENUM_LOGLEVEL level_;
        int line_;
        const char* func_;
        std::string basename_;
        LogStream logBuffer_;//日志流
    };

    extern Logging::ENUM_LOGLEVEL gLogLevel;
    inline Logging::ENUM_LOGLEVEL Logging::logLevel(){ return gLogLevel; }

    const char* strerror_rl(int saveErrno);//对strerror_r的封装

#define LOG_TRACE if(pallette::Logging::logLevel() <= pallette::Logging::TRACE) \
    pallette::Logging(__FILE__, __LINE__, pallette::Logging::TRACE, __func__).stream()//使用匿名对象，调用完后就析构，不会使日志反序

#define LOG_DEBUG if(pallette::Logging::logLevel() <= pallette::Logging::DEBUG) \
    pallette::Logging(__FILE__, __LINE__, pallette::Logging::DEBUG, __func__).stream()

#define LOG_INFO if(pallette::Logging::logLevel() <= pallette::Logging::INFO) \
    pallette::Logging(__FILE__, __LINE__, pallette::Logging::INFO, __func__).stream()

#define LOG_WARN if(pallette::Logging::logLevel() <= pallette::Logging::WARN) \
    pallette::Logging(__FILE__, __LINE__, pallette::Logging::WARN, __func__).stream()

#define LOG_ERROR if(pallette::Logging::logLevel() <= pallette::Logging::ERROR) \
    pallette::Logging(__FILE__, __LINE__, pallette::Logging::ERROR, __func__).stream()

#define LOG_FATAL if(pallette::Logging::logLevel() <= pallette::Logging::FATAL) \
    pallette::Logging(__FILE__, __LINE__, pallette::Logging::FATAL, __func__).stream()

#define LOG_SYSERR if(pallette::Logging::logLevel() <= pallette::Logging::ERROR) \
    pallette::Logging(__FILE__, __LINE__, pallette::Logging::ERROR, __func__, true).stream()

#define LOG_SYSFATAL if(pallette::Logging::logLevel() <= pallette::Logging::FATAL) \
    pallette::Logging(__FILE__, __LINE__, pallette::Logging::FATAL, __func__, true).stream()
}

#endif
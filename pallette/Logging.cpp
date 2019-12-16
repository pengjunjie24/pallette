/*************************************************************************
  > File Name: Logger.cpp
  > Author: pengjunjie
  > Mail: 1002398145@qq.com
  > Created Time: 2018年12月23日 星期日 10时22分18秒
 ************************************************************************/

#include <pallette/Logging.h>

#include <pallette/CurrentThread.h>
#include <pallette/Timestamp.h>
#include <pallette/TimeZone.h>
#include <pallette/Strings.h>

#include <string.h>
#include <stdio.h>
#include <stdlib.h>
#include <assert.h>
#include <errno.h>

namespace pallette
{
    const char* LogLevelName[Logger::NUM_LOG_LEVELS] =
    {
        "TRACE",
        "DEBUG",
        "INFO",
        "WARN",
        "ERROR",
        "FATAL",
    };

    Logger::ENUM_LOGLEVEL InitLogLevel()
    {
        if (::getenv("BASE_LOG_TRACE"))
        {
            return Logger::TRACE;
        }
        else if (::getenv("BASE_LOG_DEBUG"))
        {
            return Logger::DEBUG;
        }
        else
        {
            return Logger::INFO;
        }
    }

    void DefaultOutput(const char* msg, int len)
    {
        size_t n __attribute__((unused)) = fwrite(msg, 1, len, stdout);
        assert(static_cast<int>(n) == len);
    }

    void DefaultFulsh()
    {
        fflush(stdout);
    }

    Logger::ENUM_LOGLEVEL gLogLevel = InitLogLevel();
    Logger::OutputFunc gOutput = DefaultOutput;
    Logger::FlushFunc gFlush = DefaultFulsh;
    TimeZone gLogTimeZone;

    __thread time_t tLastSecond;
    __thread char tTime[32];
    __thread char tErrnobuf[512];

    const char* strerror_rl(int saveErrno)
    {
        return strerror_r(saveErrno, tErrnobuf, sizeof(tErrnobuf));
    }
}

using namespace pallette;
Logger::Logger(const char* file, int line, ENUM_LOGLEVEL level, const char* func, bool systemError)
    : time_(Timestamp::now())
    , level_(level)
    , line_(line)
    , func_(func)
    , basename_(file)
{
    int saveError = errno;
    const char* slash = strrchr(file, '/');
    if (slash)
    {
        basename_.assign(slash + 1);
    }

    logBuffer_ << formatTime() << " [" << LogLevelName[level_] << "]"
        << " [" << current_thread::tid() << "] " << basename_ << " " << func_ << " "
        << line_ << ": {";

    const LogStream::Buffer& buf(logBuffer_.getBuffer());
    if (systemError && saveError != 0)
    {
        logBuffer_ << " (errno=" << saveError << "  " << strerror_rl(saveError) << ") ";
    }
}
Logger:: ~Logger()
{
    logBuffer_ << "}\n";

    const LogStream::Buffer& buf(stream().getBuffer());
    gOutput(buf.data(), buf.length());

    if (level_ == FATAL)
    {
        gFlush();
        abort();
    }
}

std::string Logger::formatTime()
{
    int64_t microSecondsSinceEpoch = time_.getMicroSeconds();
    time_t seconds = static_cast<time_t>(microSecondsSinceEpoch / Timestamp::kMicroSecondsPerSecond);
    int microseconds = static_cast<int>(microSecondsSinceEpoch % Timestamp::kMicroSecondsPerSecond);
    if (seconds != tLastSecond)//秒数不同时，修改所有时间字符串，否则只修改秒之后的字符串
    {
        tLastSecond = seconds;
        struct tm tm_time;
        if (gLogTimeZone.valid())
        {
            tm_time = gLogTimeZone.toLocalTime(seconds);
        }
        else
        {
            ::gmtime_r(&seconds, &tm_time); // FIXME TimeZone::fromUtcTime
        }

        int len __attribute__((unused)) = snprintf(tTime, sizeof(tTime), "%4d%02d%02d %02d:%02d:%02d",
            tm_time.tm_year + 1900, tm_time.tm_mon + 1, tm_time.tm_mday,
            tm_time.tm_hour, tm_time.tm_min, tm_time.tm_sec);
        assert(len == 17);
    }

    std::string ftime = "";
    if (gLogTimeZone.valid())
    {
        utils::stringFormat(ftime, "%s.%06d", tTime, microseconds);
    }
    else
    {
        utils::stringFormat(ftime, "%s.%06dZ", tTime, microseconds);
    }
    return ftime;
}

void Logger::setLogLevel(Logger::ENUM_LOGLEVEL level)
{
    gLogLevel = level;
}

void Logger::setOutput(OutputFunc out)
{
    gOutput = out;
}

void Logger::setFlush(FlushFunc flush)
{
    gFlush = flush;
}

void Logger::setTimeZone(const TimeZone& tz)
{
    gLogTimeZone = tz;
}

LogStream& Logger::stream()
{
    return logBuffer_;
}
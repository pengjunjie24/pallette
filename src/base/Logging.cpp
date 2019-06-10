/*************************************************************************
  > File Name: Logging.cpp
  > Author: pengjunjie
  > Mail: 1002398145@qq.com
  > Created Time: 2018年12月23日 星期日 10时22分18秒
 ************************************************************************/

#include "Logging.h"
#include "ProcessInfo.h"

#include <string.h>
#include <stdio.h>
#include <stdlib.h>
#include <assert.h>
#include <errno.h>

namespace pallette
{
    const char* LogLevelName[Logging::NUM_LOG_LEVELS] =
    {
        "TRACE",
        "DEBUG",
        "INFO",
        "WARN",
        "ERROR",
        "FATAL",
    };

    Logging::ENUM_LOGLEVEL InitLogLevel()
    {
        if (::getenv("BASE_LOG_TRACE"))
        {
            return Logging::TRACE;
        }
        else if (::getenv("BASE_LOG_DEBUG"))
        {
            return Logging::DEBUG;
        }
        else
        {
            return Logging::INFO;
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

    Logging::ENUM_LOGLEVEL gLogLevel = InitLogLevel();
    Logging::OutputFunc gOutput = DefaultOutput;
    Logging::FlushFunc gFlush = DefaultFulsh;

    const char* strerror_rl(int saveErrno)
    {
        char error_buf[512] = { 0 };
        return strerror_r(saveErrno, error_buf, sizeof(error_buf));
    }
}

using namespace pallette;
Logging::Logging(const char* file, int line, ENUM_LOGLEVEL level, const char* func, bool systemError)
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

    logBuffer_ << time_.toFormatTedString(true) << " [" << LogLevelName[level_] << "]"
        << " [" << process_info::tid() << "] " << basename_ << " " << func_ << " "
        << line_ << ": {";

    if (systemError && saveError != 0)
    {
        logBuffer_ << " (errno=" << saveError << "  " << strerror_rl(saveError) << ") ";
    }
}
Logging:: ~Logging()
{
    logBuffer_ << "}\n";

    const LogStream::Buffer& buf(stream().getBuffer());
    gOutput(buf.getString().c_str(), buf.getString().length());
    if (level_ == FATAL)
    {
        gFlush();
        abort();
    }
}

void Logging::setLogLevel(Logging::ENUM_LOGLEVEL level)
{
    gLogLevel = level;
}

void Logging::setOutput(OutputFunc out)
{
    gOutput = out;
}

void Logging::setFlush(FlushFunc flush)
{
    gFlush = flush;
}

LogStream& Logging::stream()
{
    return logBuffer_;
}
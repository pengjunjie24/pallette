/*************************************************************************
  > File Name: ProcessInspector.cpp
  > Author: pengjunjie
  > Mail: 1002398145@qq.com
  > Created Time: 2019年07月04日 星期四 11时13分06秒
 ************************************************************************/

#include "ProcessInspector.h"

#include "../ProcessInfo.h"
#include "../FileUtil.h"
#include "../Strings.h"

#include <string.h>
#include <stdarg.h>
#include <limits.h>

using namespace pallette;

namespace pallette
{
    std::string uptime(Timestamp now, Timestamp start, bool showMicroseconds)
    {
        char buf[256];
        int64_t age = now.getMicroSeconds() - start.getMicroSeconds();
        int seconds = static_cast<int>(age / Timestamp::kMicroSecondsPerSecond);
        int days = seconds / 86400;
        int hours = (seconds % 86400) / 3600;
        int minutes = (seconds % 3600) / 60;
        if (showMicroseconds)
        {
            int microseconds = static_cast<int>(age % Timestamp::kMicroSecondsPerSecond);
            snprintf(buf, sizeof buf, "%d days %02d:%02d:%02d.%06d",
                days, hours, minutes, seconds % 60, microseconds);
        }
        else
        {
            snprintf(buf, sizeof buf, "%d days %02d:%02d:%02d",
                days, hours, minutes, seconds % 60);
        }
        return buf;
    }

    long getLong(const std::string& procStatus, const char* key)
    {
        long result = 0;
        size_t pos = procStatus.find(key);
        if (pos != std::string::npos)
        {
            result = ::atol(procStatus.c_str() + pos + strlen(key));
        }
        return result;
    }

    std::string getProcessName(const std::string& procStatus)
    {
        std::string result;
        size_t pos = procStatus.find("Name:");
        if (pos != std::string::npos)
        {
            pos += strlen("Name:");
            while (procStatus[pos] == '\t')
            {
                ++pos;
            }
            size_t eol = pos;
            while (procStatus[eol] != '\n')
            {
                ++eol;
            }
            result = procStatus.substr(pos, eol - pos);
        }
        return result;
    }

    process_info::CpuTime getCpuTime(const std::vector<std::string>& vecStat)
    {
        process_info::CpuTime t;

        long utime = strtol(vecStat[13].c_str(), NULL, 10);
        long stime = strtol(vecStat[14].c_str(), NULL, 10);
        const double hz = static_cast<double>(process_info::clockTicksPerSecond());
        t.userSeconds = static_cast<double>(utime) / hz;
        t.systemSeconds = static_cast<double>(stime) / hz;
        return t;
    }

    int stringPrintf(std::string* out, const char* fmt, ...) __attribute__((format(printf, 2, 3)));
    int stringPrintf(std::string* out, const char* fmt, ...)
    {
        char buf[256];
        va_list args;
        va_start(args, fmt);
        int ret = vsnprintf(buf, sizeof buf, fmt, args);
        va_end(args);
        out->append(buf);
        return ret;
    }

}

std::string ProcessInspector::username_ = process_info::username();

void ProcessInspector::registerCommands(Inspector* ins)
{
    ins->add("proc", "overview", ProcessInspector::overview, "print basic overview");
    ins->add("proc", "pid", ProcessInspector::pid, "print pid");
    ins->add("proc", "status", ProcessInspector::procStatus, "print /proc/self/status");
    ins->add("proc", "threads", ProcessInspector::threads, "list /proc/self/task");
}

std::string ProcessInspector::overview(HttpRequest::Method, const Inspector::ArgList&)
{
    //获取当前时间和开始时间
    std::string result;
    result.reserve(1024);
    Timestamp now = Timestamp::now();
    result += "Page generated at ";
    result += now.toFormatTedString();
    result += "\nStarted at ";
    result += process_info::startTime().toFormatTedString();
    result += ", up for ";
    result += uptime(now, process_info::startTime(), true/* show microseconds */);
    result += "\n";

    std::string procStatus = process_info::procStatus();
    result += getProcessName(procStatus);
    result += " (";
    result += process_info::exePath();
    result += ") running as ";
    result += username_;
    result += " on ";
    result += process_info::hostname(); // cache ?
    result += "\n";

    stringPrintf(&result, "pid %d, num of threads %ld, bits %zd\n",
        process_info::pid(), getLong(procStatus, "Threads:"), CHAR_BIT * sizeof(void*));

    result += "Virtual memory: ";
    stringPrintf(&result, "%.3f MiB, ",
        static_cast<double>(getLong(procStatus, "VmSize:")) / 1024.0);

    result += "RSS memory: ";
    stringPrintf(&result, "%.3f MiB\n",
        static_cast<double>(getLong(procStatus, "VmRSS:")) / 1024.0);

    stringPrintf(&result, "Opened files: %d, limit: %d\n",
        process_info::openedFiles(), process_info::maxOpenFiles());

    process_info::CpuTime t = process_info::cpuTime();
    stringPrintf(&result, "User time: %12.3fs\nSys time:  %12.3fs\n",
        t.userSeconds, t.systemSeconds);

    return result;
}

std::string ProcessInspector::pid(HttpRequest::Method, const Inspector::ArgList&)
{
    char buf[32] = { 0 };
    snprintf(buf, sizeof buf, "%d", process_info::pid());
    return buf;
}

std::string ProcessInspector::procStatus(HttpRequest::Method, const Inspector::ArgList&)
{
    return process_info::procStatus();
}

std::string ProcessInspector::openedFiles(HttpRequest::Method, const Inspector::ArgList&)
{
    char buf[32] = { 0 };
    snprintf(buf, sizeof buf, "%d", process_info::openedFiles());
    return buf;
}

std::string ProcessInspector::threads(HttpRequest::Method, const Inspector::ArgList&)
{
    std::vector<pid_t> allThreads = process_info::threads();

    std::string result = "  TID NAME             S    User Time  System Time\n";
    result.reserve(allThreads.size() * 64);
    std::string stat;
    for (size_t i = 0; i < allThreads.size(); ++i)
    {
        char buf[256] = { 0 };
        int tid = allThreads[i];
        snprintf(buf, sizeof buf, "/proc/%d/task/%d/stat", process_info::pid(), tid);
        if (file_util::readFile(buf, 65536, &stat) == 0)
        {
            std::string name = process_info::procname(stat);

            std::vector<std::string> vecStat = utils::regexSplit(stat, " +");
            process_info::CpuTime t = getCpuTime(vecStat);
            snprintf(buf, sizeof(buf), "%5d %-16s %s %12.3f %12.3f\n",
                tid, name.c_str(), vecStat[2].c_str(), t.userSeconds, t.systemSeconds);
            result += buf;
        }
    }
    return result;
}

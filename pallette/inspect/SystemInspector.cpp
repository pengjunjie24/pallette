/*************************************************************************
  > File Name: SystemInspector.cpp
  > Author: pengjunjie
  > Mail: 1002398145@qq.com
  > Created Time: 2019年07月13日 星期六 08时02分18秒
 ************************************************************************/

#include <pallette/inspect/SystemInspector.h>
#include <pallette/FileUtil.h>

#include <sys/utsname.h>

using namespace pallette;

namespace pallette
{
    std::string uptime(Timestamp now, Timestamp start, bool showMicroseconds);
    long getLong(const std::string& content, const char* key);
    int stringPrintf(std::string* out, const char* fmt, ...) __attribute__((format(printf, 2, 3)));
}

void SystemInspector::registerCommands(Inspector* ins)
{
    ins->add("sys", "overview", SystemInspector::overview, "print system overview");
    ins->add("sys", "loadavg", SystemInspector::loadavg, "print /proc/loadavg");
    ins->add("sys", "version", SystemInspector::version, "print /proc/version");
    ins->add("sys", "cpuinfo", SystemInspector::cpuinfo, "print /proc/cpuinfo");
    ins->add("sys", "meminfo", SystemInspector::meminfo, "print /proc/meminfo");
    ins->add("sys", "stat", SystemInspector::stat, "print /proc/stat");
}

std::string SystemInspector::overview(HttpRequest::Method, const Inspector::ArgList&)
{
    std::string result;
    result.reserve(1024);
    Timestamp now = Timestamp::now();
    result += "Page generated at ";
    result += now.toFormatTedString();
    result += " (UTC)\n";
    // Hardware and OS
    {
        struct utsname un;
        if (::uname(&un) == 0)
        {
            stringPrintf(&result, "Hostname: %s\n", un.nodename);
            stringPrintf(&result, "Machine: %s\n", un.machine);
            stringPrintf(&result, "OS: %s %s %s\n", un.sysname, un.release, un.version);
        }
    }
    std::string stat;
    file_util::readFile("/proc/stat", 65536, &stat);
    Timestamp bootTime(Timestamp::kMicroSecondsPerSecond * getLong(stat, "btime "));
    result += "Boot time: ";
    result += bootTime.toFormatTedString(false /* show microseconds */);
    result += " (UTC)\n";
    result += "Up time: ";
    result += uptime(now, bootTime, false /* show microseconds */);
    result += "\n";

    // CPU load
    {
        std::string loadavg;
        file_util::readFile("/proc/loadavg", 65536, &loadavg);
        stringPrintf(&result, "Processes created: %ld\n", getLong(stat, "processes "));//创建任务个数
        stringPrintf(&result, "Loadavg: %s\n", loadavg.c_str());
    }

    // Memory
    {
        std::string meminfo;
        file_util::readFile("/proc/meminfo", 65536, &meminfo);
        long totalKb = getLong(meminfo, "MemTotal:");
        long freeKb = getLong(meminfo, "MemFree:");
        long buffersKb = getLong(meminfo, "Buffers:");
        long cachedKb = getLong(meminfo, "Cached:");

        stringPrintf(&result, "Total Memory: %6ld MiB\n", totalKb / 1024);
        stringPrintf(&result, "Free Memory:  %6ld MiB\n", freeKb / 1024);
        stringPrintf(&result, "Buffers:      %6ld MiB\n", buffersKb / 1024);
        stringPrintf(&result, "Cached:       %6ld MiB\n", cachedKb / 1024);
        stringPrintf(&result, "Real Used:    %6ld MiB\n", (totalKb - freeKb - buffersKb - cachedKb) / 1024);
        stringPrintf(&result, "Real Free:    %6ld MiB\n", (freeKb + buffersKb + cachedKb) / 1024);

        // Swap
    }
    // Disk
    // Network
    return result;
}

std::string SystemInspector::loadavg(HttpRequest::Method, const Inspector::ArgList&)
{
    std::string loadavg;
    file_util::readFile("/proc/loadavg", 65536, &loadavg);
    return loadavg;
}

std::string SystemInspector::version(HttpRequest::Method, const Inspector::ArgList&)
{
    std::string version;
    file_util::readFile("/proc/version", 65536, &version);
    return version;
}

std::string SystemInspector::cpuinfo(HttpRequest::Method, const Inspector::ArgList&)
{
    std::string cpuinfo;
    file_util::readFile("/proc/cpuinfo", 65536, &cpuinfo);
    return cpuinfo;
}

std::string SystemInspector::meminfo(HttpRequest::Method, const Inspector::ArgList&)
{
    std::string meminfo;
    file_util::readFile("/proc/meminfo", 65536, &meminfo);
    return meminfo;
}

std::string SystemInspector::stat(HttpRequest::Method, const Inspector::ArgList&)
{
    std::string stat;
    file_util::readFile("/proc/stat", 65536, &stat);
    return stat;
}

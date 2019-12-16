/*************************************************************************
  > File Name: ProcessInfo.cpp
  > Author: pengjunjie
  > Mail: 1002398145@qq.com
  > Created Time: 2019年01月01日 星期二 16时32分22秒
 ************************************************************************/

#include <pallette/ProcessInfo.h>

#include <pallette/FileUtil.h>

#include <limits.h>
#include <dirent.h>
#include <unistd.h>
#include <string.h>
#include <assert.h>
#include <pwd.h>
#include <sys/syscall.h>
#include <sys/resource.h>
#include <sys/times.h>

namespace pallette
{
    namespace detail
    {
        __thread int tNumOpenFiles = 0;
        //筛选条件，返回值为找到文件个数。找到的文件保存在tNumOpenFiles
        int fdDirFilter(const struct dirent* d)
        {
            if (::isdigit(d->d_name[0]))
            {
                ++tNumOpenFiles;
            }
            return 0;
        }

        __thread std::vector<pid_t>* tPids = NULL;
        int taskDirFilter(const struct dirent* d)
        {
            if (::isdigit(d->d_name[0]))
            {
                tPids->push_back(atoi(d->d_name));
            }
            return 0;
        }

        int scanDir(const char *dirpath, int(*filter)(const struct dirent *))
        {
            struct dirent** namelist = NULL;
            int result = ::scandir(dirpath, &namelist, filter, alphasort);
            assert(namelist == NULL);
            return result;
        }

        Timestamp gStartTime = Timestamp::now();
        int gClockTicks = static_cast<int>(::sysconf(_SC_CLK_TCK));//CPU时钟频率
        int gPageSize = static_cast<int>(::sysconf(_SC_PAGE_SIZE));//内存分页大小
    }
}

using namespace pallette;
using namespace pallette::detail;

std::string process_info::processname()
{
    char processdir[PATH_MAX] = { 0 };
    if (readlink("/proc/self/exe", processdir, sizeof(processdir)) < 0)
    {
        return "";
    }

    char* pathEnd = strrchr(processdir, '/');
    assert(pathEnd != NULL);
    ++pathEnd;
    return std::string(pathEnd);
}

pid_t process_info::pid()
{
    return ::getpid();
}

uid_t process_info::uid()
{
    return ::getuid();
}

Timestamp process_info::startTime()
{
    return gStartTime;
}

std::string process_info::username()
{
    struct passwd pwd;
    struct passwd* result = NULL;
    char buf[8192] = { 0 };
    const char* name = "unknownuser";

    getpwuid_r(uid(), &pwd, buf, sizeof buf, &result);
    if (result)
    {
        name = pwd.pw_name;
    }
    return name;
}

std::string process_info::hostname()
{
    char buf[256] = { 0 };
    if (::gethostname(buf, sizeof buf) == 0)
    {
        buf[sizeof(buf) - 1] = '\0';
        return buf;
    }
    else
    {
        return "unknownhost";
    }
}

int process_info::clockTicksPerSecond()
{
    return gClockTicks;
}

int process_info::pageSize()
{
    return gPageSize;
}

int process_info::openedFiles()
{
    tNumOpenFiles = 0;
    scanDir("/proc/self/fd", fdDirFilter);
    return tNumOpenFiles;
}

int process_info::maxOpenFiles()
{
    struct rlimit rl;
    if (::getrlimit(RLIMIT_NOFILE, &rl))
    {
        return openedFiles();
    }
    else
    {
        return static_cast<int>(rl.rlim_cur);
    }
}

std::string process_info::procStatus()
{
    std::string result;
    file_util::readFile("/proc/self/status", 65536, &result);
    return result;
}

std::string process_info::procStat()
{
    std::string result;
    file_util::readFile("/proc/self/stat", 65536, &result);
    return result;
}

std::string process_info::procname()
{
    return procname(procStat());
}

std::string process_info::procname(const std::string& stat)
{
    std::string name;
    size_t lp = stat.find('(');
    size_t rp = stat.rfind(')');
    if (lp != std::string::npos && rp != std::string::npos && lp < rp)
    {
        name.assign(stat.data() + lp + 1, static_cast<int>(rp - lp - 2));
    }
    return name;
}

std::string process_info::exePath()
{
    std::string result;
    char buf[1024] = { 0 };
    ssize_t n = ::readlink("/proc/self/exe", buf, sizeof buf);
    if (n > 0)
    {
        result.assign(buf, n);
    }
    return result;
}

process_info::CpuTime process_info::cpuTime()
{
    process_info::CpuTime t;
    struct tms ctms;
    if (::times(&ctms) >= 0)
    {
        const double hz = static_cast<double>(clockTicksPerSecond());
        t.userSeconds = static_cast<double>(ctms.tms_utime) / hz;
        t.systemSeconds = static_cast<double>(ctms.tms_stime) / hz;
    }

    return t;
}


int process_info::numThreads()
{
    int result = 0;
    std::string status = procStatus();
    size_t pos = status.find("Threads:");
    if (pos != std::string::npos)
    {
        result = ::atoi(status.c_str() + pos + 8);
    }
    return result;
}

std::vector<pid_t> process_info::threads()
{
    std::vector<pid_t> result;
    tPids = &result;
    scanDir("/proc/self/task", taskDirFilter);
    tPids = NULL;
    std::sort(result.begin(), result.end());
    return result;
}
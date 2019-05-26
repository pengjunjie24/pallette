/*************************************************************************
  > File Name: ProcessInfo.cpp
  > Author: pengjunjie
  > Mail: 1002398145@qq.com
  > Created Time: 2019年01月01日 星期二 16时32分22秒
 ************************************************************************/

#include "ProcessInfo.h"

#include <limits.h>
#include <unistd.h>
#include <string.h>
#include <assert.h>
#include <sys/syscall.h>

using namespace pallette;

namespace
{
    thread_local pid_t tCachedTid = 0;
    pid_t cachePid = 0;
}

std::string process_info::processname()
{
    char processdir[PATH_MAX] = { 0 };
    if (readlink("/proc/self/exe", processdir, sizeof(processdir)) < 0)
    {
        return "";
    }

    char* path_end = strrchr(processdir, '/');
    assert(path_end != NULL);
    ++path_end;
    return std::string(path_end);
}

pid_t process_info::pid()
{
    if (cachePid == 0)
    {
        cachePid = ::getpid();
    }

    return cachePid;
}

pid_t process_info::tid()
{
    if (tCachedTid == 0)
    {
        tCachedTid = static_cast<pid_t>(syscall(SYS_gettid));
    }
    return tCachedTid;
}
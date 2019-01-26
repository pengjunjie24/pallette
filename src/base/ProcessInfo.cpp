/*************************************************************************
  > File Name: ProcessInfo.cpp
  > Author: pengjunjie
  > Mail: 1002398145@qq.com
  > Created Time: 2019年01月01日 星期二 16时32分22秒
 ************************************************************************/

#include "ProcessInfo.h"

#include <thread>
#include <limits.h>
#include <unistd.h>
#include <string.h>
#include <assert.h>
using namespace pallette;

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
    return ::getpid();
}

size_t process_info::tid()
{
    std::thread::id id = std::this_thread::get_id();
    return std::hash<std::thread::id>()(id);
}
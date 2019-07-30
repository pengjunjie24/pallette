/*************************************************************************
  > File Name: ProcessInfo.h
  > Author: pengjunjie
  > Mail: 1002398145@qq.com
  > Created Time: 2019年01月01日 星期二 16时32分13秒
 ************************************************************************/

#ifndef PALLETTE_PROCESSINFO_H
#define PALLETTE_PROCESSINFO_H

#include "Timestamp.h"

#include <string>
#include <sys/types.h>

namespace pallette
{
    namespace process_info
    {
        std::string processname();
        pid_t pid();
        pid_t tid();
        uid_t uid();
        Timestamp startTime();
        std::string username();//得到进程用户名
        std::string hostname();//获取主机名
        int clockTicksPerSecond();
        int pageSize();//页大小
        bool isMainThread();//是否在主线程中
        int openedFiles();//打开文件描述符数目
        int maxOpenFiles();//最大打开文件描述符数目
        std::string procStatus();//进程状态信息,查看/proc/self/stat,比stat/statm更具可读性
        std::string procStat();//查看/proc/self/stat
        std::string procname();
        std::string procname(const std::string& stat);
        std::string exePath();//readlink /proc/self/exe

        int numThreads();
        std::vector<pid_t> threads();//当前进程下的所有线程号

        struct CpuTime
        {
            double userSeconds;
            double systemSeconds;

            CpuTime()
                : userSeconds(0.0)
                , systemSeconds(0.0)
            {
            }
        };

        CpuTime cpuTime();
    }
}

#endif

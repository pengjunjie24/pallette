/*************************************************************************
  > File Name: ProcessInfo.h
  > Author: pengjunjie
  > Mail: 1002398145@qq.com
  > Created Time: 2019年01月01日 星期二 16时32分13秒
 ************************************************************************/

#ifndef PALLETTE_PROCESSINFO_H
#define PALLETTE_PROCESSINFO_H

#include <string>

namespace pallette
{
    namespace process_info
    {
        std::string processname();
        pid_t pid();
        size_t tid();
    }
}

#endif

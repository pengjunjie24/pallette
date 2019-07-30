/*************************************************************************
  > File Name: SystemInspector.h
  > Author: pengjunjie
  > Mail: 1002398145@qq.com
  > Created Time: 2019年07月13日 星期六 08时02分08秒
 ************************************************************************/

#ifndef PALLETTE_SYSTEM_INSPECTOR_H
#define PALLETTE_SYSTEM_INSPECTOR_H

#include "Inspector.h"

namespace pallette
{
    class SystemInspector : noncopyable
    {
    public:
        void registerCommands(Inspector* ins);

        static std::string overview(HttpRequest::Method, const Inspector::ArgList&);
        static std::string loadavg(HttpRequest::Method, const Inspector::ArgList&);//系统平均负载
        static std::string version(HttpRequest::Method, const Inspector::ArgList&);//系统版本号
        static std::string cpuinfo(HttpRequest::Method, const Inspector::ArgList&);//CPU信息
        static std::string meminfo(HttpRequest::Method, const Inspector::ArgList&);//内存信息
        static std::string stat(HttpRequest::Method, const Inspector::ArgList&);//所有CPU活动的信息
    };
}

#endif

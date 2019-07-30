/*************************************************************************
  > File Name: ProcessInspector.h
  > Author: pengjunjie
  > Mail: 1002398145@qq.com
  > Created Time: 2019年07月04日 星期四 11时12分59秒
 ************************************************************************/

#ifndef PALLETTE_PROCESS_INSPECTOR_H
#define PALLETTE_PROCESS_INSPECTOR_H

#include "Inspector.h"

namespace pallette
{
    //该系统中进程的检测器
    class ProcessInspector : noncopyable
    {
    public:

        //将对应的回调函数注册到ins中,不同的uri就会调到不同的回调函数中去
        void registerCommands(Inspector* ins);

        static std::string overview(HttpRequest::Method, const Inspector::ArgList&);
        static std::string pid(HttpRequest::Method, const Inspector::ArgList&);//进程号
        static std::string procStatus(HttpRequest::Method, const Inspector::ArgList&);//进程状态信息
        static std::string openedFiles(HttpRequest::Method, const Inspector::ArgList&);//打开的文件描述符
        static std::string threads(HttpRequest::Method, const Inspector::ArgList&);//线程信息

        static std::string username_;
    };
}

#endif

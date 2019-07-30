/*************************************************************************
  > File Name: Inspector.h
  > Author: pengjunjie
  > Mail: 1002398145@qq.com
  > Created Time: 2019年07月04日 星期四 09时58分40秒
 ************************************************************************/

#ifndef PALLETTE_INSPECTOR_H
#define PALLETTE_INSPECTOR_H

#include "../http/HttpRequest.h"
#include "../http/HttpServer.h"

#include <mutex>
#include <map>

namespace pallette
{
    class ProcessInspector;
    class SystemInspector;

    //系统状态检测器，最好在单独线程运行，因为有些方法可能会阻塞
    class Inspector : noncopyable
    {
    public:
        typedef std::vector<std::string> ArgList;
        typedef std::function<std::string(HttpRequest::Method, const ArgList& args)> Callback;
        Inspector(EventLoop* loop,
            const InetAddress& httpAddr, const std::string& name);
        ~Inspector();

        /// Add a Callback for handling the special uri : /mudule/command
        //module:查看的目录
        //command:查看的信息
        //help:/moduel/command 这条uri信息的说明
        void add(const std::string& module, const std::string& command,
            const Callback& cb, const std::string& help);
        void remove(const std::string& module, const std::string& command);

    private:
        typedef std::map<std::string, Callback> CommandList;
        typedef std::map<std::string, std::string> HelpList;//说明帮助信息

        void start();
        void onRequest(const HttpRequest& req, HttpResponse* resp);

        HttpServer server_;
        std::unique_ptr<ProcessInspector> processInspector_;
        std::unique_ptr<SystemInspector> systemInspector_;
        std::mutex mutex_;
        std::map<std::string, CommandList> modules_;//存入对应业务处理的回调函数
        std::map<std::string, HelpList> helps_;
    };
}

#endif

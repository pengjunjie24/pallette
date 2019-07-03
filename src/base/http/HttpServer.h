/*************************************************************************
  > File Name: HttpServer.h
  > Author: pengjunjie
  > Mail: 1002398145@qq.com
  > Created Time: 2019年06月16日 星期日 15时31分35秒
 ************************************************************************/

#ifndef PALLETTE_HTTP_SERVER_H
#define PALLETTE_HTTP_SERVER_H

#include "../TcpServer.h"

namespace pallette
{
    class HttpRequest;
    class HttpResponse;

    //Http服务器
    class HttpServer : noncopyable
    {
    public:
        typedef std::function<void(const HttpRequest&,
            HttpResponse*)> HttpCallback;

        HttpServer(EventLoop* loop, const InetAddress& listenAddr,
            const std::string& name,
            TcpServer::Option option = TcpServer::kNoReusePort);

        EventLoop* getLoop()const { return server_.getLoop(); }
        void setHttpCallback(const HttpCallback& cb) { httpCallback_ = cb; }
        void setThreadNum(int numThreads) { server_.setThreadNum(numThreads); }

        void start();

    private:
        void onConnection(const TcpConnectionPtr& conn);
        void onMessage(const TcpConnectionPtr& conn, Buffer* buf, Timestamp receiveTime);
        void onRequest(const TcpConnectionPtr& conn, const HttpRequest& req);

        TcpServer server_;
        HttpCallback httpCallback_;
    };
}

#endif

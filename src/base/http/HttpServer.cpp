/*************************************************************************
  > File Name: HttpServer.cpp
  > Author: pengjunjie
  > Mail: 1002398145@qq.com
  > Created Time: 2019年06月16日 星期日 15时31分44秒
 ************************************************************************/

#include "HttpServer.h"

#include "HttpContext.h"
#include "HttpRequest.h"
#include "HttpResponse.h"
#include "../Logging.h"

using namespace pallette;

namespace pallette
{
    namespace detail
    {
        void defaultHttpCallback(const HttpRequest&, HttpResponse* resp)
        {
            resp->setStatusCode(HttpResponse::K404_NOT_FOUND);
            resp->setStatusMessage("Not Found");
            resp->setCloseConnection(true);
        }
    }
}


HttpServer::HttpServer(EventLoop* loop, const InetAddress& listenAddr,
    const std::string& name, TcpServer::Option option)
    :server_(loop, listenAddr, name, option)
    , httpCallback_(detail::defaultHttpCallback)
{
    server_.setConnectionCallback(
        std::bind(&HttpServer::onConnection, this, _1));
    server_.setMessageCallback(
        std::bind(&HttpServer::onMessage, this, _1, _2, _3));
}

void HttpServer::start()
{
    LOG_WARN << "HttpServer[" << server_.name()
        << "] start listenning on " << server_.ipPort();
    server_.start();
}


void HttpServer::onConnection(const TcpConnectionPtr& conn)
{
    if (conn->connected())
    {
        conn->setContext(HttpContext());//TcpConnection与一个HttpContext绑定
    }
}

void HttpServer::onMessage(const TcpConnectionPtr& conn,
    Buffer* buf, Timestamp receiveTime)
{
    HttpContext* context = any_cast<HttpContext>(conn->getMutableContext());

    if (!context->parseRequest(buf, receiveTime))
    {
        conn->send("HTTP/1.1 400 Bad Request\r\n\r\n");
        conn->shutdown();
    }

    //请求消息解析完毕
    if (context->gotAll())
    {
        onRequest(conn, context->request());
        context->reset();// 本次请求处理完毕，重置HttpContext，适用于长连接(一个连接多次请求)
    }
}

void HttpServer::onRequest(const TcpConnectionPtr& conn,
    const HttpRequest& req)
{
    const std::string& connection = req.getHeader("Connection");
    bool close = (connection == "close") ||
        (req.getVersion() == HttpRequest::HTTP10 && connection != "Keep-Alive");
    HttpResponse response(close);
    httpCallback_(req, &response);//客户代码设置的回调函数，填充response
    Buffer buf;
    response.appendToBuffer(&buf);//将响应填充到buf
    conn->send(&buf);
    if (response.closeConnection())
    {
        conn->shutdown();
    }
}
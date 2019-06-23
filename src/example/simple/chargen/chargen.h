/*************************************************************************
  > File Name: chargen.h
  > Author: pengjunjie
  > Mail: 1002398145@qq.com
  > Created Time: 2019年06月09日 星期日 14时27分28秒
 ************************************************************************/

#ifndef PALLETTE_EXAMPLE_CHARGEN_H
#define PALLETTE_EXAMPLE_CHARGEN_H

#include "../../../base/TcpServer.h"
#include "../../../base/Callbacks.h"

#include <string>

namespace pallette
{
    class InetAddress;
    class EventLoop;
}

//RFC864(字符产生器协议)，此服务可以是一个基于TCP的服务，一旦连接建立，
//服务器会传送一个字符流。这个连接直到客户断开连接为止。字符流会在用户请求下中止。
class ChargenServer
{
public:
    ChargenServer(pallette::EventLoop* loop,
        const pallette::InetAddress& listenAddr, bool print = false);

    void start();

private:
    void onConnection(const pallette::TcpConnectionPtr& conn);
    void onMessage(const pallette::TcpConnectionPtr& conn,
        pallette::Buffer* buf, pallette::Timestamp time);

    void onWriteComplete(const pallette::TcpConnectionPtr& conn);
    void printThroughput();

    pallette::TcpServer server_;
    std::string message_;//需要发送的字符串
    int64_t transferred_;//发送的数据量
    pallette::Timestamp startTime_;

};

#endif

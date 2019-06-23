/*************************************************************************
  > File Name: chargen.cpp
  > Author: pengjunjie
  > Mail: 1002398145@qq.com
  > Created Time: 2019年06月09日 星期日 14时27分36秒
 ************************************************************************/

#include "chargen.h"

#include "../../../base/Logging.h"
#include "../../../base/EventLoop.h"

using namespace pallette;

ChargenServer::ChargenServer(pallette::EventLoop* loop,
    const pallette::InetAddress& listenAddr, bool print)
    :server_(loop, listenAddr, "ChargenServer")
    , transferred_(0)
    , startTime_(Timestamp::now())
{
    server_.setConnectionCallback(std::bind(
        &ChargenServer::onConnection, this, _1));
    server_.setMessageCallback(std::bind(
        &ChargenServer::onMessage, this, _1, _2, _3));
    server_.setWriteCompleteCallback(std::bind(
        &ChargenServer::onWriteComplete, this, _1));

    if (print)
    {
        loop->runEvery(3.0, std::bind(
            &ChargenServer::printThroughput, this));
    }

    //拼装要发送的字符串
    std::string line;
    for (int i = 33; i < 127; ++i)
    {
        line.push_back(char(i));
    }
    line += line;

    for (size_t i = 0; i < 127 - 33; ++i)
    {
        message_ += line.substr(i, 72) + '\n';
    }
}

void ChargenServer::start()
{
    server_.start();
}


void ChargenServer::onConnection(
    const pallette::TcpConnectionPtr& conn)
{
    LOG_INFO << "ChargenServer - " << conn->peerAddress().toIpPort() << " -> "
        << conn->localAddress().toIpPort() << " is "
        << (conn->connected() ? "UP" : "DOWN");

    if (conn->connected())
    {
        conn->setTcpNoDelay(true);
        conn->send(message_);
    }
}

void ChargenServer::onMessage(const pallette::TcpConnectionPtr& conn,
    pallette::Buffer* buf, pallette::Timestamp time)
{
    std::string msg(buf->retrieveAllAsString());
    LOG_INFO << conn->name() << "discards " << msg.size()
        << "bytes received at " << time.toString();
}

void ChargenServer::onWriteComplete(const pallette::TcpConnectionPtr& conn)
{
    transferred_ += message_.size();
    conn->send(message_);
}

void ChargenServer::printThroughput()
{
    Timestamp endTime = Timestamp::now();
    double time = timeDifference(endTime, startTime_);
    printf("%4.3f MiB/s\n", static_cast<double>(transferred_) / time / 1024 / 1024);
    transferred_ = 0;
    startTime_ = endTime;
}
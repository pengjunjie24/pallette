/*************************************************************************
  > File Name: chargenclient.cpp
  > Author: pengjunjie
  > Mail: 1002398145@qq.com
  > Created Time: 2019年06月09日 星期日 16时19分08秒
 ************************************************************************/

#include "../../../base/Logging.h"
#include "../../../base/EventLoop.h"
#include "../../../base/InetAddress.h"
#include "../../../base/TcpClient.h"

#include <utility>

#include <stdio.h>
#include <unistd.h>

using namespace pallette;

class ChargenClient : noncopyable
{
public:
    ChargenClient(EventLoop* loop, const InetAddress& listenAddr)
        : loop_(loop),
        client_(loop, listenAddr, "ChargenClient")
    {
        client_.setConnectionCallback(
            std::bind(&ChargenClient::onConnection, this, _1));
        client_.setMessageCallback(
            std::bind(&ChargenClient::onMessage, this, _1, _2, _3));
        // client_.enableRetry();
    }

    void connect()
    {
        client_.connect();
    }

private:
    void onConnection(const TcpConnectionPtr& conn)
    {
        LOG_INFO << conn->localAddress().toIpPort() << " -> "
            << conn->peerAddress().toIpPort() << " is "
            << (conn->connected() ? "UP" : "DOWN");

        if (!conn->connected())
            loop_->quit();
    }

    void onMessage(const TcpConnectionPtr&, Buffer* buf, Timestamp)
    {
        buf->retrieveAll();
    }

    EventLoop* loop_;
    TcpClient client_;
};

int main(int argc, char* argv[])
{
    LOG_INFO << "pid = " << getpid();
    if (argc > 1)
    {
        EventLoop loop;
        InetAddress serverAddr(argv[1], 2019);

        ChargenClient chargenClient(&loop, serverAddr);
        chargenClient.connect();
        loop.loop();
    }
    else
    {
        printf("Usage: %s host_ip\n", argv[0]);
    }
}
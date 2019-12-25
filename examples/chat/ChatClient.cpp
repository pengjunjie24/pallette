/*************************************************************************
> File Name: ChatClient.cpp
> Author: pengjunjie
> Mail: 1002398145@qq.com
> Created Time: 2019年02月27日 星期三 20时15分07秒
************************************************************************/

#include <examples/chat/ChatCodec.h>

#include <pallette/TcpServer.h>
#include <pallette/EventLoop.h>
#include <pallette/TcpClient.h>
#include <pallette/TcpConnection.h>
#include <pallette/EventLoopThread.h>
#include <pallette/Logging.h>

#include <iostream>
#include <mutex>
#include <unistd.h>

using namespace pallette;

class ChatClient : public noncopyable
{
public:
    ChatClient(EventLoop* loop, const InetAddress& serverAddr)
        :client_(loop, serverAddr, "ChatClient")
        , codec_(std::bind(&ChatClient::onStringMessage, this, _1, _2, _3))
    {
        client_.setConnectionCallback(
            std::bind(&ChatClient::onConnection, this, _1));
        client_.setMessageCallback(
            std::bind(&LengthHeaderCodec::onMessage, &codec_, _1, _2, _3));
    }

    void connect()
    {
        client_.connect();
    }

    void disconnect()
    {
        client_.disconnect();
    }

    void write(const std::string& message)
    {
        std::unique_lock<std::mutex> lock(mutex_);
        if (connection_)
        {
            codec_.send(connection_, message);
        }
    }
private:
    void onConnection(const TcpConnectionPtr& conn)
    {
        LOG_INFO << conn->localAddress().toIpPort() << "->"
            << conn->peerAddress().toIpPort() << " is "
            << (conn->connected() ? "UP" : "DOWN");

        std::lock_guard<std::mutex> lock(mutex_);
        if (conn->connected())
        {
            connection_ = conn;
        }
        else
        {
            connection_.reset();
        }
    }

    void onStringMessage(const TcpConnectionPtr&,
        const std::string& message, Timestamp)
    {
        LOG_INFO << "<<< " << message.c_str();
    }

    TcpClient client_;
    LengthHeaderCodec codec_;//编解码器
    mutable std::mutex mutex_;
    TcpConnectionPtr connection_;
};

int main(int argc, char* argv[])
{
    LOG_INFO << "pid = " << getpid();

    if (argc > 2)
    {
        //两个线程，loopThread处理网络IO，main线程负责读键盘
        EventLoopThread loopThread;
        uint16_t port = static_cast<uint16_t>(atoi(argv[2]));
        InetAddress serverAddr(argv[1], port);

        ChatClient client(loopThread.startLoop(), serverAddr);
        client.connect();
        std::string line;
        while (std::getline(std::cin, line))
        {
            client.write(line);
        }
        client.disconnect();
        usleep(1000 * 1000);  // wait for disconnect, see ace/logging/client.cc
    }
    else
    {
        LOG_ERROR << "Usage: " << argv[0] << "port";
    }
}
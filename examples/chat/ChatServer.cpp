/*************************************************************************
> File Name: ChatServer.cpp
> Author: pengjunjie
> Mail: 1002398145@qq.com
> Created Time: 2019年02月27日 星期三 20时14分59秒
************************************************************************/

#include <examples/chat/ChatCodec.h>

#include <pallette/TcpServer.h>
#include <pallette/EventLoop.h>

#include <set>
#include <unistd.h>

using namespace pallette;

//将某个客户端发送数据转发给所有客户端
class ChatServer : public noncopyable
{
public:
    ChatServer(EventLoop* loop, InetAddress& listenAddr)
        :server_(loop, listenAddr, "ChatServer")
        , codec_(std::bind(&ChatServer::onStringMessage, this, _1, _2, _3))
    {
        server_.setConnectionCallback(
            std::bind(&ChatServer::onConnection, this, _1));
        server_.setMessageCallback(
            std::bind(&LengthHeaderCodec::onMessage, &codec_, _1, _2, _3));
    }

    void start()
    {
        server_.start();
    }

private:
    void onConnection(const TcpConnectionPtr& conn)
    {
        LOG_INFO << conn->localAddress().toIpPort() << "->"
            << conn->peerAddress().toIpPort() << " is "
            << (conn->connected() ? "UP" : "DOWN");

        if (conn->connected())
        {
            connections_.insert(conn);
        }
        else
        {
            connections_.erase(conn);
        }
    }

    void onStringMessage(const TcpConnectionPtr&,
        const std::string& message, Timestamp)
    {
        for (auto conn : connections_)
        {
            codec_.send(conn, message);
        }
    }

    TcpServer server_;
    LengthHeaderCodec codec_;//编解码器
    std::set<TcpConnectionPtr> connections_;//存储链接
};


int main(int argc, char* argv[])
{
    LOG_INFO << "pid = " << ::getpid();
    if (argc > 1)
    {
        EventLoop loop;
        uint16_t port = static_cast<uint16_t>(atoi(argv[1]));
        InetAddress serverAddr(port);
        ChatServer server(&loop, serverAddr);
        server.start();
        loop.loop();
    }
    else
    {
        LOG_ERROR << "Usage: " << argv[0] << "port";
    }
}
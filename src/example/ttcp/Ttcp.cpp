/*************************************************************************
	> File Name: Ttcp.cpp
	> Author: pengjunjie
	> Mail: 1002398145@qq.com
	> Created Time: 2019年03月14日 星期四 22时31分12秒
 ************************************************************************/

#include "Common.h"
#include "../../base/Logging.h"
#include "../../base/EventLoop.h"
#include "../../base/TcpClient.h"
#include "../../base/TcpServer.h"
#include "../../base/any.hpp"

#include <stdio.h>

using namespace pallette;

//客户端链接上后先发送SessionMessage结构体，告诉平台每次发送数据长度和发送次数
//然后客户端发送数据，数据格式为:数据头(int32)+数据内容
//服务端接收到客户端发送数据后，给客户端应答，应答内容为接收到数据长度(int32)
//客户端接收到应答后再继续发送下一条数据
//等到客户端发送次数达到预定次数后，主动断开连接
struct Context
{
    int count;//已经发送消息次数
    int64_t bytes;//发送消息总字节数
    SessionMessage session;
    Buffer output;

    Context()
        : count(0)
        , bytes(0)
    {
        session.number = 0;
        session.length = 0;
    }
};

namespace trans
{

    void onConnection(const Options& opt, const TcpConnectionPtr& conn)
    {
        LOG_INFO << conn->peerAddress().toIpPort() << " -> "
            << conn->localAddress().toIpPort() << " is "
            << (conn->connected() ? "UP" : "DOWN");

        if (conn->connected())
        {
            Context context;
            context.count = 1;
            context.bytes = opt.length;
            context.session.number = opt.number;
            context.session.length = opt.length;
            context.output.appendInt32(opt.length);
            context.output.ensureWritableBytes(opt.length);
            for (int i = 0; i < opt.length; ++i)
            {
                context.output.beginWrite()[i] = "0123456789ABCDEF"[i % 16];
            }
            context.output.hasWritten(opt.length);
            conn->setContext(context);

            SessionMessage sessionMessage = { 0, 0 };//填充SessionMessage结构体
            sessionMessage.number = htonl(opt.number);
            sessionMessage.length = htonl(opt.length);
            conn->send(&sessionMessage, sizeof(sessionMessage));//连接上后先发送SessionMessage
            conn->send(context.output.BuffertoString());//再发送数据
        }
        else
        {
            const Context& context = any_cast<Context>(conn->getContext());
            LOG_INFO << "payload bytes: " << context.bytes;
            conn->getLoop()->quit();
        }
    }

    void onMessage(const TcpConnectionPtr& conn, Buffer* buf, Timestamp)
    {
        Context* context = any_cast<Context>(conn->getMutableContext());
        while (buf->readableBytes() >= sizeof(int32_t))
        {
            int32_t length = buf->readInt32();
            if (length == context->session.length)//平台应答接收到的数据长度和客户端发送的数据长度相等
            {
                if (context->count < context->session.number)//客户端发送数据次数没有达到最大次数
                {
                    conn->send(context->output.BuffertoString());//继续发送数据
                    ++context->count;
                    context->bytes += length;//客户端累加发送数据总长度
                }
                else
                {
                    conn->shutdown();
                    break;
                }
            }
            else
            {
                conn->shutdown();//数据出现问题则断开
                break;
            }
        }
    }
}

void transmit(const Options& opt)
{
    InetAddress addr(opt.port);
    if (!InetAddress::resolve(opt.host, &addr))
    {
        LOG_FATAL << "Unable to resolve " << opt.host;
    }
    pallette::Timestamp start(pallette::Timestamp::now());
    EventLoop loop;
    TcpClient client(&loop, addr, "TtcpClient");
    client.setConnectionCallback(
        std::bind(&trans::onConnection, opt, _1));
    client.setMessageCallback(
        std::bind(&trans::onMessage, _1, _2, _3));
    client.connect();
    loop.loop();

    double elapsed = timeDifference(pallette::Timestamp::now(), start);//发送完成的时间
    double totalMb = 1.0 * opt.length * opt.number / 1024 / 1024;//发送数据大小(MB)
    printf("%.3f MiB transferred\n%.3f MiB/s\n", totalMb, totalMb / elapsed);
    printf("latency: %.6fs\n", elapsed / opt.number);
}


namespace receiving
{
    void onConnection(const TcpConnectionPtr& conn)
    {
        LOG_INFO << conn->peerAddress().toIpPort() << " -> "
            << conn->localAddress().toIpPort() << " is "
            << (conn->connected() ? "UP" : "DOWN");

        if (conn->connected())
        {
            Context context;
            conn->setContext(context);
        }
        else
        {
            const Context& context = any_cast<Context>(conn->getContext());
            LOG_INFO << "payload bytes: " << static_cast<double>(1.0 * context.bytes / 1024 / 1024)
                << "MiB from " << conn->peerAddress().toIpPort();
        }
    }


    void onMessage(const TcpConnectionPtr& conn, Buffer* buf, Timestamp)
    {
        while (buf->readableBytes() >= sizeof(int32_t))
        {
            Context* context = any_cast<Context>(conn->getMutableContext());
            SessionMessage& session = context->session;
            if (session.number == 0 && session.length == 0)//第一次收到客户端数据
            {
                if (buf->readableBytes() >= sizeof(SessionMessage))//要先接收到SessionMessage结构体
                {
                    session.number = buf->readInt32();
                    session.length = buf->readInt32();
                    context->output.appendInt32(session.length);//应答buffer中填入每次要接收的数据长度
                    LOG_INFO << "receive number = " << session.number << ", receive length = "
                        << session.length << " form " << conn->peerAddress().toIpPort();
                }
                else
                {
                    break;//没有接收到SessionMessage结构体长度的数据，重新等待接收
                }
            }
            else
            {
                const unsigned totalLen = session.length + static_cast<int>(sizeof(int32_t));//解析数据包头长度
                const int32_t length = buf->peekInt32();
                if (length == session.length)
                {
                    if (buf->readableBytes() >= totalLen)
                    {
                        buf->retrieve(totalLen);//接收到一条完整数据
                        conn->send(context->output.BuffertoString());//发送应答给客户端
                        ++context->count;//累加接收到的次数
                        context->bytes += length;//累加接收到的总数据长度

                        if (context->count >= session.number)
                        {
                            conn->shutdown();
                            break;
                        }
                    }
                    else
                    {
                        break;
                    }
                }
                else
                {
                    printf("wrong length %d\n", length);
                    conn->shutdown();
                    break;
                }
            }
        }
    }
}

void receive(const Options& opt)
{
    EventLoop loop;
    InetAddress listenAddr(opt.port);
    TcpServer server(&loop, listenAddr, "TtcpReceive");
    server.setConnectionCallback(
        std::bind(&receiving::onConnection, _1));
    server.setMessageCallback(
        std::bind(&receiving::onMessage, _1, _2, _3));
    server.start();
    loop.loop();
}
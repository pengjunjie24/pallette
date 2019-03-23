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

struct Context
{
    int count;//已经发送消息次数
    int64_t bytes;//发送消息总字节数
    SessionMessage session;
    Buffer output;

    Context()
        : count(0),
        bytes(0)
    {
        session.number = 0;
        session.length = 0;
    }
};

namespace trans
{

    void onConnection(const Options& opt, const TcpConnectionPtr& conn)
    {
        if (conn->connected())
        {
            LOG_INFO << conn->peerAddress().toIpPort() << " -> "
                << conn->localAddress().toIpPort() << " is "
                << (conn->connected() ? "UP" : "DOWN");

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

            SessionMessage sessionMessage = { 0, 0 };
            sessionMessage.number = htonl(opt.number);
            sessionMessage.length = htonl(opt.length);
            conn->send(&sessionMessage, sizeof(sessionMessage));
            conn->send(context.output.BuffertoString());
        }
        else
        {
            const Context& context = any_cast<Context>(conn->getContext());
            LOG_INFO << "payload bytes " << context.bytes;
            conn->getLoop()->quit();
        }
    }

    void onMessage(const TcpConnectionPtr& conn, Buffer* buf, Timestamp)
    {
        Context* context = any_cast<Context>(conn->getMutableContext());
        while (buf->readableBytes() >= sizeof(int32_t))
        {
            int32_t length = buf->readInt32();
            if (length == context->session.length)
            {
                if (context->count < context->session.number)
                {
                    conn->send(context->output.BuffertoString());
                    ++context->count;
                    context->bytes += length;
                }
                else
                {
                    conn->shutdown();
                    break;
                }
            }
            else
            {
                conn->shutdown();
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

    double elapsed = timeDifference(pallette::Timestamp::now(), start);
    double total_mb = 1.0 * opt.length * opt.number / 1024 / 1024;
    printf("%.3f MiB transferred\n%.3f MiB/s\n", total_mb, total_mb / elapsed);
}


namespace receiving
{
    void onConnection(const TcpConnectionPtr& conn)
    {
        if (conn->connected())
        {
            Context context;
            conn->setContext(context);
        }
        else
        {
            const Context& context = any_cast<Context>(conn->getContext());
            LOG_INFO << "payload bytes " << context.bytes;
            conn->getLoop()->quit();
        }
    }


    void onMessage(const TcpConnectionPtr& conn, Buffer* buf, Timestamp)
    {
        while (buf->readableBytes() >= sizeof(int32_t))
        {
            Context* context = any_cast<Context>(conn->getMutableContext());
            SessionMessage& session = context->session;
            if (session.number == 0 && session.length == 0)
            {
                if (buf->readableBytes() >= sizeof(SessionMessage))
                {
                    session.number = buf->readInt32();
                    session.length = buf->readInt32();
                    context->output.appendInt32(session.length);
                    printf("receive number = %d\nreceive length = %d\n",
                        session.number, session.length);
                }
                else
                {
                    break;
                }
            }
            else
            {
                const unsigned total_len = session.length + static_cast<int>(sizeof(int32_t));
                const int32_t length = buf->peekInt32();
                if (length == session.length)
                {
                    if (buf->readableBytes() >= total_len)
                    {
                        buf->retrieve(total_len);
                        conn->send(context->output.BuffertoString());
                        ++context->count;
                        context->bytes += length;
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
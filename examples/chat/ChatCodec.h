/*************************************************************************
> File Name: Codec.h
> Author: pengjunjie
> Mail: 1002398145@qq.com
> Created Time: 2019年02月27日 星期三 20时11分27秒
************************************************************************/

#ifndef PALLETTE_EXAMPLE_CHAT_CODEC_H
#define PALLETTE_EXAMPLE_CHAT_CODEC_H

#include <pallette/Logging.h>
#include <pallette/TcpConnection.h>

#include <functional>
#include <string>

//编解码器类，处理到达的数据,目的让用户只关心消息到达
class LengthHeaderCodec : pallette::noncopyable
{
public:
    typedef std::function<void(const pallette::TcpConnectionPtr&, const std::string&,
        pallette::Timestamp)> StringMessageCallback;

    LengthHeaderCodec(const StringMessageCallback& cb)
        :messageCallback_(cb)
    {
    }

    //解码
    void onMessage(const pallette::TcpConnectionPtr& conn,
        pallette::Buffer* buf, pallette::Timestamp recvTime)
    {
        while (buf->readableBytes() >= kHeaderLen)
        {
            const int32_t len = buf->peekInt32();
            if (len > 65535 || len < 0)
            {
                LOG_ERROR << "Invalid length:" << len;
                conn->forceClose();//FIXME: disable reading
                break;
            }
            else if (buf->readableBytes() >= len + kHeaderLen)
            {
                buf->retrieve(kHeaderLen);
                std::string message(buf->peek(), len);
                messageCallback_(conn, message, recvTime);//将接受的消息给用户回调
                buf->retrieve(len);
            }
            else
            {
                break;
            }
        }
    }

    //编码
    void send(pallette::TcpConnectionPtr& conn, const std::string& message)
    {
        pallette::Buffer buf;
        buf.append(message.data(), message.size());
        int32_t len = static_cast<int32_t>(message.size());
        buf.prependInt32(len);
        conn->send(&buf);
    }
private:
    StringMessageCallback messageCallback_;//接收到Tcp数据后的回调
    const static size_t kHeaderLen = sizeof(int32_t);
};

#endif
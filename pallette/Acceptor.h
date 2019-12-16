/*************************************************************************
> File Name: Acceptor.h
> Author: pengjunjie
> Mail: 1002398145@qq.com
> Created Time: 2019年01月29日 星期二 16时58分41秒
************************************************************************/

#ifndef PALLETTE_ACCEPTOR_H
#define PALLETTE_ACCEPTOR_H

#include <pallette/noncopyable.h>
#include <pallette/Channel.h>
#include <pallette/Socket.h>

#include <functional>

namespace pallette
{
    class EventLoop;
    class InetAddress;

    //对accept操作的封装
    class Acceptor : noncopyable
    {
    public:
        typedef std::function<void(int sockfd, const InetAddress&)> NewConnectionCallback;

        Acceptor(EventLoop* loop, const InetAddress& listenAddr, bool reuseport);
        ~Acceptor();

        void setNewConnectionCallback(const NewConnectionCallback& cb)
        {
            newConnectionCallback_ = cb;
        }

        bool listenning() const { return listenning_; }
        void listen();

    private:
        void handleRead();

        EventLoop* loop_;
        Socket acceptSocket_;//监听ip和端口的套接字
        Channel acceptChannel_;//监听套接字的通道
        NewConnectionCallback newConnectionCallback_;//有新连接到来时调用的回调函数
        bool listenning_;
        int idleFd_;//空闲文件描述符，用于fd耗尽时
    };
}

#endif
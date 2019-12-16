/*************************************************************************
> File Name: Socket.h
> Author: pengjunjie
> Mail: 1002398145@qq.com
> Created Time: 2019年01月29日 星期二 16时33分54秒
************************************************************************/

#ifndef PALLETTE_SOCKET_H
#define PALLETTE_SOCKET_H

#include <pallette/noncopyable.h>

struct tcp_info;

namespace pallette
{
    class InetAddress;

    //对socket套接字各项操作的封装
    class Socket : noncopyable
    {
    public:
        explicit Socket(int sockfd)
            : sockfd_(sockfd)
        {
        }

        ~Socket();

        int fd() const { return sockfd_; }
        //return true if success.
        bool getTcpInfo(struct tcp_info*) const;
        bool getTcpInfoString(char* buf, int len) const;

        // abort if address in use
        void bindAddress(const InetAddress& localaddr);
        // abort if address in use
        void listen();


        int accept(InetAddress* peeraddr);
        void shutdownWrite();

        // Enable/disable TCP_NODELAY (disable/enable Nagle's algorithm).
        void setTcpNoDelay(bool on);
        // Enable/disable SO_REUSEADDR
        void setReuseAddr(bool on);
        // Enable/disable SO_REUSEPORT
        void setReusePort(bool on);
        // Enable/disable SO_KEEPALIVE
        void setKeepAlive(bool on);

    private:
        const int sockfd_;
    };
}

#endif
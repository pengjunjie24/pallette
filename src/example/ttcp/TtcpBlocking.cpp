/*************************************************************************
	> File Name: TtcpBlocking.cpp
	> Author: pengjunjie
	> Mail: 1002398145@qq.com
	> Created Time: 2019年03月14日 星期四 22时31分01秒
 ************************************************************************/

#include "Common.h"
#include "../../base/Timestamp.h"

#include <assert.h>
#include <errno.h>
#include <stdio.h>
#include <strings.h>
#include <unistd.h>

#include <netinet/in.h>
#include <arpa/inet.h>

static int acceptOrDie(uint16_t port)
{
    int listenfd = ::socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);
    assert(listenfd > 0);

    int yes = 1;
    if (setsockopt(listenfd, SOL_SOCKET, SO_REUSEADDR, &yes, sizeof(yes)))
    {
        perror("setsockopt");
        exit(1);
    }

    struct sockaddr_in addr;
    bzero(&addr, sizeof(addr));
    addr.sin_family = AF_INET;
    addr.sin_port = htons(port);
    addr.sin_addr.s_addr = INADDR_ANY;
    if (bind(listenfd, reinterpret_cast<struct sockaddr*>(&addr), sizeof(addr)))
    {
        perror("bind");
        exit(1);
    }

    if (listen(listenfd, 5))
    {
        perror("listen");
        exit(1);
    }

    struct sockaddr_in peerAddr;
    bzero(&peerAddr, sizeof(peerAddr));
    socklen_t addrlen = 0;
    int sockfd = ::accept(listenfd, reinterpret_cast<struct sockaddr*>(&peerAddr), &addrlen);
    if (sockfd < 0)
    {
        perror("accept");
        exit(1);
    }
    ::close(listenfd);
    return sockfd;
}

static int writeN(int sockfd, const void* buf, int length)
{
    int written = 0;
    while (written < length)
    {
        ssize_t nw = ::write(sockfd, static_cast<const char*>(buf)+written,
            length - written);
        if (nw > 0)
        {
            written += static_cast<int>(nw);
        }
        else if (nw == 0)
        {
            break;
        }
        else if (errno != EINTR)
        {
            perror("write");
            break;
        }
    }
    return written;
}

static int readN(int sockfd, void* buf, int length)
{
    int nread = 0;
    while (nread < length)
    {
        ssize_t nr = ::read(sockfd, static_cast<char*>(buf) + nread,
            length - nread);
        if (nr > 0)
        {
            nread += static_cast<int>(nr);
        }
        else if (nr == 0)
        {
            break;
        }
        else if (errno != EINTR)
        {
            perror("read");
            break;
        }
    }

    return nread;
}

void transmit(const Options& opt)
{
    struct sockaddr_in addr = resolveOrDie(opt.host.c_str(), opt.port);
    printf("connecting to %s:%d\n", inet_ntoa(addr.sin_addr), opt.port);

    int sockfd = ::socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);
    assert(sockfd > 0);
    int ret = ::connect(sockfd, reinterpret_cast<struct sockaddr*>(&addr),
        sizeof(addr));
    if (ret)
    {
        perror("connect");
        printf("Unable to connect %s\n",opt.host.c_str());
        ::close(sockfd);
        return;
    }

    printf("connected\n");
    pallette::Timestamp start(pallette::Timestamp::now());
    //得到每条消息长度和发送次数
    struct SessionMessage sessionMessage = { 0, 0 };
    sessionMessage.number = htonl(opt.number);
    sessionMessage.length = htonl(opt.length);
    if (writeN(sockfd, &sessionMessage, sizeof(sessionMessage)) != sizeof(sessionMessage))
    {
        perror("write SessionMessage");
        exit(1);
    }

    const int totalLen = static_cast<int>(sizeof(int32_t)+ opt.length);
    PayloadMessage* payload = static_cast<PayloadMessage*>(::malloc(totalLen));
    assert(payload);//错误处理，比较简陋
    payload->length = htonl(opt.length);
    //给将要发送的消息赋值
    for (int i = 0; i < opt.length; ++i)
    {
        payload->data[i] = "0123456789ABCDEF"[i % 16];
    }
    double totalMb = 1.0 * opt.length * opt.number / 1024 / 1024;
    printf("%.3f MiB in total\n", totalMb);

    //发送消息
    for (int i = 0; i < opt.number; ++i)
    {
        //每次发送totalLen长度的消息
        int nw = writeN(sockfd, payload, totalLen);
        assert(nw == totalLen);

        //接收到对端应用层应答才能继续发送
        int ack = 0;
        int nr = readN(sockfd, &ack, sizeof(ack));
        assert(nr == sizeof(ack));
        ack = ntohl(ack);
        assert(ack == opt.length);
    }

    ::free(payload);
    ::close(sockfd);
    double elapsed = pallette::timeDifference(pallette::Timestamp::now(), start);
    printf("%.3f seconds\n%.3f MiB/s\n", elapsed, totalMb / elapsed);//得到发送时长和带宽
}

void receive(const Options& opt)
{
    int sockfd = acceptOrDie(opt.port);
    SessionMessage sessionMessage = { 0, 0 };
    //连接建立后得到将要接收消息的讯息
    if (readN(sockfd, &sessionMessage, sizeof(sessionMessage)) != sizeof(sessionMessage))
    {
        perror("read SessionMessage");
        exit(1);
    }
    sessionMessage.number = ntohl(sessionMessage.number);
    sessionMessage.length = ntohl(sessionMessage.length);
    printf("receive number = %d\nreceive length = %d\n",
        sessionMessage.number, sessionMessage.length);
    //申请足够的空间接收消息
    const int totalLen = static_cast<int>(sizeof(int32_t) + sessionMessage.length);
    PayloadMessage* payload = static_cast<PayloadMessage*>(::malloc(totalLen));
    assert(payload);

    for (int i = 0; i < sessionMessage.number; ++i)
    {
        //得到这次接收消息的长度
        payload->length = 0;
        if (readN(sockfd, &payload->length, sizeof(payload->length)) != sizeof(payload->length))
        {
            perror("read length");
            exit(1);
        }
        payload->length = ntohl(payload->length);
        assert(payload->length == sessionMessage.length);
        //接收消息
        if (readN(sockfd, payload->data, payload->length) != payload->length)
        {
            perror("read payload data");
            exit(1);
        }
        //接收完成后返回应用层应答
        int32_t ack = htonl(payload->length);
        if (writeN(sockfd, &ack, sizeof(ack)) != sizeof(ack))
        {
            perror("write ack");
            exit(1);
        }
    }

    ::free(payload);
    ::close(sockfd);
}
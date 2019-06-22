/*************************************************************************
> File Name: Connector.cpp
> Author: pengjunjie
> Mail: 1002398145@qq.com
> Created Time: 2019年01月30日 星期三 16时15分58秒
************************************************************************/

#include "Connector.h"

#include "Logging.h"
#include "Channel.h"
#include "EventLoop.h"
#include "SocketsOperations.h"

#include <errno.h>
#include <assert.h>

using namespace pallette;

const int Connector::kMaxRetryDelayMs;
const int Connector::kInitRetryDelayMs;

Connector::Connector(EventLoop* loop, const InetAddress& serverAddr)
: loop_(loop),
serverAddr_(serverAddr),
connect_(false),
state_(kDisconnected),
retryDelayMs_(kInitRetryDelayMs),
retryConnector_(false)
{
    LOG_DEBUG << "ctor[" << this << "]";
}

Connector::~Connector()
{
    LOG_DEBUG << "dtor[" << this << "]";
    assert(!channel_);
}

void Connector::start()
{
    connect_ = true;
    loop_->runInLoop(std::bind(&Connector::startInLoop, this)); // FIXME: unsafe
}

void Connector::startInLoop()
{
    loop_->assertInLoopThread();
    assert(state_ == kDisconnected);
    retryConnector_ = false;
    if (connect_)
    {
        connect();
    }
    else
    {
        LOG_DEBUG << "do not connect";
    }
}

void Connector::stop()
{
    connect_ = false;
    loop_->queueInLoop(std::bind(&Connector::stopInLoop, this)); // FIXME: unsafe
}

void Connector::stopInLoop()
{
    loop_->assertInLoopThread();

    if (retryConnector_)
    {
        loop_->cancel(retryConnectorTimerId_);
    }

    if (state_ == kConnecting)
    {
        setState(kDisconnected);
        int sockfd = removeAndResetChannel();
        retry(sockfd);
    }
}

void Connector::connect()
{
    int sockfd = sockets::createNonblockingOrDie(serverAddr_.family());
    int ret = sockets::connect(sockfd, serverAddr_.getSockAddr());
    int savedErrno = (ret == 0) ? 0 : errno;
    switch (savedErrno)
    {
    case 0:
    case EINPROGRESS://连接建立中
    case EINTR://系统调用的执行由于捕获中断而中止
    case EISCONN://已经连接到该套接字
        connecting(sockfd);
        break;

    case EAGAIN://没有足够空闲的本地端口
    case EADDRINUSE://本地地址处于使用状态
    case EADDRNOTAVAIL:
    case ECONNREFUSED: //远程地址并没有处于监听状态
    case ENETUNREACH: //网络不可到达
        retry(sockfd);
        break;

    case EACCES:
    case EPERM://在套接字广播标志没有设置的情况下连接广播地址或由于防火墙策略导致连接失败
    case EAFNOSUPPORT://参数serv_add中的地址非合法地址
    case EALREADY://套接字为非阻塞套接字，并且原来的连接请求还未完成
    case EBADF://非法的文件描述符
    case EFAULT://指向套接字结构体的地址非法
    case ENOTSOCK://文件描述符不与套接字相关
        LOG_SYSERR << "connect error in Connector::startInLoop " << savedErrno;
        sockets::close(sockfd);
        break;

    default:
        LOG_SYSERR << "Unexpected error in Connector::startInLoop " << savedErrno;
        sockets::close(sockfd);
        break;
    }
}

void Connector::restart()
{
    loop_->assertInLoopThread();
    setState(kDisconnected);
    retryDelayMs_ = kInitRetryDelayMs;
    connect_ = true;
    startInLoop();
}

void Connector::connecting(int sockfd)
{
    setState(kConnecting);
    assert(!channel_);
    channel_.reset(new Channel(loop_, sockfd));
    channel_->setWriteCallback(
        std::bind(&Connector::handleWrite, this)); // FIXME: unsafe
    channel_->setErrorCallback(
        std::bind(&Connector::handleError, this)); // FIXME: unsafe

    channel_->enableWriting();
}

int Connector::removeAndResetChannel()
{
    channel_->disableAll();
    channel_->remove();
    int sockfd = channel_->fd();
    loop_->queueInLoop(std::bind(&Connector::resetChannel, this)); // FIXME: unsafe
    return sockfd;
}

void Connector::resetChannel()
{
    channel_.reset();
}

void Connector::handleWrite()
{
    LOG_TRACE << "Connector::handleWrite " << state_;

    if (state_ == kConnecting)//连接建立后不用再关注channel中可写事件，使用channel_.reset析构掉
    {
        int sockfd = removeAndResetChannel();
        int err = sockets::getSocketError(sockfd);//socket可写不一定连接成功，还需要getsockopt()再次确认
        if (err)
        {
            LOG_WARN << "Connector::handleWrite - SO_ERROR = "
                << err << " " << strerror_rl(err);
            retry(sockfd);
        }
        else if (sockets::isSelfConnect(sockfd))
        {
            LOG_WARN << "Connector::handleWrite - Self connect";
            retry(sockfd);
        }
        else
        {
            setState(kConnected);
            if (connect_)
            {
                newConnectionCallback_(sockfd);//连接成功后，调用用户回调
            }
            else
            {
                sockets::close(sockfd);
            }
        }
    }
    else
    {
        assert(state_ == kDisconnected);
    }
}

void Connector::handleError()
{
    if (state_ == kConnecting)
    {
        int sockfd = removeAndResetChannel();
        int err = sockets::getSocketError(sockfd);
        LOG_TRACE << "SO_ERROR = " << err << " " << strerror_rl(err);
        retry(sockfd);
    }
}

void Connector::retry(int sockfd)
{
    sockets::close(sockfd);
    setState(kDisconnected);
    if (connect_)
    {
        LOG_INFO << "Connector::retry - Retry connecting to " << serverAddr_.toIpPort()
            << " in " << retryDelayMs_ << " milliseconds. ";
        retryConnectorTimerId_ = loop_->runAfter(retryDelayMs_ / 1000.0,
            std::bind(&Connector::startInLoop, shared_from_this()));
        retryDelayMs_ = std::min(retryDelayMs_ * 2, kMaxRetryDelayMs);//每次重连时间延长一倍
        retryConnector_ = true;
    }
    else
    {
        LOG_DEBUG << "do not connect";
    }
}

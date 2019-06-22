/*************************************************************************
> File Name: TcpConnection.h
> Author: pengjunjie
> Mail: 1002398145@qq.com
> Created Time: 2019年01月30日 星期三 10时25分17秒
************************************************************************/

#ifndef PALLETTE_TCPCONNECTION_H
#define PALLETTE_TCPCONNECTION_H

#include "any.hpp"
#include "noncopyable.h"
#include "Callbacks.h"
#include "Buffer.h"
#include "InetAddress.h"

#include <atomic>

struct tcp_info;

namespace pallette
{
    class Channel;
    class EventLoop;
    class Socket;

    //Tcp建立连接后的通讯对象
    class TcpConnection : noncopyable,
        public std::enable_shared_from_this<TcpConnection>
    {
    public:
        TcpConnection(EventLoop* loop, const std::string& name, int sockfd,
            const InetAddress& localAddr, const InetAddress& peerAddr);
        ~TcpConnection();

        EventLoop* getLoop() const { return loop_; }
        const std::string& name() const { return name_; }
        const InetAddress& localAddress() const { return localAddr_; }
        const InetAddress& peerAddress() const { return peerAddr_; }
        bool connected() const { return state_ == kConnected; }
        bool disconnected() const { return state_ == kDisconnected; }
        bool getTcpInfo(struct tcp_info*) const;
        std::string getTcpInfoString() const;

        void send(const void* message, int len);
        void send(const std::string& message);
        void send(Buffer* message);
        void shutdown();//关闭连接调用shutdown而不调用close,避免数据丢失
        void forceClose();
        void forceCloseWithDelay(double seconds);
        void setTcpNoDelay(bool on);
        void startRead();
        void stopRead();
        bool isReading() const { return reading_; };

        void setContext(const any& context) { context_ = context; }
        const any& getContext() { return context_; }
        any* getMutableContext() { return &context_; }

        void setConnectionCallback(const ConnectionCallback& cb)
        {
            connectionCallback_ = cb;
        }
        void setMessageCallback(const MessageCallback& cb)
        {
            messageCallback_ = cb;
        }
        void setWriteCompleteCallback(const WriteCompleteCallback& cb)
        {
            writeCompleteCallback_ = cb;
        }
        void setHighWaterMarkCallback(const HighWaterMarkCallback& cb, size_t highWaterMark)
        {
            highWaterMarkCallback_ = cb; highWaterMark_ = highWaterMark;
        }
        void setCloseCallback(const CloseCallback& cb)
        {
            closeCallback_ = cb;
        }

        Buffer* inputBuffer(){ return &inputBuffer_; }
        Buffer* outputBuffer(){ return &outputBuffer_; }

        void connectEstablished();//连接建立，在TcpServer中建立连接后调用该函数
        void connectDestroyed();//连接断开，在TcpServer中移除连接时调用函数

        std::string creationTime() { return creationTime_.toFormatTedString(); }
        std::string lastReceiveTime() { return lastReceiveTime_.toFormatTedString(); }
        uint64_t bytesReceived() { return bytesReceived_; }
        uint64_t bytesSent() { return bytesSent_; }

    private:
        enum StateE { kDisconnected, kConnecting, kConnected, kDisconnecting };//类似于状态机,对Tcp连接的状态定义
        void handleRead(Timestamp receiveTime);
        void handleWrite();
        void handleClose();
        void handleError();
        void sendInLoop(const std::string& message);
        void sendInLoop(const void* message, size_t len);
        void shutdownInLoop();
        void forceCloseInLoop();
        void setState(StateE s) { state_ = s; }
        const char* stateToString() const;
        void startReadInLoop();
        void stopReadInLoop();

        EventLoop* loop_;
        const std::string name_;
        std::atomic<StateE> state_;
        bool reading_;
        std::unique_ptr<Socket> socket_;
        std::unique_ptr<Channel> channel_;
        const InetAddress localAddr_;//本地服务器信息
        const InetAddress peerAddr_;//远端客户端信息

        ConnectionCallback connectionCallback_;//TcpConn连接和断开调用的回调,用于处理连接和断开事件
        MessageCallback messageCallback_;
        WriteCompleteCallback writeCompleteCallback_;
        HighWaterMarkCallback highWaterMarkCallback_;//当发送缓冲区上限阀值过小，调用该函数
        CloseCallback closeCallback_;
        size_t highWaterMark_;//发送缓冲区数据上限阀值

        Buffer inputBuffer_;
        Buffer outputBuffer_; // FIXME: use list<Buffer> as output buffer.
        Timestamp creationTime_;
        Timestamp lastReceiveTime_;
        uint64_t bytesReceived_;
        uint64_t bytesSent_;

        any context_;
    };

    typedef std::shared_ptr<TcpConnection> TcpConnectionPtr;
}

#endif
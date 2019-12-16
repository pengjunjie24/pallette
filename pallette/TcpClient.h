/*************************************************************************
> File Name: TcpClient.h
> Author: pengjunjie
> Mail: 1002398145@qq.com
> Created Time: 2019年01月31日 星期四 16时32分18秒
************************************************************************/

#ifndef PALLETTE_TCP_CLIENT_H
#define PALLETTE_TCP_CLIENT_H

#include <pallette/TcpConnection.h>

#include <mutex>
#include <atomic>

namespace pallette
{
    class Connector;
    typedef std::shared_ptr<Connector> ConnectorPtr;

    //Tcp客户端封装
    class TcpClient : noncopyable
    {
    public:
        TcpClient(EventLoop* loop, const InetAddress& serverAddr, const std::string& nameArg);
        ~TcpClient();

        void connect();
        void disconnect();
        void stop();

        TcpConnectionPtr connection() const
        {
            std::unique_lock<std::mutex> locker(mutex_);
            return connection_;
        }

        EventLoop* getLoop() const { return loop_; }
        bool retry() const { return retry_; }
        void enableRetry() { retry_ = true; }

        const std::string& name() const { return name_; }

        void setConnectionCallback(ConnectionCallback cb)
        {
            connectionCallback_ = std::move(cb);
        }
        void setMessageCallback(MessageCallback cb)
        {
            messageCallback_ = std::move(cb);
        }
        void setWriteCompleteCallback(WriteCompleteCallback cb)
        {
            writeCompleteCallback_ = std::move(cb);
        }

    private:
        void newConnection(int sockfd);
        void removeConnection(const TcpConnectionPtr& conn);

        EventLoop* loop_;
        ConnectorPtr connector_;
        const std::string name_;
        ConnectionCallback connectionCallback_;
        MessageCallback messageCallback_;
        WriteCompleteCallback writeCompleteCallback_;
        std::atomic<bool> retry_;
        std::atomic<bool> connect_;

        int nextConnId_;
        mutable std::mutex mutex_;
        TcpConnectionPtr connection_;
    };
}

#endif
/*************************************************************************
> File Name: Connector.h
> Author: pengjunjie
> Mail: 1002398145@qq.com
> Created Time: 2019年01月30日 星期三 16时15分47秒
************************************************************************/

#ifndef PALLETTE_CONNECTOR_H
#define PALLETTE_CONNECTOR_H

#include "InetAddress.h"
#include "noncopyable.h"
#include "TimerId.h"

#include <functional>
#include <memory>
#include <atomic>

namespace pallette
{
    class Channel;
    class EventLoop;

    class Connector : noncopyable,
        public std::enable_shared_from_this<Connector>
    {
    public:
        typedef std::function<void(int sockfd)> NewConnectionCallback;

        Connector(EventLoop* loop, const InetAddress& serverAddr);
        ~Connector();

        void setNewConnectionCallback(const NewConnectionCallback& cb)
        {
            newConnectionCallback_ = cb;
        }

        void start();
        void restart();
        void stop();

        const InetAddress& serverAddress() const { return serverAddr_; }

    private:
        enum States { kDisconnected, kConnecting, kConnected };

        void setState(States s) { state_ = s; }
        void startInLoop();
        void stopInLoop();
        void connect();
        void connecting(int sockfd);
        void handleWrite();
        void handleError();
        void retry(int sockfd);
        int removeAndResetChannel();
        void resetChannel();

        EventLoop* loop_;
        InetAddress serverAddr_;
        std::atomic<bool> connect_;
        std::atomic<States> state_;
        std::unique_ptr<Channel> channel_;
        NewConnectionCallback newConnectionCallback_;
        int retryDelayMs_;
        bool retryConnector_;//是否有重连事件
        TimerId retryConnectorTimerId_;

        static const int kMaxRetryDelayMs = 30 * 1000;
        static const int kInitRetryDelayMs = 500;
    };
}

#endif
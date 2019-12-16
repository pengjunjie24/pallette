/*************************************************************************
> File Name: Connector.h
> Author: pengjunjie
> Mail: 1002398145@qq.com
> Created Time: 2019年01月30日 星期三 16时15分47秒
************************************************************************/

#ifndef PALLETTE_CONNECTOR_H
#define PALLETTE_CONNECTOR_H

#include <pallette/InetAddress.h>
#include <pallette/noncopyable.h>
#include <pallette/TimerId.h>

#include <functional>
#include <memory>
#include <atomic>

namespace pallette
{
    class Channel;
    class EventLoop;

    //Connector用于发起连接，并且带有自动重连功能
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

        void start();//任何线程都能调用
        void restart();//当前线程调用
        void stop();//任何线程都能调用

        const InetAddress& serverAddress() const { return serverAddr_; }

    private:
        enum States { kDisconnected, kConnecting, kConnected };

        void setState(States s) { state_ = s; }
        void startInLoop();
        void stopInLoop();
        void connect();//实际调用connect函数
        void connecting(int sockfd);
        void handleWrite();
        void handleError();
        void retry(int sockfd);//重连函数
        int removeAndResetChannel();//移除channel并且释放
        void resetChannel();//释放channel

        EventLoop* loop_;
        InetAddress serverAddr_;//服务器地址
        std::atomic<bool> connect_;
        std::atomic<States> state_;//当前连接所属状态，状态机
        std::unique_ptr<Channel> channel_;
        NewConnectionCallback newConnectionCallback_;//连接上后的用户回调
        int retryDelayMs_;//重连时间
        bool retryConnector_;//是否有重连事件
        TimerId retryConnectorTimerId_;//控制定时器的句柄

        static const int kMaxRetryDelayMs = 30 * 1000;
        static const int kInitRetryDelayMs = 500;
    };
}

#endif
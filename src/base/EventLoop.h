/*************************************************************************
> File Name: EventLoop.h
> Author: pengjunjie
> Mail: 1002398145@qq.com
> Created Time: 2019年01月06日 星期日 21时22分58秒
************************************************************************/

#ifndef PALLETTE_EVENTLOOP_H
#define PALLETTE_EVENTLOOP_H

#include "noncopyable.h"
#include "Timestamp.h"
#include "any.hpp"
#include "ProcessInfo.h"
#include "TimerId.h"
#include "Callbacks.h"

#include<atomic>
#include <memory>
#include <mutex>
#include <functional>

namespace pallette
{
    class Channel;
    class EpollPoller;
    class TimerQueue;

    //一个线程一个事件循环，EventLoop主要功能是运行事件循环，等待事件发生然后调用回调函数处理发生事件
    //EventLoop生命周期与所在线程相同，将拥有EventLoop的线程称为IO线程，它只关心socket fd的IO
    //EventLoop不知道自己有多少的Channel，它只关心发生了事件的Channel，并将其存储到activeChannels_
    class EventLoop : noncopyable
    {
    public:
        typedef std::function<void()> Functor;

        EventLoop();
        ~EventLoop();

        void loop();//事件循环,执行epoll_wait操作
        void quit();//退出

        Timestamp pollReturnTime() const { return pollReturnTime_; }
        int64_t iteration() const { return iteration_; }

        void runInLoop(Functor cb);//让EventLoop在其owner线程执行cb函数
        void queueInLoop(Functor cb);
        size_t queueSize() const;

        TimerId runAfter(double delay, TimerCallback cb);//在delay秒后执行cb函数
        TimerId runEvery(double interval, TimerCallback cb);//每隔interval秒后执行cb函数
        void cancel(TimerId timerId);//取消定时器

        void wakeup();//唤醒当前EventLoop线程
        void updateChannel(Channel* channel);//更新或添加channel到EpollPoller
        void removeChannel(Channel* channel);//从EopllPoller中删除channel
        bool hasChannel(Channel* channel);

        void assertInLoopThread();
        bool isInLoopThread() const { return static_cast<pid_t>(threadId_) == process_info::tid(); }
        bool eventHandling() const { return eventHandling_; }

        void setContext(const any& context) { context_ = context; }
        const any& getContext() const { return context_; }
        any* getMutableContext() { return &context_; }

        static EventLoop* getEventLoopOfCurrentThread();

    private:
        typedef std::vector<Channel*> ChannelList;

        void abortNotInLoopThread();
        void handleRead();
        void doPendingFunctors();//执行用户任务

        bool looping_;
        std::atomic<bool> quit_;
        std::atomic<bool> eventHandling_;//就绪事件处理标志
        std::atomic<bool> callingPendingFunctors_;//执行用户任务标志
        int64_t iteration_;
        const size_t threadId_;
        Timestamp pollReturnTime_;
        std::unique_ptr<EpollPoller> poller_;//多路复用IO,用指针是因为只有对EpollPoller的前向声明
        std::unique_ptr<TimerQueue> timerQueue_;
        int wakeupFd_;//eventfd，用来唤醒本线程的文件描述符
        std::unique_ptr<Channel> wakeupChannel_;
        any context_;

        ChannelList activeChannels_;//就绪事件集合
        Channel* currentActiveChannel_;//当前处理的就绪事件

        mutable std::mutex mutex_;
        std::vector<Functor> pendingFunctors_;//存放用户任务回调
    };
}

#endif

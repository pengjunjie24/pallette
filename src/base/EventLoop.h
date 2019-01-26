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

#include<atomic>
#include <memory>
#include <mutex>
#include <functional>

namespace pallette
{
    class Channel;
    class EpollPoller;

    class EventLoop : noncopyable
    {
    public:
        typedef std::function<void()> Functor;

        EventLoop();
        ~EventLoop();

        void loop();
        void quit();

        Timestamp pollReturnTime() const { return pollReturnTime_; }
        int64_t iteration() const { return iteration_; }

        void runInLoop(Functor cb);
        void queueInLoop(Functor cb);
        size_t queueSize() const;

        void wakeup();
        void updateChannel(Channel* channel);
        void removeChannel(Channel* channel);
        bool hasChannel(Channel* channel);

        void assertInLoopThread();
        bool isInLoopThread() const { return threadId_ == process_info::tid(); }
        bool eventHandling() const { return eventHandling_; }

        void setContext(const any& context) { context_ = context; }
        const any& getContext() const { return context_; }
        any* getMutableContext() { return &context_; }

        static EventLoop* getEventLoopOfCurrentThread();

    private:
        typedef std::vector<Channel*> ChannelList;

        void abortNotInLoopThread();
        void handleRead();
        void doPendingFunctors();

        bool looping_;
        std::atomic<bool> quit_;
        std::atomic<bool> eventHandling_;
        std::atomic<bool> callingPendingFunctors_;
        int64_t iteration_;
        const size_t threadId_;
        Timestamp pollReturnTime_;
        std::unique_ptr<EpollPoller> poller_;
        int wakeupFd_;
        std::unique_ptr<Channel> wakeupChannel_;
        any context_;

        ChannelList activeChannels_;
        Channel* currentActiveChannel_;

        mutable std::mutex mutex_;
        std::vector<Functor> pendingFunctors_;
    };
}

#endif

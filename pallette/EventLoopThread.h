

#ifndef PALLETTE_EVENTLOOPTHREAD_H
#define PALLETTE_EVENTLOOPTHREAD_H

#include <pallette/noncopyable.h>
#include <pallette/Thread.h>

#include <functional>
#include <mutex>
#include <condition_variable>

namespace pallette
{
    class EventLoop;

    //IO线程类，在线程函数中创建一个EventLoop对象并调用EventLoop::loop(IO线程不一定是主线程)
    class EventLoopThread : noncopyable
    {
    public:
        typedef std::function<void(EventLoop*)> ThreadInitCallback;

        EventLoopThread(const ThreadInitCallback& cb = ThreadInitCallback(),
            const std::string& name = std::string());
        ~EventLoopThread();
        EventLoop* startLoop();//在其他线程调用

    private:
        void threadFunc();//本线程调用loop()

        EventLoop* loop_;
        bool exiting_;
        pallette::Thread thread_;
        std::mutex mutex_;
        std::condition_variable  cond_;
        ThreadInitCallback callback_;
    };

}

#endif

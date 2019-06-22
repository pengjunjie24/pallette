/*************************************************************************
> File Name: EventLoopThread.h
> Author: pengjunjie
> Mail: 1002398145@qq.com
> Created Time: 2019年01月15日 星期二 20时41分19秒
************************************************************************/

#ifndef PALLETTE_EVENTLOOP_THREAD_H
#define PALLETTE_EVENTLOOP_THREAD_H

#include "noncopyable.h"

#include <memory>
#include <string>
#include <thread>
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

        EventLoopThread(const ThreadInitCallback& cb = ThreadInitCallback());
        ~EventLoopThread();
        EventLoop* startLoop();//在其他线程调用

    private:
        void threadFunc();//本线程调用loop()

        EventLoop* loop_;
        bool exiting_;
        std::thread thread_;
        std::mutex mutex_;
        std::condition_variable cond_;
        ThreadInitCallback callback_;//线程初始化函数
    };
}

#endif
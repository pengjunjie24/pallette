/*************************************************************************
> File Name: EventLoopThreadPool.h
> Author: pengjunjie
> Mail: 1002398145@qq.com
> Created Time: 2019年01月15日 星期二 21时12分50秒
************************************************************************/

#ifndef PALLETTE_EVENTLOOP_THREADPOOL_H
#define PALLETTE_EVENTLOOP_THREADPOOL_H

#include "noncopyable.h"

#include <functional>
#include <memory>
#include <vector>

namespace pallette
{
    class EventLoop;
    class EventLoopThread;

    //IO线程池类，开启若干个IO线程，并让其处于事件循环
    class EventLoopThreadPool : noncopyable
    {
    public:
        typedef std::function<void(EventLoop*)> ThreadInitCallback;

        EventLoopThreadPool(EventLoop* baseLoop);
        ~EventLoopThreadPool();
        void setThreadNum(int numThreads) { numThreads_ = numThreads; }//设置sub线程数
        void start(const ThreadInitCallback& cb = ThreadInitCallback());

        EventLoop* getNextLoop();//得到一个IO线程句柄
        std::vector<EventLoop*> getAllLoops();//得到所有IO线程句柄

        bool started() const { return started_; }

    private:
        EventLoop* baseLoop_;//IO线程池对象所属的IO线程
        bool started_;
        int numThreads_;//sub线程数，除去了baseLoop_
        int next_;
        std::vector<std::unique_ptr<EventLoopThread>> threads_;//存放IO线程
        std::vector<EventLoop*> loops_;
    };
}

#endif
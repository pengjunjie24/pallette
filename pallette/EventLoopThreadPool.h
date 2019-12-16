
#ifndef PALLETTE_EVENTLOOPTHREADPOOL_H
#define PALLETTE_EVENTLOOPTHREADPOOL_H

#include <pallette/noncopyable.h>

#include <vector>
#include <functional>
#include <memory>
#include <string>

namespace pallette
{
    class EventLoop;
    class EventLoopThread;

    //IO线程池类，开启若干个IO线程，并让其处于事件循环
    class EventLoopThreadPool : noncopyable
    {
    public:
        typedef std::function<void(EventLoop*)> ThreadInitCallback;

        EventLoopThreadPool(EventLoop* baseLoop, const std::string& nameArg);
        ~EventLoopThreadPool();
        void setThreadNum(int numThreads) { numThreads_ = numThreads; }//设置sub线程数
        void start(const ThreadInitCallback& cb = ThreadInitCallback());

        EventLoop* getNextLoop();//得到一个IO线程句柄(轮流获取)
        EventLoop* getLoopForHash(size_t hashCode);//得到一个IO线程句柄(通过hash值)
        std::vector<EventLoop*> getAllLoops();//得到所有IO线程句柄

        bool started() const { return started_; }

    private:
        EventLoop* baseLoop_;//IO线程池对象所属的IO线程
        std::string name_;
        bool started_;
        int numThreads_;//sub线程数，除去了baseLoop_
        int next_;
        std::vector<std::unique_ptr<EventLoopThread>> threads_;//存放IO线程
        std::vector<EventLoop*> loops_;
    };
}

#endif
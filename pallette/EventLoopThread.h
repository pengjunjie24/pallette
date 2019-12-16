

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

    //IO�߳��࣬���̺߳����д���һ��EventLoop���󲢵���EventLoop::loop(IO�̲߳�һ�������߳�)
    class EventLoopThread : noncopyable
    {
    public:
        typedef std::function<void(EventLoop*)> ThreadInitCallback;

        EventLoopThread(const ThreadInitCallback& cb = ThreadInitCallback(),
            const std::string& name = std::string());
        ~EventLoopThread();
        EventLoop* startLoop();//�������̵߳���

    private:
        void threadFunc();//���̵߳���loop()

        EventLoop* loop_;
        bool exiting_;
        pallette::Thread thread_;
        std::mutex mutex_;
        std::condition_variable  cond_;
        ThreadInitCallback callback_;
    };

}

#endif

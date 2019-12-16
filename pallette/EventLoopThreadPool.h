
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

    //IO�̳߳��࣬�������ɸ�IO�̣߳������䴦���¼�ѭ��
    class EventLoopThreadPool : noncopyable
    {
    public:
        typedef std::function<void(EventLoop*)> ThreadInitCallback;

        EventLoopThreadPool(EventLoop* baseLoop, const std::string& nameArg);
        ~EventLoopThreadPool();
        void setThreadNum(int numThreads) { numThreads_ = numThreads; }//����sub�߳���
        void start(const ThreadInitCallback& cb = ThreadInitCallback());

        EventLoop* getNextLoop();//�õ�һ��IO�߳̾��(������ȡ)
        EventLoop* getLoopForHash(size_t hashCode);//�õ�һ��IO�߳̾��(ͨ��hashֵ)
        std::vector<EventLoop*> getAllLoops();//�õ�����IO�߳̾��

        bool started() const { return started_; }

    private:
        EventLoop* baseLoop_;//IO�̳߳ض���������IO�߳�
        std::string name_;
        bool started_;
        int numThreads_;//sub�߳�������ȥ��baseLoop_
        int next_;
        std::vector<std::unique_ptr<EventLoopThread>> threads_;//���IO�߳�
        std::vector<EventLoop*> loops_;
    };
}

#endif
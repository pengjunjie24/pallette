/*************************************************************************
> File Name: EventLoopThreadPool.cpp
> Author: pengjunjie
> Mail: 1002398145@qq.com
> Created Time: 2019年01月15日 星期二 21时13分04秒
************************************************************************/

#include "EventLoopThreadPool.h"
#include "EventLoopThread.h"
#include "EventLoop.h"
#include <assert.h>

using namespace pallette;

EventLoopThreadPool::EventLoopThreadPool(EventLoop* baseLoop)
:baseLoop_(baseLoop),
started_(false),
numThreads_(0),
next_(0)
{
}

EventLoopThreadPool::~EventLoopThreadPool()
{
}

void EventLoopThreadPool::start(const ThreadInitCallback& cb)
{
    assert(!started_);
    baseLoop_->assertInLoopThread();

    started_ = true;

    for (int i = 0; i < numThreads_; ++i)
    {
        EventLoopThread* t = new EventLoopThread(cb);
        threads_.push_back(std::unique_ptr<EventLoopThread>(t));
        loops_.push_back(t->startLoop());//启动EventLoopThread线程
    }
    if (numThreads_ == 0 && cb)
    {
        cb(baseLoop_);
    }
}

EventLoop* EventLoopThreadPool::getNextLoop()
{
    baseLoop_->assertInLoopThread();//只能在IO线程池对象所在IO线程拿到IO线程池中线程
    assert(started_);
    EventLoop* loop = baseLoop_;//当线程池中线程个数为0，返回IO线程池对象所在IO线程

    if (!loops_.empty())
    {
        loop = loops_[next_];
        ++next_;
        if (static_cast<size_t>(next_) >= loops_.size())
        {
            next_ = 0;
        }
    }
    return loop;
}
std::vector<EventLoop*> EventLoopThreadPool::getAllLoops()
{
    baseLoop_->assertInLoopThread();
    assert(started_);
    if (loops_.empty())
    {
        return std::vector<EventLoop*>(1, baseLoop_);
    }
    else
    {
        return loops_;
    }
}
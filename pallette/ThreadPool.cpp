/*************************************************************************
> File Name: ThreadPool.cpp
> Author: pengjunjie
> Mail: 1002398145@qq.com
> Created Time: 2018年12月23日 星期日 23时51分32秒
************************************************************************/

#include <pallette/ThreadPool.h>

#include <exception>
#include <stdio.h>
#include <assert.h>

using namespace pallette;

ThreadPool::ThreadPool(const std::string& nameArg)
: name_(nameArg)
, maxQueueSize_(0)
, running_(false)
{
}

ThreadPool::~ThreadPool()
{
    if (running_)
    {
        stop();
    }
}

void ThreadPool::start(int numThreads)
{
    assert(threads_.empty());
    running_ = true;
    threads_.reserve(numThreads);

    for (int i = 0; i < numThreads; ++i)
    {
        char id[32] = { 0 };
        snprintf(id, sizeof id, "%d", i + 1);
        threads_.emplace_back(new Thread(std::bind(
            &ThreadPool::runInThread, this), name_ + id));
        threads_[i]->start();
    }
    if (numThreads == 0 && threadInitCallback_)
    {
        threadInitCallback_();
    }
}

void ThreadPool::stop()
{
    {
        std::lock_guard<std::mutex> lock(mutex_);
        running_ = false;
        notEmpty_.notify_all();
    }
    for (auto& thr : threads_)
    {
        thr->join();
    }
}

size_t ThreadPool::queueSize() const
{
    std::lock_guard<std::mutex> lock(mutex_);
    return queue_.size();
}

void ThreadPool::run(task func)
{
    if (threads_.empty())
    {
        func();
    }
    else
    {
        std::unique_lock <std::mutex> lock(mutex_);
        while (isFull())
        {
            notFull_.wait(lock);
        }
        assert(!isFull());

        queue_.push_back(std::move(func));
        notEmpty_.notify_one();
    }
}

bool ThreadPool::isFull() const
{
    return maxQueueSize_ > 0 && queue_.size() >= maxQueueSize_;
}

void ThreadPool::runInThread()
{
    try
    {
        if (threadInitCallback_)
        {
            threadInitCallback_();
        }
        while (running_)
        {
            task threadTask(take());
            if (threadTask)
            {
                threadTask();
            }
        }
    }
    catch (const std::exception& ex)
    {
        fprintf(stderr, "exception caught in ThreadPool %s\n", name_.c_str());
        fprintf(stderr, "reason: %s\n", ex.what());
        abort();
    }
    catch (...)
    {
        fprintf(stderr, "unknown exception caught in ThreadPool %s\n", name_.c_str());
        throw;
    }
}

ThreadPool::task ThreadPool::take()
{
    std::unique_lock <std::mutex> lock(mutex_);
    while (queue_.empty() && running_)
    {
        notEmpty_.wait(lock);
    }

    task threadTask;
    if (!queue_.empty())
    {
        threadTask = queue_.front();
        queue_.pop_front();
        if (maxQueueSize_ > 0)
        {
            notFull_.notify_one();
        }
    }

    return threadTask;
}
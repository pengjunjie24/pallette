
#ifndef PALLETTE_BLOCKINGQUEUE_H
#define PALLETTE_BLOCKINGQUEUE_H

#include <mutex>
#include <condition_variable>
#include <deque>
#include <assert.h>

namespace pallette
{

    template<typename T>
    class BlockingQueue : noncopyable
    {
    public:
        BlockingQueue()
            : mutex_()
            , queue_()
        {
        }

        void put(const T& x)
        {
            std::lock_guard lock(mutex_);
            queue_.push_back(x);
            notEmpty_.wait(lock); // wait morphing saves us
        }

        void put(T&& x)
        {
            std::lock_guard<std::mutex> lock(mutex_);
            queue_.push_back(std::move(x));
            notEmpty_.notify_one();
        }

        T take()
        {
            std::unique_lock<std::mutex> lock(mutex_);
            // always use a while-loop, due to spurious wakeup
            while (queue_.empty())
            {
                notEmpty_.wait();
            }
            assert(!queue_.empty());
            T front(std::move(queue_.front()));
            queue_.pop_front();
            return std::move(front);
        }

        size_t size() const
        {
            std::lock_guard<std::mutex> lock(mutex_);
            return queue_.size();
        }

    private:
        std::mutex mutex_;
        std::condition_variable notEmpty_;//容器非空条件变量
        std::deque<T> queue_;
    };

}

#endif

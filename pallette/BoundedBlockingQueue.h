
#ifndef PALLETTE_BOUNDEDBLOCKINGQUEUE_H
#define PALLETTE_BOUNDEDBLOCKINGQUEUE_H

#include <pallette/CircularBuffer.hpp>

#include <mutex>
#include <condition_variable>
#include <assert.h>

namespace pallette
{
    template<typename T>
    class BoundedBlockingQueue : noncopyable
    {
    public:
        explicit BoundedBlockingQueue(int maxSize)
            : mutex_()
            , notEmpty_(mutex_)
            , notFull_(mutex_)
            , queue_(maxSize)
        {
        }

        void put(const T& x)
        {
            std::unique_lock<std::mutex> lock(mutex_);
            while (queue_.full())
            {
                notFull_.wait();
            }
            assert(!queue_.full());
            queue_.push_back(x);
            notEmpty_.notify_one();
        }

        T take()
        {
            std::unique_lock<std::mutex> lock(mutex_);
            while (queue_.empty())
            {
                notEmpty_.wait();
            }
            assert(!queue_.empty());
            T front(queue_.front());
            queue_.pop_front();
            notFull_.notify_one();
            return front;
        }

        bool empty() const
        {
            std::lock_guard<std::mutex> lock(mutex_);
            return queue_.empty();
        }

        bool full() const
        {
            std::lock_guard<std::mutex> lock(mutex_);
            return queue_.full();
        }

        size_t size() const
        {
            std::lock_guard<std::mutex> lock(mutex_);
            return queue_.size();
        }

    private:
        std::mutex mutex_;
        std::condition_variable notEmpty_;
        std::condition_variable notFull_;
        pallette::CircularBuffer<T> queue_;
    };

}

#endif

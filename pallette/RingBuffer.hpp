
#ifndef PALLETTE_RING_BUFFER_HPP
#define PALLETTE_RING_BUFFER_HPP

#include <pallette/noncopyable.h>

#include <assert.h>
#include <deque>
namespace pallette
{
    //环形队列，底层数据结构为deque
    template <typename T>
    class RingBuffer : noncopyable
    {
    public:
        explicit RingBuffer(int size,
            T value __attribute__((unused)) = T())
            : size_(size)
            , buffer_(size)
        {
            assert(size > 0);
        }

        size_t size()const { return size_; }
        T& operator[](int index) { return buffer_[index]; }
        T& front() { return buffer_.front(); }
        T& back() { return buffer_.back(); }
        bool empty()const { return size() == 0; }
        bool full()const { return size() == size_; }

        void push_back(const T& value)
        {
            if (buffer_.size() < size_)
            {
                buffer_.push_back(value);
            }
            else
            {
                buffer_.pop_front();
                buffer_.push_back(value);
            }
        }

        void push_front(const T& value)
        {
            if (buffer_.size() < size_)
            {
                buffer_.push_front(value);
            }
            else
            {
                buffer_.pop_back();
                buffer_.push_front(value);
            }
        }

        void resize(int newSize, T value = T())
        {
            assert(newSize > 0);
            buffer_.resize(newSize);
            size_ = newSize;
        }

    private:
        size_t size_;//当前实际写入元素数量
        std::deque<T> buffer_;
    };
}

#endif
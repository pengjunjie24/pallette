
#ifndef PALLETTE_Circular_BUFFER_HPP
#define PALLETTE_Circular_BUFFER_HPP

#include <pallette/noncopyable.h>

#include <assert.h>
#include <deque>
namespace pallette
{
    //环形队列，底层数据结构为deque
    template <typename T>
    class CircularBuffer : noncopyable
    {
    public:
        explicit CircularBuffer(int maxSize)
            : maxSize_(maxSize)
        {
            assert(maxSize_ > 0);
        }

        size_t size()const { return buffer_.size(); }
        T& operator[](int index) { return buffer_[index]; }
        T& front() { return buffer_.front(); }
        T& back() { return buffer_.back(); }
        bool empty()const { return size() == 0; }
        bool full()const { return size() == static_cast<size_t>(maxSize_); }

        void push_back(const T& value)
        {
            if (buffer_.size() < static_cast<size_t>(maxSize_))
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
            if (buffer_.size() < static_cast<size_t>(maxSize_))
            {
                buffer_.push_front(value);
            }
            else
            {
                buffer_.pop_back();
                buffer_.push_front(value);
            }
        }

        void pop_back() { buffer_.pop_back(); }
        void pop_front() { buffer_.pop_front(); }

    private:
        const int maxSize_;//环形队列最大个数
        std::deque<T> buffer_;
    };
}

#endif
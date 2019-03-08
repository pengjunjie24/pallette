/*************************************************************************
> File Name: Timer.h
> Author: pengjunjie
> Mail: 1002398145@qq.com
> Created Time: 2019年01月30日 星期三 16时27分34秒
************************************************************************/

#ifndef PALLETTE_TIMER_H
#define PALLETTE_TIMER_H

#include "Timestamp.h"
#include "Callbacks.h"
#include "noncopyable.h"

#include <atomic>

namespace pallette
{
    class Timer : noncopyable
    {
    public:
        Timer(TimerCallback cb, Timestamp when, double interval)
            : callback_(std::move(cb)),
            expiration_(when),
            interval_(interval),
            repeat_(interval > 0.0),
            sequence_(sNumCreated_++)
        {
        }

        void run() const { callback_(); }

        Timestamp expiration() const { return expiration_; }
        bool repeat() const { return repeat_; }
        int64_t sequence() const { return sequence_; }

        void restart(Timestamp now);

        static int64_t numCreated() { return sNumCreated_.load(); }

    private:
        const TimerCallback callback_;
        Timestamp expiration_;
        const double interval_;
        const bool repeat_;
        const int64_t sequence_;

        static std::atomic<int64_t> sNumCreated_;
    };
}


#endif
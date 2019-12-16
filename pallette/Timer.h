/*************************************************************************
> File Name: Timer.h
> Author: pengjunjie
> Mail: 1002398145@qq.com
> Created Time: 2019年01月30日 星期三 16时27分34秒
************************************************************************/

#ifndef PALLETTE_TIMER_H
#define PALLETTE_TIMER_H

#include <pallette/Timestamp.h>
#include <pallette/Callbacks.h>
#include <pallette/noncopyable.h>

#include <atomic>

namespace pallette
{
    //Timer是对定时器的抽象，封装了超时回调，超时时间，定时器是否重复，定时器序号等
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

        void run() const { callback_(); }//调用超时回调

        Timestamp expiration() const { return expiration_; }
        bool repeat() const { return repeat_; }
        int64_t sequence() const { return sequence_; }

        void restart(Timestamp now);//重新启动定时器

        static int64_t numCreated() { return sNumCreated_.load(); }

    private:
        const TimerCallback callback_;//定时器的回调函数
        Timestamp expiration_;//下一次超时时刻
        const double interval_;//超时时间间隔，为0则是一次定时器
        const bool repeat_;//是否重复
        const int64_t sequence_;//定时器序号

        static std::atomic<int64_t> sNumCreated_;//维护定时器id唯一
    };
}


#endif
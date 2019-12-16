/*************************************************************************
> File Name: TimerId.h
> Author: pengjunjie
> Mail: 1002398145@qq.com
> Created Time: 2019年01月30日 星期三 16时59分01秒
************************************************************************/

#ifndef PALLETTE_TIMERID_H
#define PALLETTE_TIMERID_H

namespace pallette
{
    class Timer;

    //定时器句柄，用于删除定时器
    class TimerId
    {
    public:
        TimerId()
            : timer_(NULL)
            , sequence_(0)
        {
        }

        TimerId(Timer* timer, int64_t seq)
            : timer_(timer)
            , sequence_(seq)
        {
        }

        friend class TimerQueue;

    private:
        Timer* timer_;
        int64_t sequence_;
    };
}

#endif
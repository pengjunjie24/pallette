/*************************************************************************
> File Name: TimerQueue.h
> Author: pengjunjie
> Mail: 1002398145@qq.com
> Created Time: 2019年01月30日 星期三 16时57分05秒
************************************************************************/

#ifndef PALLETTE_TIMERQUEUE_H
#define PALLETTE_TIMERQUEUE_H

#include "noncopyable.h"
#include "Callbacks.h"
#include "Channel.h"

#include <atomic>
#include <set>
#include <list>

namespace pallette
{
    class EventLoop;
    class Timer;
    class TimerId;

    class TimerQueue : noncopyable
    {
    public:
        explicit TimerQueue(EventLoop* loop);
        ~TimerQueue();

        TimerId addTimer(TimerCallback cb, Timestamp when, double interval);
        void cancel(TimerId timerId);

    private:
        typedef std::pair<Timestamp, Timer*> Entry;
        typedef std::set<Entry> TimerSet;
        typedef std::list<Timer*> TimerList;

        void addTimerInLoop(Timer* timer);
        void cancelInLoop(TimerId timerId);
        void handleRead();

        bool insert(Timer* timer);//插入定时器
        void getExpired(Timestamp now);//返回时间已到的定时器
        void reset(Timestamp now);//重置定时器

        EventLoop* loop_;
        const int timerfd_;
        Channel timerfdChannel_;
        TimerSet timers_;//等待唤醒的定时器
        TimerList activeTimers_;//当前激活的定时器
        Timer* currentActiveTimer_;//当前正在处理的定时器事件
        std::atomic<bool> callingExpiredTimers_;
        TimerList waitTimers_;//同timers_数量一致
        TimerList cancelingTimers_;
    };
}

#endif
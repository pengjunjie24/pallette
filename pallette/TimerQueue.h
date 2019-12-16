/*************************************************************************
> File Name: TimerQueue.h
> Author: pengjunjie
> Mail: 1002398145@qq.com
> Created Time: 2019年01月30日 星期三 16时57分05秒
************************************************************************/

#ifndef PALLETTE_TIMERQUEUE_H
#define PALLETTE_TIMERQUEUE_H

#include <pallette/noncopyable.h>
#include <pallette/Callbacks.h>
#include <pallette/Channel.h>

#include <atomic>
#include <set>
#include <list>

namespace pallette
{
    class EventLoop;
    class Timer;
    class TimerId;

    //TimerQueue的封装是为了让未到期的时间Timer有序的排列起来，
    //能够根据当前时间找到已经到期的Timer也能高效的添加和删除Timer
    class TimerQueue : noncopyable
    {
    public:
        explicit TimerQueue(EventLoop* loop);
        ~TimerQueue();

        TimerId addTimer(TimerCallback cb, Timestamp when, double interval);//通常在其他线程被调用
        void cancel(TimerId timerId);

    private:
        typedef std::pair<Timestamp, Timer*> Entry;
        typedef std::set<Entry> TimerSet;
        typedef std::list<Timer*> TimerList;

        //服务器性能杀手之一就是锁竞争，以下成员函数只能在所属的IO线程调用
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
        TimerList cancelingTimers_;//待删除的定时器
        TimerList freeTimers_;//被删除的定时器
    };
}

#endif
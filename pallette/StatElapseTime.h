
#pragma once

#include <pallette/Timestamp.h>

#include <functional>

//统计程序消耗时长
namespace pallette
{
    //单次统计时间差
    class AutoStatElapsedTimeus
    {
    public:
        AutoStatElapsedTimeus(int64_t& elapsedTime)
            : startTimeus_(Timestamp::now())
            , elapsedTimeus_(elapsedTime)
        {
        }

        ~AutoStatElapsedTimeus()
        {
            elapsedTimeus_ = static_cast<int64_t>(
                timeDifference(Timestamp::now(), startTimeus_) * Timestamp::kMicroSecondsPerSecond);
        }

    private:
        Timestamp startTimeus_;//开始时间
        int64_t& elapsedTimeus_;//消耗时间
    };

    //持续统计时间差, 线程不安全
    class StatElapsedTimeus
    {
    public:
        StatElapsedTimeus()
            : elapsedTimeus_(0)
            , maxElapsedTimeus_(0)
        {
        }

        //XXX 传入func返回值只能是void
        void execStatElapsedTime(std::function<void()> func)
        {
            {
                AutoStatElapsedTimeus autoStatElapsed(elapsedTimeus_);
                func();
            }

            //获取最大消耗时长
            maxElapsedTimeus_ = (elapsedTimeus_ > maxElapsedTimeus_) ?
                elapsedTimeus_ : maxElapsedTimeus_;
        }

        int64_t getElapsedTimeus() const { return elapsedTimeus_; }
        int64_t getMaxElapsedTimeus() const { return maxElapsedTimeus_; }


    private:
        int64_t elapsedTimeus_;//统计函数执行过程中消耗时长
        int64_t maxElapsedTimeus_;//记录执行过程中最大消耗时长
    };
}
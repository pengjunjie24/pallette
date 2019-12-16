/*************************************************************************
> File Name: Timer.cpp
> Author: pengjunjie
> Mail: 1002398145@qq.com
> Created Time: 2019年01月30日 星期三 16时27分39秒
************************************************************************/

#include <pallette/Timer.h>

using namespace pallette;

std::atomic<int64_t> Timer::sNumCreated_;

void Timer::restart(Timestamp now)
{
    if (repeat_)
    {
        expiration_ = addTime(now, interval_);
    }
    else
    {
        expiration_ = Timestamp();
    }
}
/*************************************************************************
  > File Name: CountDownLatch.cpp
  > Author: pengjunjie
  > Mail: 1002398145@qq.com
  > Created Time: 2018年12月30日 星期日 19时27分55秒
 ************************************************************************/

#include <pallette/CountDownLatch.h>

using namespace pallette;

CountDownLatch::CountDownLatch(int count)
    :count_(count)
{
}

void CountDownLatch::wait()
{
    std::unique_lock<std::mutex> lock(mutex_);
    while (count_ > 0)
    {
        condition_.wait(lock);
    }
}

void CountDownLatch::countDown()
{
    std::lock_guard<std::mutex> lock(mutex_);
    --count_;
    if (count_ == 0)
    {
        condition_.notify_all();
    }
}

int CountDownLatch::getCount() const
{
    std::lock_guard<std::mutex> lock(mutex_);
    return count_;
}
/*************************************************************************
  > File Name: CountDownLatch.h
  > Author: pengjunjie
  > Mail: 1002398145@qq.com
  > Created Time: 2018年12月30日 星期日 19时27分45秒
 ************************************************************************/

#ifndef PALLETTE_COUNTDOWNLATCH_H
#define PALLETTE_COUNTDOWNLATCH_H

#include <pallette/noncopyable.h>

#include <mutex>
#include <condition_variable>

namespace pallette
{
    //线程间安全的计数器
    class CountDownLatch : noncopyable
    {
    public:
        explicit CountDownLatch(int count);

        void wait();
        void countDown();
        int getCount() const;

    private:
        mutable std::mutex mutex_;
        std::condition_variable condition_;
        int count_;
    };
}

#endif

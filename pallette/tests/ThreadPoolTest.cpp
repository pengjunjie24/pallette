/*************************************************************************
  > File Name: ThreadPoolTest.cpp
  > Author: pengjunjie
  > Mail: 1002398145@qq.com
  > Created Time: 2018年12月24日 星期一 00时49分47秒
 ************************************************************************/

#include <pallette/ThreadPool.h>
#include <pallette/Logging.h>
#include <pallette/CountDownLatch.h>
#include <pallette/CurrentThread.h>

#include <stdio.h>
#include <unistd.h>
#include <sys/types.h>

using namespace std;
using namespace pallette;

void print()
{
    LOG_INFO << "tid = " << current_thread::tid();
}

void printString(const std::string& str)
{
    LOG_INFO << str;
    usleep(100 * 1000);
}

void test(int maxSize)
{
    LOG_INFO << "Test ThreadPool with max queue size = " << maxSize;
    ThreadPool pool("MainThreadPool");
    pool.setMaxQueueSize(maxSize);
    pool.start(5);

    LOG_INFO << "Adding";
    pool.run(print);
    pool.run(print);
    for (int i = 0; i < 100; ++i)
    {
        char buf[32];
        snprintf(buf, sizeof(buf), "task %d", i);
        pool.run(std::bind(printString, std::string(buf)));
    }
    LOG_WARN << "Done";

    CountDownLatch latch(1);
    pool.run(bind(&CountDownLatch::countDown, &latch));
    latch.wait();

    pool.stop();
}

int main()
{
    test(0);
    test(1);
    test(5);
    test(10);
    test(50);
}
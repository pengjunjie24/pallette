/*************************************************************************
  > File Name: ThreadPoolTest.cpp
  > Author: pengjunjie
  > Mail: 1002398145@qq.com
  > Created Time: 2018年12月24日 星期一 00时49分47秒
 ************************************************************************/

#include "../ThreadPool.h"
#include "../Logging.h"
#include "../CountDownLatch.h"

#include <stdio.h>
#include <unistd.h>
#include <sys/types.h>

using namespace std;
using namespace pallette;

void print()
{
    std::thread::id id = std::this_thread::get_id();
    LOG_INFO << "tid = " << std::hash<std::thread::id>()(id);
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
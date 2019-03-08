/*************************************************************************
  > File Name: loger_test.cpp
  > Author: pengjunjie
  > Mail: 1002398145@qq.com
  > Created Time: 2018年06月16日 星期六 11时00分53秒
 ************************************************************************/

#include "../logging.h"
#include "../ThreadPool.h"
#include "../CountDownLatch.h"

#include <functional>

using namespace pallette;
void LogInThread()
{
    LOG_INFO << "logInThread";

    int n = 100;
    for (int i = 0; i < n; ++i)
    {
        LOG_INFO << "task " << i;
    }
}

int main()
{
    ThreadPool pool("pool");
    pool.start(5);
    pool.run(LogInThread);
    pool.run(LogInThread);
    pool.run(LogInThread);
    pool.run(LogInThread);
    pool.run(LogInThread);

    CountDownLatch latch(1);
    pool.run(std::bind(&CountDownLatch::countDown, &latch));
    latch.wait();

    pool.stop();

    return 0;
}
/*************************************************************************
	> File Name: DaemonTest.cpp
	> Author: pengjunjie
	> Mail: 1002398145@qq.com
	> Created Time: 2019年03月19日 星期二 19时15分23秒
 ************************************************************************/

#include "../Daemon.h"
#include "../AsyncLogging.h"
#include "../Logging.h"
#include "../ThreadPool.h"

#include <functional>
#include <stdio.h>
#include <time.h>
#include <sys/stat.h>
#include <unistd.h>

using namespace pallette;

AsyncLogging* gAsyncLog = NULL;

void runInThread()
{
    LOG_INFO << "logInThread";

    int n = 10000;
    bool kLongLog = true;
    for (int i = 0; i < n; ++i)
    {
        kLongLog = true;
        LOG_DEBUG << "Hello DEBUG 0245" << kLongLog << i;
        kLongLog = false;
        LOG_WARN << "Hello WARN 1244" << kLongLog << i;
        sleep(1);
    }
}

void outputFunc(const char* msg, int len)
{
    gAsyncLog->append(msg, len);
}

int main()
{
    if (daemonInit("_runtime"))
    {
        fprintf(stderr, "daemonInit error\n");
    }

    const long kRollSize = 512 * 1024 * 1024;//滚动长度设为512M

    std::string dirname = "log";
    if (mkdir(dirname.c_str(), 0755))
    {
        if (errno != EEXIST)
        {
            fprintf(stderr, "mkdir error\n");
            return -1;
        }
    }

    AsyncLogging asyncLogging(dirname, kRollSize);
     gAsyncLog = &asyncLogging;
    Logging::setOutput(outputFunc);
    asyncLogging.start();
    Logging::setLogLevel(Logging::DEBUG);

    LOG_INFO << "logInMainThread";

    ThreadPool pool;
    pool.start(6);

    for (int i = 0; i < 50; ++i)
    {
        pool.run(runInThread);
    }

    sleep(100);
}

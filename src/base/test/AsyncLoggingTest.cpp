/*************************************************************************
> File Name: AsyncLoggingTest.cpp
> Author: pengjunjie
> Mail: 1002398145@qq.com
> Created Time: 2019年01月01日 星期二 18时22分05秒
************************************************************************/


#include "../AsyncLogging.h"
#include "../Logging.h"
#include "../ThreadPool.h"

#include <unistd.h>

using namespace pallette;

const long kRollSize = 512 * 1024 * 1024;//滚动长度设为512M
const char* kFilename = ".";//日志路径

pallette::AsyncLogging gAsyncLogging(kFilename, kRollSize);

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
    }
}

void outputFunc(const char* msg, int len)
{
    gAsyncLogging.append(msg, len);
}

int main()
{
    Logging::setOutput(outputFunc);
    gAsyncLogging.start();
    Logging::setLogLevel(Logging::DEBUG);

    LOG_INFO << "logInMainThread";

    ThreadPool pool;
    pool.start(6);

    for (int i = 0; i < 50; ++i)
    {
        pool.run(runInThread);
    }

    sleep(50);
}

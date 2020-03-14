/*************************************************************************
> File Name: AsyncLoggingTest.cpp
> Author: pengjunjie
> Mail: 1002398145@qq.com
> Created Time: 2019年01月01日 星期二 18时22分05秒
************************************************************************/

#include <pallette/AsyncLogging.h>
#include <pallette/Logging.h>
#include <pallette/Timestamp.h>

#include <stdio.h>
#include <sys/resource.h>
#include <unistd.h>

off_t kRollSize = 500 * 1000 * 1000;
pallette::AsyncLogging* g_asyncLog = NULL;

void asyncOutput(const char* msg, int len)
{
    g_asyncLog->append(msg, len);
}

void bench(bool longLog)
{
    pallette::Logger::setOutput(asyncOutput);

    int cnt = 0;
    const int kBatch = 1000;
    std::string empty = " ";
    std::string longStr(3000, 'X');
    longStr += " ";

    for (int t = 0; t < 30; ++t)
    {
        pallette::Timestamp start = pallette::Timestamp::now();
        for (int i = 0; i < kBatch; ++i)
        {
            LOG_INFO << "Hello 0123456789" << " abcdefghijklmnopqrstuvwxyz "
                << (longLog ? longStr : empty) << cnt;
            ++cnt;
        }
        pallette::Timestamp end = pallette::Timestamp::now();
        printf("%02d %fus\n", t + 1, timeDifference(end, start) * 1000000 / kBatch);
        usleep(500000);
    }
}

int main(int argc, char* argv[])
{
    {
        // set max virtual memory to 2GB.
        size_t kOneGB = 1000 * 1024 * 1024;
        rlimit rl = { 2 * kOneGB, 2 * kOneGB };
        setrlimit(RLIMIT_AS, &rl);
    }

    printf("pid = %d\n", getpid());

    char name[256];
    strncpy(name, argv[0], sizeof(name));
    pallette::AsyncLogging log(::basename(name), kRollSize);
    log.start();
    g_asyncLog = &log;

    bool longLog = argc > 1;
    bench(longLog);
}

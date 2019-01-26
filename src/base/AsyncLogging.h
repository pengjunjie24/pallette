/*************************************************************************
  > File Name: AsyncLogging.h
  > Author: pengjunjie
  > Mail: 1002398145@qq.com
  > Created Time: 2018年12月23日 星期日 10时23分17秒
 ************************************************************************/

#ifndef PALLETTE_ASYNCLOGGING_H
#define PALLETTE_ASYNCLOGGING_H

#include "LogStream.h"
#include "CountDownLatch.h"

#include <atomic>
#include <thread>
#include <vector>
#include <condition_variable>
#include <string>

namespace pallette
{
    //异步日志类，多个线程共有一个前端，通过后端写入磁盘文件
    //使用的是多缓冲技术，基本思路是准备多块Buffer，前端负责向Buffer中填数据，后端负责将Buffer中数据取出来写入文件
    class AsyncLogging
    {
    public:
        AsyncLogging(const std::string&, int, int flushInterval = 3);
        ~AsyncLogging();

        void append(const char*, int);
        void start();
        void stop();

    private:
        void threadFunc();

        typedef detail::LogBuffer<detail::kLargeBuffer> Buffer;
        typedef std::vector<std::unique_ptr<Buffer>> BufferVector;
        typedef BufferVector::value_type BufferPtr;

        const int flushInterval_;//刷新日志文件间隔时间(s)
        std::atomic<bool> running_;
        std::string basename_;
        int rollSize_;
        std::thread thread_;
        CountDownLatch latch_;
        std::mutex mutex_;
        std::condition_variable cond_;
        BufferPtr currentBuffer_;
        BufferPtr nextBuffer_;
        BufferVector buffers_;
    };
}

#endif

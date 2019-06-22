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

        void append(const char* buf, int len);//前端线程调用接口
        void start();
        void stop();

    private:
        void threadFunc();//后台线程处理日志接口

        typedef detail::LogBuffer<detail::kLargeBuffer> Buffer;
        typedef std::vector<std::unique_ptr<Buffer>> BufferVector;
        typedef BufferVector::value_type BufferPtr;

        const int flushInterval_;//刷新日志文件间隔时间(s)
        std::atomic<bool> running_;
        std::string basename_;//写入的文件名
        int rollSize_;
        std::thread thread_;//背景线程，处理前端写入日志
        CountDownLatch latch_;//计数器，主线程等待子线程初始化完成
        std::mutex mutex_;
        std::condition_variable cond_;
        BufferPtr currentBuffer_;//当前缓冲区，日志全部写入该缓冲
        BufferPtr nextBuffer_;//预备缓冲区，当前缓冲区写满后写入预备缓冲区
        BufferVector buffers_;//缓冲区队列，存放写满日志的缓冲区
    };
}

#endif

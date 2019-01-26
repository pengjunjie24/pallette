/*************************************************************************
  > File Name: ThreadPool.h
  > Author: pengjunjie
  > Mail: 1002398145@qq.com
  > Created Time: 2018年12月23日 星期日 23时51分23秒
 ************************************************************************/

#ifndef PALLETTE_THREADPOOL_H
#define PALLETTE_THREADPOOL_H

#include "noncopyable.h"

#include <thread>
#include <mutex>
#include <condition_variable>
#include <functional>
#include <vector>
#include <deque>
#include <memory>

namespace pallette
{
    class ThreadPool : public noncopyable
    {
    public:
        typedef std::function<void()> task;
        explicit ThreadPool(const std::string& nameArg = std::string("ThreadPool"));
        ~ThreadPool();

        void setMaxQueueSize(int maxSize) { maxQueueSize_ = maxSize; }
        void setThreadInitCallback(const task& cb) { threadInitCallback_ = cb; }
        const std::string& name() const { return name_; }

        void start(int);//启动固定数量线程的线程池
        void stop();

        size_t queueSize() const;
        void run(task);//将任务添加到线程池中运行

    private:
        bool isFull() const;
        void runInThread();
        task take();//从任务队列中取出任务

        mutable std::mutex mutex_;
        std::condition_variable notEmpty_;//容器非空条件变量
        std::condition_variable notFull_;//容器非满的条件变量
        std::string name_;//线程池名
        task threadInitCallback_;//线程池初始化任务
        std::vector<std::unique_ptr<std::thread>> threads_;//存放线程容器
        std::deque<task> queue_;//任务队列
        size_t maxQueueSize_;//任务队列最大容量
        bool running_;
    };
}

#endif

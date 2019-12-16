
#ifndef PALLETTE_THREAD_H
#define PALLETTE_THREAD_H

#include <pallette/CountDownLatch.h>
#include <pallette/noncopyable.h>

#include <functional>
#include <memory>
#include <atomic>
#include <pthread.h>

namespace pallette
{
    class Thread : noncopyable
    {
    public:
        typedef std::function<void()> ThreadFunc;

        explicit Thread(const ThreadFunc&, const std::string& name = std::string());
        ~Thread();

        void start();
        int join();

        bool started() const { return started_; }
        pid_t tid() const { return tid_; }
        static int numCreated() { return numCreated_.load(); }

    private:
        void setDefaultName();

        bool started_;
        bool joined_;
        pthread_t pthreadId_;
        pid_t tid_;
        ThreadFunc func_;
        std::string name_;
        CountDownLatch latch_;

        static std::atomic<int> numCreated_;
    };
}

#endif
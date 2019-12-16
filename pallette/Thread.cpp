
#include <pallette/Thread.h>
#include <pallette/CurrentThread.h>

#include <assert.h>
#include <stdio.h>
#include <sys/prctl.h>
#include <sys/syscall.h>
#include <unistd.h>

namespace pallette
{
    namespace current_thread
    {
        __thread int tCachedTid = 0;
        __thread const char* tThreadName = "unknown";
    }

    namespace detail
    {
        pid_t gettid()
        {
            return static_cast<pid_t>(::syscall(SYS_gettid));
        }

        struct ThreadData
        {
            typedef pallette::Thread::ThreadFunc ThreadFunc;

            ThreadFunc func_;
            std::string name_;
            pid_t* tid_;
            CountDownLatch* latch_;

            ThreadData(const ThreadFunc& func, const std::string& name,
                pid_t* tid, CountDownLatch* latch)
                : func_(func)
                , name_(name)
                , tid_(tid)
                , latch_(latch)
            {
            }

            void runInThread()
            {
                *tid_ = gettid();
                tid_ = NULL;
                latch_->countDown();
                latch_ = NULL;

                current_thread::tThreadName = name_.empty() ? "palletteThread" : name_.c_str();
                ::prctl(PR_SET_NAME, current_thread::tThreadName);

                try
                {
                    func_();
                    current_thread::tThreadName = "finished";
                }
                catch (...)
                {
                    current_thread::tThreadName = "crashed";
                    fprintf(stderr, "unknown exception caught in Thread %s\n", name_.c_str());
                    throw;
                }
            }
        };

        void* startThread(void* obj)
        {
            ThreadData* data = static_cast<ThreadData*>(obj);
            data->runInThread();
            delete data;
            return NULL;
        }
    }
}

using namespace pallette;

void current_thread::cacheTid()
{
    if (tCachedTid == 0)
    {
        tCachedTid = detail::gettid();
    }
}

bool current_thread::isMainThread()
{
    return tid() == ::getpid();
}

std::atomic<int> Thread::numCreated_;

Thread::Thread(const ThreadFunc& func, const std::string& name)
    : started_(false),
    joined_(false),
    pthreadId_(0),
    tid_(0),
    func_(func),
    name_(name),
    latch_(1)
{
    setDefaultName();
}

Thread::~Thread()
{
    if (started_ && !joined_)
    {
        pthread_detach(pthreadId_);
    }
}

void Thread::start()
{
    assert(!started_);
    started_ = true;

    detail::ThreadData* data = new detail::ThreadData(std::move(func_), name_, &tid_, &latch_);
    if (pthread_create(&pthreadId_, NULL, &detail::startThread, data))
    {
        started_ = false;
        delete data; // or no delete?
        fprintf(stderr, "Failed in pthread_create");
    }
    else
    {
        latch_.wait();
        assert(tid_ > 0);
    }
}

int Thread::join()
{
    assert(started_);
    assert(!joined_);
    joined_ = true;
    return pthread_join(pthreadId_, NULL);
}

void Thread::setDefaultName()
{
    int num = numCreated_++;
    if (name_.empty())
    {
        char buf[32] = { 0 };
        snprintf(buf, sizeof buf, "Thread%d", num);
        name_ = buf;
    }
}

#include <pallette/EventLoopThread.h>
#include <pallette/EventLoop.h>
#include <pallette/CountDownLatch.h>
#include <pallette/CurrentThread.h>

#include <stdio.h>
#include <unistd.h>

using namespace pallette;

void print(EventLoop* p = NULL)
{
    printf("print: pid = %d, tid = %d, loop = %p\n",
        getpid(), current_thread::tid(), p);
}

void quit(EventLoop* p)
{
    print(p);
    p->quit();
}

int main()
{
    print();

    {
        EventLoopThread thr1;  // never start
    }

    {
        // dtor calls quit()
        EventLoopThread thr2;
        EventLoop* loop = thr2.startLoop();
        loop->runInLoop(std::bind(print, loop));
        usleep(500 * 1000);
    }

    {
        // quit() before dtor
        EventLoopThread thr3;
        EventLoop* loop = thr3.startLoop();
        loop->runInLoop(std::bind(quit, loop));
        usleep(500 * 1000);
    }
}

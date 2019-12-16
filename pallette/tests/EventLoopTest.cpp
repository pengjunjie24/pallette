#include <pallette/EventLoop.h>
#include <pallette/CurrentThread.h>
#include <pallette/Thread.h>

#include <functional>

#include <assert.h>
#include <stdio.h>
#include <unistd.h>

using namespace pallette;

EventLoop* g_loop;

void callback()
{
    printf("callback(): pid = %d, tid = %d\n", getpid(), current_thread::tid());
    EventLoop anotherLoop;
}

void threadFunc()
{
    printf("threadFunc(): pid = %d, tid = %d\n", getpid(), current_thread::tid());

    assert(EventLoop::getEventLoopOfCurrentThread() == NULL);
    EventLoop loop;
    assert(EventLoop::getEventLoopOfCurrentThread() == &loop);
    loop.runAfter(1.0, callback);
    loop.loop();
}

int main()
{
    printf("main(): pid = %d, tid = %d\n", getpid(), current_thread::tid());

    assert(EventLoop::getEventLoopOfCurrentThread() == NULL);
    EventLoop loop;
    assert(EventLoop::getEventLoopOfCurrentThread() == &loop);

    pallette::Thread testThread(threadFunc);
    testThread.start();

    loop.loop();
}

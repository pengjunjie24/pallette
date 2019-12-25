/*************************************************************************
> File Name: EchoServerMain.cpp
> Author: pengjunjie
> Mail: 1002398145@qq.com
> Created Time: 2019年02月28日 星期四 14时12分02秒
************************************************************************/

#include <pallette/Logging.h>
#include <pallette/EventLoop.h>
#include <pallette/InetAddress.h>

#include <examples/idleconnection/EchoServer.h>

#include <unistd.h>
#include <assert.h>

using namespace pallette;

void testHash()
{
    std::hash<std::shared_ptr<int> > h;
    std::shared_ptr<int> x1(new int(10));
    std::shared_ptr<int> x2(new int(10));
    h(x1);
    assert(h(x1) != h(x2));
    x1 = x2;
    assert(h(x1) == h(x2));
    x1.reset();
    assert(h(x1) != h(x2));
    x2.reset();
    assert(h(x1) == h(x2));
}

int main(int argc, char* argv[])
{
    testHash();
    EventLoop loop;
    InetAddress listenAddr(2007);
    int idleSeconds = 10;
    if (argc > 1)
    {
        idleSeconds = atoi(argv[1]);
    }
    LOG_INFO << "pid = " << getpid() << ", idle seconds = " << idleSeconds;
    EchoServer server(&loop, listenAddr, idleSeconds);
    server.start();
    loop.loop();
}

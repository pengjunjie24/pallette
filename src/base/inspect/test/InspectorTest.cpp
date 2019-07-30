/*************************************************************************
  > File Name: InspectorTest.cpp
  > Author: pengjunjie
  > Mail: 1002398145@qq.com
  > Created Time: 2019年07月05日 星期五 18时03分38秒
 ************************************************************************/

#include "../Inspector.h"
#include "../../EventLoop.h"
#include "../../EventLoopThread.h"

using namespace pallette;

int main()
{
    EventLoop loop;
    EventLoopThread t;
    Inspector ins(t.startLoop(), InetAddress(12345), "test");
    loop.loop();
}
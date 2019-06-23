/*************************************************************************
  > File Name: main.cpp
  > Author: pengjunjie
  > Mail: 1002398145@qq.com
  > Created Time: 2019年06月09日 星期日 15时06分21秒
 ************************************************************************/

#include "chargen.h"

#include "../../../base/Logging.h"
#include "../../../base/EventLoop.h"
#include "../../../base/ProcessInfo.h"

using namespace pallette;

int main()
{
    LOG_INFO << "pid = " << process_info::pid();
    EventLoop loop;
    InetAddress listenAddr(2019);
    ChargenServer server(&loop, listenAddr, true);
    server.start();

    loop.loop();
}
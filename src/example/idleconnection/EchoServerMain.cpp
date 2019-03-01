/*************************************************************************
  > File Name: EchoServerMain.cpp
  > Author: pengjunjie
  > Mail: 1002398145@qq.com 
  > Created Time: 2019年02月28日 星期四 14时12分02秒
 ************************************************************************/

#include "../../base/Logging.h"
#include "../../base/EventLoop.h"
#include "../../base/InetAddress.h"
#include "../DataConfig.h"
#include "EchoServer.h"

#include <unistd.h>

using namespace pallette;

int main()
{
	LOG_INFO << "pid = " << ::getpid();
	EventLoop loop;
	int idleSeconds = 10;
	DataConfig config("../config.ini");
	InetAddress listenAddr(config.serverIp(), config.serverPort());
	EchoServer server(&loop, listenAddr, idleSeconds);
	server.start();
	loop.loop();
}
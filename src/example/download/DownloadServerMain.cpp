/*************************************************************************
  > File Name: DownloadServerMain.cpp
  > Author: pengjunjie
  > Mail: 1002398145@qq.com 
  > Created Time: 2019年02月25日 星期一 10时54分10秒
 ************************************************************************/

#include "DownloadServer.h"

#include "../DataConfig.h"
#include "../../base/Logging.h"
#include "../../base/InetAddress.h"
#include "../../base/EventLoop.h"

#include <unistd.h>

using namespace pallette;

int main()
{
	LOG_INFO << "pid = " << ::getpid();

	EventLoop loop;
	DataConfig config("../config.ini");
	InetAddress listenAddr(config.serverIp(), config.serverPort());
	std::string downloadFile = config.downloadServerFile();

	DownloadServer server(&loop, listenAddr, downloadFile.c_str());
	server.start();
	loop.loop();
}
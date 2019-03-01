/*************************************************************************
  > File Name: DownloadClientMain.cpp
  > Author: pengjunjie
  > Mail: 1002398145@qq.com 
  > Created Time: 2019年02月26日 星期二 15时20分09秒
 ************************************************************************/

#include "../../base/Logging.h"
#include "../../base/EventLoop.h"
#include "../../base/InetAddress.h"
#include "../DataConfig.h"
#include "DownloadClient.h"

#include <unistd.h>

using namespace pallette;

int main()
{
	LOG_INFO << "pid = " << ::getpid();

	EventLoop loop;
	DataConfig config("../config.ini");
	InetAddress listenAddr(config.serverIp(), config.serverPort());
	std::string downloadFile = config.downloadClientFile();

	DownlaodClient client(&loop, listenAddr, downloadFile);
	client.connect();
	loop.loop();
}
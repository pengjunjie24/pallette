/*************************************************************************
  > File Name: DownloadServer.h
  > Author: pengjunjie
  > Mail: 1002398145@qq.com 
  > Created Time: 2019年02月25日 星期一 10时01分54秒
 ************************************************************************/

#ifndef EXAMPLE_DOWNLOAD_SERVER_H
#define EXAMPLE_DOWNLOAD_SERVER_H

#include "../../base/Callbacks.h"
#include "../../base/TcpServer.h"

#include <stdio.h>
#include <memory>

class DownloadServer
{
public:
	DownloadServer(pallette::EventLoop*, const pallette::InetAddress&, const char*);
	void start();

private:
	typedef std::shared_ptr<FILE> FilePtr;

	void onHighWaterMark(const pallette::TcpConnectionPtr&, size_t len);
	void onConnection(const pallette::TcpConnectionPtr& conn);
	void onWriteComplete(const pallette::TcpConnectionPtr& conn);

	pallette::TcpServer server_;
	std::string filename_;
	uint64_t bytes_;

	static const int kBufSize = 64 * 1024;
};

#endif
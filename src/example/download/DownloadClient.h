/*************************************************************************
  > File Name: DownloadClient.h
  > Author: pengjunjie
  > Mail: 1002398145@qq.com 
  > Created Time: 2019年02月26日 星期二 15时19分45秒
 ************************************************************************/

#ifndef EXAMPLE_DOWNLOAD_CLIENT_H
#define EXAMPLE_DOWNLOAD_CLIENT_H

#include <stdio.h>
#include <memory>

#include "../../base/Callbacks.h"
#include "../../base/TcpClient.h"
#include "../../base/FileUtil.h"

class DownlaodClient
{
public:
	DownlaodClient(pallette::EventLoop*, const pallette::InetAddress&, const std::string&);
	void connect();

private:
	typedef std::shared_ptr<FILE> FilePtr;

	void onConnection(const pallette::TcpConnectionPtr&);
	void onMessage(const pallette::TcpConnectionPtr&, pallette::Buffer*, pallette::Timestamp);

	pallette::TcpClient client_;
	std::string filename_;
	uint64_t bytes_;

	static const int kBufSize = 64 * 1024;
};

#endif
/*************************************************************************
  > File Name: DownloadClient.cpp
  > Author: pengjunjie
  > Mail: 1002398145@qq.com 
  > Created Time: 2019年02月26日 星期二 15时19分54秒
 ************************************************************************/

#include "DownloadClient.h"

#include "../../base/Logging.h"
#include "../../base/EventLoop.h"
#include "../../base/InetAddress.h"
#include "../../base/TcpConnection.h"

#include <string>

using namespace pallette;

DownlaodClient::DownlaodClient(EventLoop* loop,
	const InetAddress& listenAddr, const std::string& filename)
	:client_(loop, listenAddr, "DownloadClient")
	,filename_(filename)
	,bytes_(0)
{
	client_.setConnectionCallback(bind(&DownlaodClient::onConnection, this, _1));
	client_.setMessageCallback(bind(&DownlaodClient::onMessage, this, _1, _2, _3));
}

void DownlaodClient::connect()
{
	client_.connect();
}

void DownlaodClient::onConnection(const TcpConnectionPtr& conn)
{
	LOG_INFO << "DownlaodClient - " << conn->localAddress().toIpPort() << " -> "
		<< conn->peerAddress().toIpPort() << " is "
		<< (conn->connected() ? "UP" : "DOWN");

	if (conn->connected())
	{
		LOG_INFO << "FileServer - recv file: " << filename_ << " from "
			<< conn->peerAddress().toIpPort();

		FILE* fp = ::fopen(filename_.c_str(), "w");
		if (fp)
		{
			FilePtr ctx(fp, ::fclose);
			conn->setContext(ctx);
		}
	}
	else
	{
		LOG_INFO << "download_client quit";
		LOG_INFO << "recv bytes:" << bytes_ << " bytes";
	}
}

void DownlaodClient::onMessage(const TcpConnectionPtr& conn, Buffer* buf, Timestamp)
{
	std::string recvMsg = buf->retrieveAllAsString();
	const FilePtr& fp = any_cast<const FilePtr>(conn->getContext());
	size_t nwrite = 0;
	do {
		int oneWrite = ::fwrite(recvMsg.c_str() + nwrite,
			1, recvMsg.length() - nwrite, fp.get());
		nwrite += oneWrite;
	} while (nwrite != recvMsg.length());

	bytes_ += nwrite;
}
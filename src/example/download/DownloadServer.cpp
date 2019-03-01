/*************************************************************************
  > File Name: DownloadServer.cpp
  > Author: pengjunjie
  > Mail: 1002398145@qq.com 
  > Created Time: 2019年02月25日 星期一 10时02分01秒
 ************************************************************************/

#include "DownloadServer.h"

#include "../../base/Logging.h"
#include "../../base/InetAddress.h"
#include "../../base/TcpConnection.h"

using namespace pallette;

DownloadServer::DownloadServer(pallette::EventLoop* loop,
	const pallette::InetAddress& listenAddr, const char* filename)
	:server_(loop, listenAddr, "DownloadServer")
	,filename_(filename)
	,bytes_(0)
{
	server_.setConnectionCallback(std::bind(&DownloadServer::onConnection, this, _1));
	server_.setWriteCompleteCallback(std::bind(&DownloadServer::onWriteComplete, this, _1));
}
void DownloadServer::start()
{
	server_.start();
}

void DownloadServer::onHighWaterMark(const pallette::TcpConnectionPtr&, size_t len)
{
	LOG_INFO << "HighWaterMark " << len;
}
void DownloadServer::onConnection(const pallette::TcpConnectionPtr& conn)
{
	LOG_INFO << "FileServer - " << conn->peerAddress().toIpPort()
		<< " -> " << conn->localAddress().toIpPort() << " is "
		<< (conn->connected() ? " UP " : " DOWN ");

	if (conn->connected())
	{
		LOG_INFO << "FileServer - Sending file" << filename_ << " to "
			<< conn->peerAddress().toIpPort();
		conn->setHighWaterMarkCallback(std::bind(
			&DownloadServer::onHighWaterMark, this, _1, _2), kBufSize + 1);
		conn->setHighWaterMarkCallback(bind(&DownloadServer::onHighWaterMark, this, _1, _2), kBufSize + 1);

		FILE* fp = ::fopen(filename_.c_str(), "rb");
		if (fp)
		{
			FilePtr ctx(fp, ::fclose);
			conn->setContext(ctx);
			char buf[kBufSize] = { 0 };
			size_t nread = ::fread(buf, 1, sizeof(buf), fp);
			conn->send(buf, static_cast<int>(nread));
			bytes_ += nread;
		}
		else
		{
			conn->shutdown();
			LOG_INFO << "FileServer - no such file: " << filename_;
		}
	}
}
void DownloadServer::onWriteComplete(const pallette::TcpConnectionPtr& conn)
{
	if (conn->getContext().empty())
	{
		LOG_INFO << "any is empty";
		return;
	}
	const FilePtr& fp = any_cast<const FilePtr>(conn->getContext());//XXX:不能用const FilePtr&
	char buf[kBufSize] = { 0 };
	size_t nread = ::fread(buf, 1, sizeof(buf), (FILE*)(fp.get()));
	if (nread > 0)
	{
		conn->send(buf, static_cast<int>(nread));
		bytes_ += nread;
	}
	else
	{
		conn->shutdown();
		LOG_INFO << "send bytes: " << bytes_ << " bytes";
		LOG_INFO << "FileServer - down";
	}
}
/*************************************************************************
  > File Name: EchoServer.cpp
  > Author: pengjunjie
  > Mail: 1002398145@qq.com 
  > Created Time: 2019年02月28日 星期四 10时30分32秒
 ************************************************************************/

#include "../../base/TcpServer.h"
#include "../../base/Logging.h"
#include "../../base/TcpConnection.h"
#include "../../base/InetAddress.h"
#include "../../base/EventLoop.h"
#include "../DataConfig.h"

#include <string>
#include <unistd.h>

using namespace pallette;

class EchoServer
{
public:
	EchoServer(EventLoop* loop,
		const InetAddress& listenAddr)
		:server_(loop, listenAddr, "EchoServer")
	{
		server_.setConnectionCallback(
			std::bind(&EchoServer::onConnection, this, _1));
		server_.setMessageCallback(
			std::bind(&EchoServer::onMessage, this, _1, _2, _3));
	}

	void start() { server_.start(); }

private:
	void onConnection(const TcpConnectionPtr& conn)
	{
		LOG_INFO << "EchoServer - " << conn->peerAddress().toIpPort() << " -> "
			<< conn->localAddress().toIpPort() << " is "
			<< (conn->connected() ? "UP" : "DOWN");
	}

	void onMessage(const TcpConnectionPtr& conn,
		Buffer* buf, Timestamp time)
	{
		std::string msg(buf->retrieveAllAsString());
		LOG_INFO << conn->name() << " echo " << msg.size() << " bytes, "
			<< "data received at " << time.toFormatTedString();
		conn->send(msg);
	}

	TcpServer server_;
};

int main()
{
	LOG_INFO << "pid = " << ::getpid();
	EventLoop loop;
	DataConfig config("../config.ini");
	InetAddress listenAddr(config.serverIp(), config.serverPort());
	EchoServer server(&loop, listenAddr);
	server.start();
	loop.loop();
}
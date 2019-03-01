/*************************************************************************
  > File Name: ChatServer.cpp
  > Author: pengjunjie
  > Mail: 1002398145@qq.com 
  > Created Time: 2019年02月27日 星期三 20时14分59秒
 ************************************************************************/

#include "Codec.h"
#include "../DataConfig.h"
#include "../../base/TcpServer.h"
#include "../../base/EventLoop.h"

#include <set>
#include <unistd.h>

using namespace pallette;

 //将某个客户端发送数据转发给所有客户端
class ChatServer : public noncopyable
{
public:
	ChatServer(EventLoop* loop, InetAddress& listenAddr)
		:server_(loop, listenAddr, "ChatServer")
		, codec_(std::bind(&ChatServer::onStringMessage, this, _1, _2, _3))
	{
		server_.setConnectionCallback(
			std::bind(&ChatServer::onConnection, this, _1));
		server_.setMessageCallback(
			std::bind(&LengthHeaderCodec::onMessage, &codec_, _1, _2, _3));
	}

	void start()
	{
		server_.start();
	}

private:
	void onConnection(const TcpConnectionPtr& conn)
	{
		LOG_INFO << conn->localAddress().toIpPort() << "->"
			<< conn->peerAddress().toIpPort() << " is "
			<< (conn->connected() ? "UP" : "DOWN");

		if (conn->connected())
		{
			connections_.insert(conn);
		}
		else
		{
			connections_.erase(conn);
		}
	}

	void onStringMessage(const TcpConnectionPtr&,
		const std::string& message, Timestamp)
	{
		for (auto conn : connections_)
		{
			codec_.send(conn, message);
		}
	}

	TcpServer server_;
	LengthHeaderCodec codec_;//编解码器
	std::set<TcpConnectionPtr> connections_;//存储链接
};


int main()
{
	LOG_INFO << "pid = " << ::getpid();

	EventLoop loop;
	DataConfig config("../config.ini");
	InetAddress listenAddr(config.serverIp(), config.serverPort());
	ChatServer server(&loop, listenAddr);
	server.start();
	loop.loop();
}
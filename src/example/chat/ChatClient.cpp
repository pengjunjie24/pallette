/*************************************************************************
  > File Name: ChatClient.cpp
  > Author: pengjunjie
  > Mail: 1002398145@qq.com 
  > Created Time: 2019年02月27日 星期三 20时15分07秒
 ************************************************************************/

#include "Codec.h"
#include "../DataConfig.h"
#include "../../base/TcpClient.h"
#include "../../base/TcpConnection.h"
#include "../../base/EventLoopThread.h"
#include "../../base/Logging.h"

#include <iostream>
#include <mutex>
#include <unistd.h>

using namespace pallette;

class ChatClient : public noncopyable
{
public:
	ChatClient(EventLoop* loop, const InetAddress& serverAddr)
		:client_(loop, serverAddr, "ChatClient")
		, codec_(std::bind(&ChatClient::onStringMessage, this, _1, _2, _3))
	{
		client_.setConnectionCallback(
			std::bind(&ChatClient::onConnection, this, _1));
		client_.setMessageCallback(
			std::bind(&LengthHeaderCodec::onMessage, &codec_, _1, _2, _3));
	}

	void connect()
	{
		client_.connect();
	}

	void disconnect()
	{
		client_.disconnect();
	}

	void write(const std::string& message)
	{
		std::unique_lock<std::mutex> lock(mutex_);
		if (connection_)
		{
			codec_.send(connection_, message);
		}
	}
private:
	void onConnection(const TcpConnectionPtr& conn)
	{
		LOG_INFO << conn->localAddress().toIpPort() << "->"
			<< conn->peerAddress().toIpPort() << " is "
			<< (conn->connected() ? "UP" : "DOWN");

		std::unique_lock<std::mutex> lock(mutex_);
		if (conn->connected())
		{
			connection_ = conn;
		}
		else
		{
			connection_.reset();
		}
	}

	void onStringMessage(const TcpConnectionPtr&,
		const std::string& message, Timestamp)
	{
		LOG_INFO << "<<< " << message.c_str();
	}

	TcpClient client_;
	LengthHeaderCodec codec_;//编解码器
	mutable std::mutex mutex_;
	TcpConnectionPtr connection_;
};

int main()
{
	LOG_INFO << "pid = " << getpid();

	//两个线程，loopThread处理网络IO，main线程负责读键盘
	EventLoopThread loopThread;
	DataConfig config("../config.ini");
	InetAddress serverAddr(config.serverIp(), config.serverPort());
	ChatClient client(loopThread.startLoop(), serverAddr);
	client.connect();

	std::string line;
	while (std::getline(std::cin, line))
	{
		client.write(line);
	}
	client.disconnect();
	
	sleep(1);
}
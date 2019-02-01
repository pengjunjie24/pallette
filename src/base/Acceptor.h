/*************************************************************************
  > File Name: Acceptor.h
  > Author: pengjunjie
  > Mail: 1002398145@qq.com 
  > Created Time: 2019年01月29日 星期二 16时58分41秒
 ************************************************************************/

#ifndef PALLETTE_ACCEPTOR_H
#define PALLETTE_ACCEPTOR_H

#include "noncopyable.h"
#include "Channel.h"
#include "Socket.h"

#include <functional>

namespace pallette
{
	class EventLoop;
	class InetAddress;

	class Acceptor : noncopyable
	{
	public:
		typedef std::function<void(int sockfd, const InetAddress&)> NewConnectionCallback;

		Acceptor(EventLoop* loop, const InetAddress& listenAddr, bool reuseport);
		~Acceptor();

		void setNewConnectionCallback(const NewConnectionCallback& cb)
		{
			newConnectionCallback_ = cb;
		}

		bool listenning() const { return listenning_; }
		void listen();

	private:
		void handleRead();

		EventLoop* loop_;
		Socket acceptSocket_;
		Channel acceptChannel_;
		NewConnectionCallback newConnectionCallback_;
		bool listenning_;
		int idleFd_;
	};
}

#endif
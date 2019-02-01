/*************************************************************************
  > File Name: TcpServer.h
  > Author: pengjunjie
  > Mail: 1002398145@qq.com 
  > Created Time: 2019年01月30日 星期三 15时16分04秒
 ************************************************************************/

#ifndef PALLETTE_TCPSERVER_H
#define PALLETTE_TCPSERVER_H

#include "TcpConnection.h"
#include "EventLoopThreadPool.h"

#include <atomic>
#include <map>

namespace pallette
{
	class Acceptor;
	class EventLoop;
	class EventLoopThreadPool;

	class TcpServer : noncopyable
	{
	public:
		typedef std::function<void(EventLoop*)> ThreadInitCallback;
		enum Option
		{
			kNoReusePort,
			kReusePort,
		};

		TcpServer(EventLoop* loop, const InetAddress& listenAddr,
			const std::string& nameArg, Option option = kNoReusePort);
		~TcpServer();

		const std::string& ipPort() const { return ipPort_; }
		const std::string& name() const { return name_; }
		EventLoop* getLoop() const { return loop_; }

		void setThreadNum(int numThreads) { threadPool_->setThreadNum(numThreads); }
		void setThreadInitCallback(const ThreadInitCallback& cb)
		{threadInitCallback_ = cb;}
		std::shared_ptr<EventLoopThreadPool> threadPool()
		{return threadPool_;}

		void start();

		void setConnectionCallback(const ConnectionCallback& cb)
		{connectionCallback_ = cb;}
		void setMessageCallback(const MessageCallback& cb)
		{messageCallback_ = cb;}
		void setWriteCompleteCallback(const WriteCompleteCallback& cb)
		{writeCompleteCallback_ = cb;}

	private:
		void newConnection(int sockfd, const InetAddress& peerAddr);
		void removeConnection(const TcpConnectionPtr& conn);
		void removeConnectionInLoop(const TcpConnectionPtr& conn);

		typedef std::map<std::string, TcpConnectionPtr> ConnectionMap;

		EventLoop* loop_;  // the acceptor loop
		const std::string ipPort_;
		const std::string name_;
		std::unique_ptr<Acceptor> acceptor_; // avoid revealing Acceptor
		std::shared_ptr<EventLoopThreadPool> threadPool_;
		ConnectionCallback connectionCallback_;
		MessageCallback messageCallback_;
		WriteCompleteCallback writeCompleteCallback_;
		ThreadInitCallback threadInitCallback_;
		std::atomic<bool> started_;
		int nextConnId_;
		ConnectionMap connections_;
	};

}

#endif
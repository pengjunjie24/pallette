/*************************************************************************
  > File Name: TcpConnection.h
  > Author: pengjunjie
  > Mail: 1002398145@qq.com 
  > Created Time: 2019年01月30日 星期三 10时25分17秒
 ************************************************************************/

#ifndef PALLETTE_TCPCONNECTION_H
#define PALLETTE_TCPCONNECTION_H

#include "any.hpp"
#include "noncopyable.h"
#include "Callbacks.h"
#include "Buffer.h"
#include "InetAddress.h"

#include <atomic>

struct tcp_info;

namespace pallette
{
	class Channel;
	class EventLoop;
	class Socket;

	class TcpConnection : noncopyable,
		public std::enable_shared_from_this<TcpConnection>
	{
	public:
		TcpConnection(EventLoop* loop, const std::string& name, int sockfd,
			const InetAddress& localAddr, const InetAddress& peerAddr);
		~TcpConnection();

		EventLoop* getLoop() const { return loop_; }
		const std::string& name() const { return name_; }
		const InetAddress& localAddress() const { return localAddr_; }
		const InetAddress& peerAddress() const { return peerAddr_; }
		bool connected() const { return state_ == kConnected; }
		bool disconnected() const { return state_ == kDisconnected; }
		bool getTcpInfo(struct tcp_info*) const;
		std::string getTcpInfoString() const;

		void send(const void* message, int len);
		void send(const std::string& message);
		void send(Buffer* message);
		void shutdown();
		void forceClose();
		void forceCloseWithDelay(double seconds);
		void setTcpNoDelay(bool on);
		void startRead();
		void stopRead();
		bool isReading() const { return reading_; };

		void setContext(const any& context) {context_ = context;}
		const any& getContext() {return context_;}
		any* getMutableContext() {return &context_;}

		void setConnectionCallback(const ConnectionCallback& cb)
		{connectionCallback_ = cb;}
		void setMessageCallback(const MessageCallback& cb)
		{messageCallback_ = cb;}
		void setWriteCompleteCallback(const WriteCompleteCallback& cb)
		{writeCompleteCallback_ = cb;}
		void setHighWaterMarkCallback(const HighWaterMarkCallback& cb, size_t highWaterMark)
		{highWaterMarkCallback_ = cb; highWaterMark_ = highWaterMark;}
		void setCloseCallback(const CloseCallback& cb)
		{closeCallback_ = cb;}

		Buffer* inputBuffer(){return &inputBuffer_;}
		Buffer* outputBuffer(){return &outputBuffer_;}

		void connectEstablished();
		void connectDestroyed();

		std::string creationTime() { return creationTime_.toFormatTedString(); }
		std::string lastReceiveTime() { return lastReceiveTime_.toFormatTedString(); }
		uint64_t bytesReceived() { return bytesReceived_; }
		uint64_t bytesSent() { return bytesSent_; }

	private:
		enum StateE { kDisconnected, kConnecting, kConnected, kDisconnecting };
		void handleRead(Timestamp receiveTime);
		void handleWrite();
		void handleClose();
		void handleError();
		void sendInLoop(const std::string& message);
		void sendInLoop(const void* message, size_t len);
		void shutdownInLoop();
		void forceCloseInLoop();
		void setState(StateE s) { state_ = s; }
		const char* stateToString() const;
		void startReadInLoop();
		void stopReadInLoop();

		EventLoop* loop_;
		const std::string name_;
		std::atomic<StateE> state_;
		bool reading_;
		std::unique_ptr<Socket> socket_;
		std::unique_ptr<Channel> channel_;
		const InetAddress localAddr_;
		const InetAddress peerAddr_;

		ConnectionCallback connectionCallback_;
		MessageCallback messageCallback_;
		WriteCompleteCallback writeCompleteCallback_;
		HighWaterMarkCallback highWaterMarkCallback_;
		CloseCallback closeCallback_;
		size_t highWaterMark_;

		Buffer inputBuffer_;
		Buffer outputBuffer_; // FIXME: use list<Buffer> as output buffer.
		Timestamp creationTime_;
		Timestamp lastReceiveTime_;
		uint64_t bytesReceived_;
		uint64_t bytesSent_;

		any context_;
	};

	typedef std::shared_ptr<TcpConnection> TcpConnectionPtr;
}

#endif
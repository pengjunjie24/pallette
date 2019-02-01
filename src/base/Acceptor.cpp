/*************************************************************************
  > File Name: Acceptor.cpp
  > Author: pengjunjie
  > Mail: 1002398145@qq.com 
  > Created Time: 2019年01月29日 星期二 16时58分49秒
 ************************************************************************/

#include "Acceptor.h"

#include "Logging.h"
#include "EventLoop.h"
#include "InetAddress.h"
#include "SocketsOperations.h"

#include <errno.h>
#include <fcntl.h>
#include <unistd.h>
#include <assert.h>

using namespace pallette;

Acceptor::Acceptor(EventLoop* loop, const InetAddress& listenAddr, bool reuseport)
	: loop_(loop),
	acceptSocket_(sockets::createNonblockingOrDie(listenAddr.family())),
	acceptChannel_(loop, acceptSocket_.fd()),
	listenning_(false),
	idleFd_(::open("/dev/null", O_RDONLY | O_CLOEXEC))
{
	assert(idleFd_ >= 0);
	acceptSocket_.setReuseAddr(true);
	acceptSocket_.setReusePort(reuseport);
	acceptSocket_.bindAddress(listenAddr);
	acceptChannel_.setReadCallback(
		std::bind(&Acceptor::handleRead, this));
}

Acceptor::~Acceptor()
{
	acceptChannel_.disableAll();
	acceptChannel_.remove();
	::close(idleFd_);
}

void Acceptor::listen()
{
	loop_->assertInLoopThread();
	listenning_ = true;
	acceptSocket_.listen();
	acceptChannel_.enableReading();
}

void Acceptor::handleRead()
{
	assert(loop_);
	loop_->assertInLoopThread();
	InetAddress peerAddr;
	int connfd = acceptSocket_.accept(&peerAddr);
	if (connfd >= 0)
	{
		if (newConnectionCallback_)
		{
			newConnectionCallback_(connfd, peerAddr);
		}
		else
		{
			sockets::close(connfd);
		}
	}
	else
	{
		LOG_SYSERR << "in Acceptor::handleRead";

		//fd耗尽情况下，调用预留的空闲文件描述符idle_fd_，接收新的连接
		//然后将新的连接关闭，避免监听套接字一直处于可读状态
		if (errno == EMFILE)
		{
			::close(idleFd_);
			idleFd_ = ::accept(acceptSocket_.fd(), NULL, NULL);
			::close(idleFd_);
			idleFd_ = ::open("/dev/null", O_RDONLY | O_CLOEXEC);
		}
	}
}
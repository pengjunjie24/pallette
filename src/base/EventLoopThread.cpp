/*************************************************************************
  > File Name: EventLoopThread.cpp
  > Author: pengjunjie
  > Mail: 1002398145@qq.com 
  > Created Time: 2019年01月15日 星期二 20时43分12秒
 ************************************************************************/

#include "EventLoopThread.h"
#include "EventLoop.h"
#include <assert.h>

using namespace pallette;

EventLoopThread::EventLoopThread(const ThreadInitCallback& cb)
	: loop_(NULL)
	, exiting_(false)
	, thread_(std::bind(&EventLoopThread::threadFunc, this))
	, callback_(cb)
{
}

EventLoopThread::~EventLoopThread()
{
	exiting_ = true;
	if (loop_ != NULL)
	{
		loop_->quit();
	}
}

EventLoop* EventLoopThread::startLoop()
{
	thread_.detach();
	{
		std::unique_lock<std::mutex> lock(mutex_);
		while (loop_ == NULL)
		{
			cond_.wait(lock);
		}
	}

	return loop_;
}

void EventLoopThread::threadFunc()
{
	EventLoop loop;

	if (callback_)
	{
		callback_(&loop);
	}

	{
		std::unique_lock<std::mutex> lock(mutex_);
		loop_ = &loop;
		cond_.notify_one();
	}

	loop.loop();
	loop_ = NULL;
}

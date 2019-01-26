/*************************************************************************
  > File Name: EventLoopThreadPool.h
  > Author: pengjunjie
  > Mail: 1002398145@qq.com 
  > Created Time: 2019年01月15日 星期二 21时12分50秒
 ************************************************************************/

#ifndef PALLETTE_EVENTLOOP_THREADPOOL_H
#define PALLETTE_EVENTLOOP_THREADPOOL_H

#include "noncopyable.h"

#include <functional>
#include <memory>
#include <vector>

namespace pallette
{
	class EventLoop;
	class EventLoopThread;

	class EventLoopThreadPool : noncopyable
	{
	public:
		typedef std::function<void(EventLoop*)> ThreadInitCallback;

		EventLoopThreadPool(EventLoop* baseLoop);
		~EventLoopThreadPool();
		void setThreadNum(int numThreads) { numThreads_ = numThreads; }
		void start(const ThreadInitCallback& cb = ThreadInitCallback());

		EventLoop* getNextLoop();
		std::vector<EventLoop*> getAllLoops();

		bool started() const { return started_; }

	private:
		EventLoop* baseLoop_;
		bool started_;
		int numThreads_;
		int next_;
		std::vector<std::unique_ptr<EventLoopThread>> threads_;
		std::vector<EventLoop*> loops_;
	};
}

#endif


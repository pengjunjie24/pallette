/*************************************************************************
  > File Name: EventLoopTest.cpp
  > Author: pengjunjie
  > Mail: 1002398145@qq.com 
  > Created Time: 2019年01月14日 星期一 20时32分25秒
 ************************************************************************/

#include "../EventLoop.h"
#include "../Logging.h"
#include "../ProcessInfo.h"
#include "../AsyncLogging.h"

#include <thread>
#include <assert.h>

using namespace pallette;

const long kRollSize = 512 * 1024 * 1024;//滚动长度设为512M
const char* kFilename = ".";//日志路径
pallette::AsyncLogging gAsyncLogging(kFilename, kRollSize);

void callback()
{
	LOG_INFO << "callback: pid = " << process_info::pid() << ", tid = " << process_info::tid();
}

void threadFunc()
{
	LOG_INFO << "threadFunc: pid = " << process_info::pid() << ", tid = " << process_info::tid();

	assert(EventLoop::getEventLoopOfCurrentThread() == NULL);
	EventLoop loop;
	assert(EventLoop::getEventLoopOfCurrentThread() == &loop);

	loop.runInLoop(callback);
	loop.loop();
}

void outputFunc(const char* msg, int len)
{
	gAsyncLogging.append(msg, len);
}


int main()
{
	Logging::setOutput(outputFunc);
	gAsyncLogging.start();
	Logging::setLogLevel(Logging::TRACE);

	LOG_INFO << "main: pid = " << process_info::pid() << ", tid = " << process_info::tid();
	assert(EventLoop::getEventLoopOfCurrentThread() == NULL);
	EventLoop loop;
	assert(EventLoop::getEventLoopOfCurrentThread() == &loop);
	std::thread backThread(threadFunc);
	backThread.detach();

	loop.loop();
}
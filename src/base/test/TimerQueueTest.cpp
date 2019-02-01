/*************************************************************************
  > File Name: TimerQueueTest.cpp
  > Author: pengjunjie
  > Mail: 1002398145@qq.com 
  > Created Time: 2019年01月31日 星期四 15时45分10秒
 ************************************************************************/

#include "../Logging.h"
#include "../EventLoop.h"
#include "../TimerQueue.h"
#include "../TimerId.h"
#include "../ProcessInfo.h"

#include <unistd.h>
using namespace pallette;

int cnt = 0;
EventLoop* g_loop;

void printTid()
{
	LOG_INFO << "pid = " << getpid() << ", tid = " << process_info::tid();
}

void print(const char* msg)
{
	LOG_INFO << "msg " << Timestamp::now().toString().c_str() << " " << msg;
}

void cancle(TimerId timer)
{
	g_loop->cancel(timer);
	LOG_INFO << "cancelled at " << Timestamp::now().toString().c_str();
}

int main()
{
	printTid();

	Logging::setLogLevel(Logging::ENUM_LOGLEVEL::INFO);
	sleep(1);
	{
		EventLoop loop;
		g_loop = &loop;

		LOG_INFO << "main";
		loop.runAfter(0.01, std::bind(print, "once1"));
		loop.runAfter(1.5, std::bind(print, "once1.5"));
		loop.runAfter(2.5, std::bind(print, "once2.5"));
		loop.runAfter(3.5, std::bind(print, "once3.5"));

		TimerId t45 = loop.runAfter(4.5, std::bind(print, "once4.5"));
		loop.runAfter(4.2, std::bind(cancle, t45));
		loop.runAfter(4.8, std::bind(cancle, t45));

		TimerId t3 = loop.runEvery(2, std::bind(print, "every3"));
		loop.runAfter(9, std::bind(cancle, t3));
		LOG_INFO << "RUN END";

		loop.loop();
		LOG_INFO << "main loop exits";
	}
}
/*************************************************************************
  > File Name: TimerQueue.cpp
  > Author: pengjunjie
  > Mail: 1002398145@qq.com 
  > Created Time: 2019年01月30日 星期三 16时57分11秒
 ************************************************************************/

#include "TimerQueue.h"

#include "EventLoop.h"
#include "Logging.h"
#include "Timer.h"
#include "TimerId.h"

#include <sys/timerfd.h>
#include <unistd.h>
#include <string.h>
#include <assert.h>

namespace
{
	int createTimerfd()
	{
		int timerfd = ::timerfd_create(CLOCK_MONOTONIC, TFD_NONBLOCK | TFD_CLOEXEC);
		if (timerfd < 0)
		{
			LOG_SYSFATAL << "Failed in timerfd_create";
		}
		return timerfd;
	}

	struct timespec howMuchTimeFromNow(pallette::Timestamp when)
	{
		int64_t microseconds = when.getMicroSeconds()
			- pallette::Timestamp::now().getMicroSeconds();
		if (microseconds < 100)
		{
			microseconds = 100;
		}
		struct timespec ts;
		ts.tv_sec = static_cast<time_t>(
			microseconds / pallette::Timestamp::kMicroSecondsPerSecond);
		ts.tv_nsec = static_cast<long>(
			(microseconds % pallette::Timestamp::kMicroSecondsPerSecond) * 1000);
		return ts;
	}

	void readTimerfd(int timerfd, pallette::Timestamp now)
	{
		uint64_t howmany;
		ssize_t n = ::read(timerfd, &howmany, sizeof howmany);
		LOG_TRACE << "TimerQueue::handleRead() " << howmany << " at " << now.toString();
		if (n != sizeof howmany)
		{
			LOG_ERROR << "TimerQueue::handleRead() reads " << n << " bytes instead of 8";
		}
	}

	void resetTimerfd(int timerfd, pallette::Timestamp expiration)
	{
		struct itimerspec newValue;
		struct itimerspec oldValue;
		bzero(&newValue, sizeof newValue);
		bzero(&oldValue, sizeof oldValue);
		newValue.it_value = howMuchTimeFromNow(expiration);
		int ret = ::timerfd_settime(timerfd, 0, &newValue, &oldValue);
		if (ret)
		{
			LOG_SYSERR << "timerfd_settime()";
		}
	}
}

using namespace pallette;

TimerQueue::TimerQueue(EventLoop* loop)
	: loop_(loop)
	,timerfd_(createTimerfd())
	,timerfdChannel_(loop, timerfd_)
	,timers_()
	,activeTimers_()
	,callingExpiredTimers_(false)
{
	timerfdChannel_.setReadCallback(
		std::bind(&TimerQueue::handleRead, this));
	timerfdChannel_.enableReading();
}

TimerQueue::~TimerQueue()
{
	timerfdChannel_.disableAll();
	timerfdChannel_.remove();
	::close(timerfd_);
	for (TimerSet::iterator it = timers_.begin();
		it != timers_.end(); ++it)
	{
		delete it->second;
	}

	for (auto cancleTimer : cancelingTimers_)
	{
		delete cancleTimer;
	}
}

TimerId TimerQueue::addTimer(TimerCallback cb, Timestamp when, double interval)
{
	Timer* timer = new Timer(std::move(cb), when, interval);
	loop_->runInLoop(
		std::bind(&TimerQueue::addTimerInLoop, this, timer));
	return TimerId(timer, timer->sequence());
}

void TimerQueue::cancel(TimerId timerId)
{
	loop_->runInLoop(
		std::bind(&TimerQueue::cancelInLoop, this, timerId));
}

void TimerQueue::addTimerInLoop(Timer* timer)
{
	loop_->assertInLoopThread();
	bool earliestChanged = insert(timer);

	if (earliestChanged)
	{
		resetTimerfd(timerfd_, timer->expiration());
	}
}

void TimerQueue::cancelInLoop(TimerId timerId)
{
	loop_->assertInLoopThread();

	TimerList::iterator it =
		std::find(waitTimers_.begin(), waitTimers_.end(), timerId.timer_);
	if (it != waitTimers_.end())//在等待的定时器中
	{
		cancelingTimers_.push_back(*it);

		size_t n __attribute__((unused))
			= timers_.erase(Entry((*it)->expiration(), (*it)));
		assert(n == 1);

		waitTimers_.erase(it);
	}
	else if(callingExpiredTimers_)//在激活的定时器里
	{
		TimerList::iterator activeIter =
			std::find(activeTimers_.begin(), activeTimers_.end(), timerId.timer_);
		if (activeIter != activeTimers_.end())
		{
			cancelingTimers_.push_back(timerId.timer_);
		}
	}
}

void TimerQueue::handleRead()
{
	loop_->assertInLoopThread();
	Timestamp now(Timestamp::now());
	readTimerfd(timerfd_, now);//读取timerfd，避免重复唤醒

	getExpired(now);//得到now时间响应的定时器

	callingExpiredTimers_ = true;
	for (auto expiredTimer : activeTimers_)
	{
		currentActiveTimer_ = expiredTimer;
		currentActiveTimer_->run();
	}
	callingExpiredTimers_ = false;

	reset(now);
}

bool TimerQueue::insert(Timer* timer)
{
	loop_->assertInLoopThread();
	bool earliestChanged = false;
	Timestamp when = timer->expiration();
	TimerSet::iterator it = timers_.begin();
	if (it == timers_.end() || when < it->first)//容器里没有定时器或者将要加入的定时器触发时间是最早的
	{
		earliestChanged = true;
	}


	std::pair<TimerSet::iterator, bool> result __attribute__((unused))
		= timers_.insert(Entry(when, timer));
	assert(result.second);

	waitTimers_.push_back(timer);
	return earliestChanged;
}

void TimerQueue::getExpired(Timestamp now)
{
	activeTimers_.clear();

	Entry sentry(now, reinterpret_cast<Timer*>(UINTPTR_MAX));
	TimerSet::iterator end = timers_.lower_bound(sentry);
	assert(end == timers_.end() || now < end->first);

	for (TimerSet::iterator it = timers_.begin(); it != end; ++it)
	{
		activeTimers_.push_back(it->second);

		TimerList::iterator waitIter =
			std::find(waitTimers_.begin(), waitTimers_.end(), it->second);
		assert(waitIter != waitTimers_.end());
		waitTimers_.erase(waitIter);
	}
	timers_.erase(timers_.begin(), end);

	assert(timers_.size() == waitTimers_.size());
}

void TimerQueue::reset(Timestamp now)
{
	Timestamp nextExpire;

	for (auto expireTimer : activeTimers_)
	{
		if (expireTimer->repeat() &&
			std::find(cancelingTimers_.begin(), cancelingTimers_.end(), expireTimer) == cancelingTimers_.end())
		{
			expireTimer->restart(now);
			insert(expireTimer);
		}
		else if(!expireTimer->repeat())
		{
			cancelingTimers_.push_back(expireTimer);
		}
	}

	if (!timers_.empty())
	{
		nextExpire = timers_.begin()->first;
	}
	if (nextExpire.valid())
	{
		resetTimerfd(timerfd_, nextExpire);
	}
}
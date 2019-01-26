/*************************************************************************
  > File Name: EpollPoller.cpp
  > Author: pengjunjie
  > Mail: 1002398145@qq.com
  > Created Time: 2019年01月06日 星期日 21时25分23秒
 ************************************************************************/

#include "EpollPoller.h"

#include "EventLoop.h"
#include "Channel.h"
#include "Logging.h"

#include <assert.h>
#include <string.h>
#include <errno.h>
#include <unistd.h>
#include <sys/epoll.h>

using namespace pallette;

namespace
{
    const int kNew = -1;//新的文件描述符，未在epoll中也不在channels_中
    const int kAdded = 1;//已经添加的文件描述符，在epoll中
    const int kDeleted = 2;//已经删除的文件描述符，未在epoll中但在channels_中
}

EpollPoller::EpollPoller(EventLoop* loop)
	:ownerLoop_(loop)
	, epollfd_(::epoll_create1(EPOLL_CLOEXEC))
	,events_(kInitEventListSize)
{
    if (epollfd_ < 0)
    {
        LOG_SYSFATAL << "EpollPoller::EpollPoller";
    }
}
EpollPoller::~EpollPoller()
{
	::close(epollfd_);
}

Timestamp EpollPoller::poll(int timeoutMs, ChannelList* activeChannels)
{
	LOG_TRACE << "fd total count " << channels_.size();

	int numEvents = ::epoll_wait(epollfd_, &*events_.begin(),
		static_cast<int>(events_.size()), timeoutMs);
	int savedErrno = errno;
	Timestamp now(Timestamp::now());
	if (numEvents > 0)
	{
		LOG_TRACE << numEvents << " events happened";
		fillActiveChannels(numEvents, activeChannels);
		if (static_cast<size_t>(numEvents) == events_.size())
		{
			events_.resize(events_.size() * 2);
		}
	}
	else if (numEvents == 0)
	{
		LOG_TRACE << "nothing happened";
	}
	else
	{
		if (savedErrno != EINTR)
		{
			errno = savedErrno;
			LOG_SYSERR << "EPollPoller::poll()";
		}
	}
	return now;
}

void EpollPoller::updateChannel(Channel* channel)
{
	EpollPoller::assertInLoopThread();
	const int index = channel->index();
	LOG_TRACE << "fd = " << channel->fd()
		<< " events = " << channel->events() << " index = " << index;
	if (index == kNew || index == kDeleted)
	{
		int fd = channel->fd();
		if (kNew == index)//不在map中，也不在epoll中
		{
			assert(channels_.find(fd) == channels_.end());
			channels_[fd] = channel;
		}
		else//不在epoll中，在map中
		{
			assert(channels_.find(fd) != channels_.end());
			assert(channels_[fd] == channel);
		}

		channel->setIndex(kAdded);
		update(EPOLL_CTL_ADD, channel);
	}
	else//在map和epoll中，进行的操作只有可能是从epoll中删除或者修改监听事件
	{
		int fd __attribute__((unused)) = channel->fd();
		assert(channels_.find(fd) != channels_.end());
		assert(channels_[fd] == channel);
		assert(index == kAdded);

		if(channel->isNoneEvent())
		{
			update(EPOLL_CTL_DEL, channel);
			channel->setIndex(kDeleted);
		}
		else
		{
			update(EPOLL_CTL_MOD, channel);
		}
	}
}

void EpollPoller::removeChannel(Channel* channel)
{
	EpollPoller::assertInLoopThread();
	int fd = channel->fd();
	LOG_TRACE << "fd = " << fd;
	assert(channels_.find(fd) != channels_.end());
	assert(channels_[fd] == channel);
	assert(channel->isNoneEvent());
	int index __attribute__((unused)) = channel->index();
	assert(index == kAdded || index == kDeleted);
	size_t n __attribute__((unused)) = channels_.erase(fd);
	assert(n == 1);

}

bool EpollPoller::hasChannel(Channel* channel) const
{
	std::map<int, Channel*>::const_iterator it = channels_.find(channel->fd());
	return it != channels_.end() && it->second == channel;
}

void EpollPoller::assertInLoopThread() const
{
	ownerLoop_->assertInLoopThread();
}

const char* EpollPoller::operationToString(int op)
{
	switch (op)
	{
	case EPOLL_CTL_ADD:
		return "ADD";
	case EPOLL_CTL_MOD:
		return "MOD";
	case EPOLL_CTL_DEL:
		return "DEL";
	default:
		return "unknown operation";
	}
}

void EpollPoller::fillActiveChannels(int numEvents, ChannelList* activeChannels) const
{
	assert(static_cast<size_t>(numEvents) <= events_.size());
	for (int i = 0; i < numEvents; ++i)
	{
		Channel* channel = static_cast<Channel*>(events_[i].data.ptr);
		channel->setRevents(events_[i].events);
		activeChannels->push_back(channel);
	}
}

void EpollPoller::update(int operation, Channel* channel)
{
	struct epoll_event opEvent;
	bzero(&opEvent, sizeof(opEvent));
	opEvent.events = channel->events();
	opEvent.data.ptr = channel;
	int fd = channel->fd();

	LOG_TRACE << "epoll_ctl op = " << operationToString(operation)
		<< " fd = " << fd << " event = " << channel->eventsToString();
	if (::epoll_ctl(epollfd_, operation, fd, &opEvent) < 0)
	{
		if (operation == EPOLL_CTL_DEL)
		{
			LOG_SYSERR << "epoll_ctl op = " << operationToString(operation) << " fd =" << fd;
		}
		else
		{
			LOG_SYSFATAL << "epoll_ctl op = " << operationToString(operation) << " fd =" << fd;
		}
	}
}
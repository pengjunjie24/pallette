/*************************************************************************
> File Name: Channel.cpp
> Author: pengjunjie
> Mail: 1002398145@qq.com
> Created Time: 2019年01月06日 星期日 21时25分57秒
************************************************************************/

#include "Channel.h"

#include "EventLoop.h"
#include "Logging.h"

#include <sstream>
#include <assert.h>

using namespace pallette;

Channel::Channel(EventLoop* loop, int cfd)
: loop_(loop),
fd_(cfd),
events_(0),
revents_(0),
index_(-1),
tied_(false),
eventHandling_(false),
addedToLoop_(false)
{
}

Channel::~Channel()
{
    assert(!eventHandling_);
    assert(!addedToLoop_);
    if (loop_->isInLoopThread())
    {
        assert(!loop_->hasChannel(this));
    }
}

void Channel::handleEvent(Timestamp receiveTime)
{
    std::shared_ptr<void> guard;
    if (tied_)
    {
        guard = tie_.lock();
        if (guard)
        {
            handleEventWithGuard(receiveTime);
        }
    }
    else
    {
        handleEventWithGuard(receiveTime);
    }
}


void Channel::tie(const std::shared_ptr<void>& obj)
{
    tie_ = obj;
    tied_ = true;
}

std::string Channel::reventsToString() const
{
    return eventsToString(fd_, revents_);
}

std::string Channel::eventsToString() const
{
    return eventsToString(fd_, events_);
}

std::string Channel::eventsToString(int fd, int ev)
{
    std::ostringstream oss;
    oss << fd << ": ";

    if (ev & EPOLLIN)
    {
        oss << "EPOLLIN ";
    }
    if (ev & EPOLLOUT)
    {
        oss << "EPOLLOUT ";
    }
    if (ev & EPOLLPRI)
    {
        oss << "EPOLLPRI ";
    }
    if (ev & EPOLLERR)
    {
        oss << "EPOLLERR ";
    }
    if (ev & EPOLLHUP)
    {
        oss << "EPOLLHUP ";
    }

    return oss.str().c_str();
}

void Channel::remove()
{
    assert(isNoneEvent());
    addedToLoop_ = false;
    loop_->removeChannel(this);
}

void Channel::update()
{
    addedToLoop_ = true;
    loop_->updateChannel(this);
}

void Channel::handleEventWithGuard(Timestamp receiveTime)
{
    eventHandling_ = true;

    if ((revents_ & EPOLLHUP) && !(revents_ & EPOLLIN))
    {
        LOG_WARN << "fd = " << fd_ << " handleEvent EPOLLHUP";
        if (closeCallback_)
        {
            closeCallback_();
        }
    }

    if (revents_ & EPOLLERR)
    {
        if (errorCallback_)
        {
            errorCallback_();
        }
    }

    if (revents_ & (EPOLLIN | EPOLLPRI | EPOLLHUP))
    {
        if (readCallback_)
        {
            readCallback_(receiveTime);
        }
    }

    if (revents_ & EPOLLOUT)
    {
        if (writeCallback_)
        {
            writeCallback_();
        }
    }

    eventHandling_ = false;
}

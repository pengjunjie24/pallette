/*************************************************************************
  > File Name: Channel.h
  > Author: pengjunjie
  > Mail: 1002398145@qq.com
  > Created Time: 2019年01月06日 星期日 21时25分50秒
 ************************************************************************/

#ifndef PALLETTE_CHANNEL_H
#define PALLETTE_CHANNEL_H

#include "Timestamp.h"
#include "noncopyable.h"

#include <functional>
#include <memory>
#include <string>
#include <sys/epoll.h>

namespace pallette
{
	class EventLoop;

	class Channel : noncopyable
	{
	public:
		typedef std::function<void()> EventCallback;
		typedef std::function<void(Timestamp)> ReadEventCallback;

		Channel(EventLoop* loop, int cfd);
		~Channel();

		void handleEvent(Timestamp receiveTime);
		void setReadCallback(ReadEventCallback cb) { readCallback_ = std::move(cb); }
		void setWriteCallback(EventCallback cb) { writeCallback_ = std::move(cb); }
		void setCloseCallback(EventCallback cb) { closeCallback_ = std::move(cb); }
		void setErrorCallback(EventCallback cb) { errorCallback_ = std::move(cb); }

		void tie(const std::shared_ptr<void>&);

		int fd() const { return fd_; }
		int events() const { return events_; }
		void setRevents(int revt) { revents_ = revt; }
		bool isNoneEvent() const { return events_ == kNoneEvent; }

		void enableReading() { events_ |= kReadEvent; update(); }
		void disableReading() { events_ &= ~kReadEvent; update(); }
		void enableWriting() { events_ |= kWriteEvent; update(); }
		void disableWriting() { events_ &= ~kWriteEvent; update(); }
		void disableAll() { events_ = kNoneEvent; update(); }
		bool isWriting() const { return events_ & kWriteEvent; }
		bool isReading() const { return events_ & kReadEvent; }

		int index() { return index_; }
		void setIndex(int idx) { index_ = idx; }

		std::string reventsToString() const;
		std::string eventsToString() const;

		EventLoop* ownerLoop() { return loop_; }
		void remove();

	private:
		static std::string eventsToString(int fd, int ev);
		void update();
		void handleEventWithGuard(Timestamp receiveTime);

		EventLoop* loop_;
		const int fd_;
		int events_;//channel监听的事件
		int revents_;//epoll返回时被响应的事件
		int index_;//在EpollPoller中的状态,在EpollPoller中使用

		std::weak_ptr<void> tie_;
		bool tied_;
		bool eventHandling_;
		bool addedToLoop_;
		ReadEventCallback readCallback_;
		EventCallback writeCallback_;
		EventCallback closeCallback_;
		EventCallback errorCallback_;

		static const int kNoneEvent = 0;
		static const int kReadEvent = EPOLLIN | EPOLLPRI;
		static const int kWriteEvent = EPOLLOUT;
	};
}

#endif
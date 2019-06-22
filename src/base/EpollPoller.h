/*************************************************************************
  > File Name: EpollPoller.h
  > Author: pengjunjie
  > Mail: 1002398145@qq.com
  > Created Time: 2019年01月06日 星期日 21时25分01秒
 ************************************************************************/

#ifndef PALLETTE_EPOLLPOLLER_H
#define PALLETTE_EPOLLPOLLER_H

#include <map>
#include <vector>

#include "Timestamp.h"
#include "noncopyable.h"

struct epoll_event;

namespace pallette
{
    class EventLoop;
    class Channel;

    //EpollPoller是对IO多路复用的封装，它属于某个EventLoop,生命周期与EventLoop相同，所以使用时无需加锁
    //EpollPoller并不拥有Channel，只是持有Channel的裸指针，因此Channel在析构前要将自己从Poller中删除
    class EpollPoller: noncopyable
    {
    public:
        typedef std::vector<Channel*> ChannelList;

        EpollPoller(EventLoop* loop);
        ~EpollPoller();

        Timestamp poll(int timeoutMs, ChannelList* activeChannels);//封装了epoll_wait函数
        void updateChannel(Channel* channel);//更新fd在epoll中的状态
        void removeChannel(Channel* channel);
        bool hasChannel(Channel* channel) const;
        void assertInLoopThread() const;

    private:
        typedef std::vector<struct epoll_event> EventList;
        typedef std::map<int, Channel*> ChannelMap;

        static const char* operationToString(int op);
        void fillActiveChannels(int numEvents, ChannelList* activeChannels) const;
        void update(int operation, Channel* channel);//封装epoll_ctl函数

        EventLoop* ownerLoop_;
        int epollfd_;//epoll的句柄
        EventList events_;//存放激活事件
        ChannelMap channels_;//监听的channel

        static const int kInitEventListSize = 16;
    };
}

#endif

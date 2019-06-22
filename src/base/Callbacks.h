/*************************************************************************
> File Name: Callbacks.h
> Author: pengjunjie
> Mail: 1002398145@qq.com
> Created Time: 2019年01月30日 星期三 10时29分09秒
************************************************************************/

#ifndef PALLETTE_CALLBACKS_H
#define PALLETTE_CALLBACKS_H

#include "Timestamp.h"

#include <functional>
#include <memory>

namespace pallette
{
    using std::placeholders::_1;
    using std::placeholders::_2;
    using std::placeholders::_3;

    class Buffer;
    class TcpConnection;

    typedef std::shared_ptr<TcpConnection> TcpConnectionPtr;
    typedef std::function<void()> TimerCallback;
    typedef std::function<void(const TcpConnectionPtr&)> ConnectionCallback;
    typedef std::function<void(const TcpConnectionPtr&)> CloseCallback;
    typedef std::function<void(const TcpConnectionPtr&)> WriteCompleteCallback;
    typedef std::function<void(const TcpConnectionPtr&, size_t)> HighWaterMarkCallback;
    typedef std::function<void(const TcpConnectionPtr&, Buffer*, Timestamp)> MessageCallback;

    void defaultConnectionCallback(const TcpConnectionPtr& conn);
    void defaultMessageCallback(const TcpConnectionPtr& conn, Buffer* buffer, Timestamp receiveTime);
}

#endif

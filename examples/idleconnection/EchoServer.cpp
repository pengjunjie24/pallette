/*************************************************************************
> File Name: EchoServer.cpp
> Author: pengjunjie
> Mail: 1002398145@qq.com
> Created Time: 2019年02月27日 星期三 21时22分33秒
************************************************************************/

#include <examples/idleconnection/EchoServer.h>

#include <pallette/Logging.h>
#include <pallette/Callbacks.h>
#include <pallette/EventLoop.h>
#include <pallette/TimerId.h>

#include <functional>
#include <memory>

using namespace pallette;

EchoServer::EchoServer(EventLoop* loop,
    const InetAddress& listenAddr, int idleSeconds)
    :server_(loop, listenAddr, "EchoServer")
    , connectionBuckets_(idleSeconds)//设置timing_wheel大小，也是设置超时时间
{
    server_.setConnectionCallback(
        std::bind(&EchoServer::onConnection, this, _1));
    server_.setMessageCallback(std::bind(
        &EchoServer::onMessage, this, _1, _2, _3));
    loop->runEvery(1.0, std::bind(&EchoServer::onTime, this));
    dumpConnectionBuckets();
}

void EchoServer::start()
{
    server_.start();
}


void EchoServer::onConnection(const TcpConnectionPtr& conn)
{
    LOG_INFO << "EchoServer - " << conn->peerAddress().toIpPort() << " -> "
        << conn->localAddress().toIpPort() << " is "
        << (conn->connected() ? "UP" : "DOWN");

    if (conn->connected())
    {
        EntryPtr entry = std::make_shared<Entry>(conn);

        connectionBuckets_.back().insert(entry);//当连接上来后放入循环队列指向的当期容器
        dumpConnectionBuckets();
        WeakEntryPtr weakEntry(entry);
        conn->setContext(weakEntry);
    }
    else
    {
        assert(!conn->getContext().empty());
        WeakEntryPtr weakEntry(any_cast<WeakEntryPtr>(conn->getContext()));
        LOG_DEBUG << "Entry use_count = " << weakEntry.use_count();
    }
}

void EchoServer::onMessage(const TcpConnectionPtr& conn,
    Buffer* buf, Timestamp time)
{
    std::string msg(buf->retrieveAllAsString());
    LOG_INFO << conn->name() << " echo " << msg.size()
        << " bytes at " << time.toFormatTedString();
    conn->send(msg);

    assert(!conn->getContext().empty());
    WeakEntryPtr weakEntry(any_cast<WeakEntryPtr>(conn->getContext()));
    //通过weakEntry提升为shared_ptr,得到同一个shared_ptr，不能new新的shared_ptr
    //否则onTime会调用之前EntryPtr析构函数,关闭掉conn的连接
    EntryPtr entry(weakEntry.lock());
    if (entry)
    {
        connectionBuckets_.back().insert(entry);//当有消息到来时，将该条连接插入当前循环队列所指向容器中
        dumpConnectionBuckets();
    }
}

//定时任务，每秒向环形队列中塞空的Bucket,清除到达时间没有触发的链接
void EchoServer::onTime()
{
    connectionBuckets_.push_back(Bucket());//会覆盖掉之前的Bucket
    dumpConnectionBuckets();
}

void EchoServer::dumpConnectionBuckets()
{
    LOG_INFO << "size = " << connectionBuckets_.size();
    for (size_t i = 0; i < connectionBuckets_.size(); ++i)
    {
        const Bucket& bucket = connectionBuckets_[i];
        LOG_INFO << "[" << i << "]" << "bucket = " << bucket.size();

        for (auto element : bucket)
        {
            bool connectionDead = element->weakConn_.expired();
            LOG_TRACE << element.get() << "(" << element.use_count()
                << ") " << (connectionDead ? "dead" : "alive");
        }
    }
}
/*************************************************************************
  > File Name: EchoServer.h
  > Author: pengjunjie
  > Mail: 1002398145@qq.com 
  > Created Time: 2019年02月27日 星期三 21时22分29秒
 ************************************************************************/

#include "../../base/TcpServer.h"
#include "../../base/TcpConnection.h"
#include "../../base/Callbacks.h"
#include "../../base/EventLoop.h"
#include "../../base/RingBuffer.hpp"

#include <memory>
#include <unordered_set>

 //Echo服务器，用Timing Wheel踢出空闲连接
class EchoServer
{
public:
	EchoServer(pallette::EventLoop* loop,
		const pallette::InetAddress& listenAddr, int idleSeconds);
	void start();

private:
	void onConnection(const pallette::TcpConnectionPtr& conn);
	void onMessage(const pallette::TcpConnectionPtr& conn,
		pallette::Buffer* buf, pallette::Timestamp time);

	void onTime();
	void dumpConnectionBuckets();

	typedef std::weak_ptr<pallette::TcpConnection> WeakTcpconnectionPtr;

	//Entry标记每条连接
	struct Entry
	{
		Entry(const WeakTcpconnectionPtr& weakConn)
			:weakConn_(weakConn)
		{
		}

		//Entry被销毁，连接断开失效
		~Entry()
		{
			pallette::TcpConnectionPtr conn = weakConn_.lock();
			if (conn)
			{
				conn->shutdown();
			}
		}

		WeakTcpconnectionPtr weakConn_;
	};

	typedef std::shared_ptr<Entry> EntryPtr;
	typedef std::weak_ptr<Entry> WeakEntryPtr;
	typedef std::unordered_set<EntryPtr> Bucket;//去重，节省空间时间
	typedef pallette::RingBuffer<Bucket> WeakConnectionList;

	pallette::TcpServer server_;
	WeakConnectionList connectionBuckets_;//循环队列保存连接
};
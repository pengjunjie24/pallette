/*************************************************************************
  > File Name: hub.cpp
  > Author: pengjunjie
  > Mail: 1002398145@qq.com 
  > Created Time: 2019年03月05日 星期二 21时10分25秒
 ************************************************************************/

#include "codec.h"

#include "../../base/Logging.h"
#include "../../base/Timestamp.h"
#include "../../base/EventLoop.h"
#include "../../base/TcpServer.h"

#include <map>
#include <set>
#include <stdio.h>

using namespace pallette;
using namespace std;

namespace pubsub
{
	typedef std::set<string> ConnectionSubscription;

	class Topic
	{
	public:
		Topic(const string& topic)
			:topic_(topic)
		{
		}

		void add(const TcpConnectionPtr& conn)
		{
			audiences_.insert(conn);
			if (lastPubTime_.valid())
			{
				conn->send(makeMessage());
			}
		}

		void remove(const TcpConnectionPtr& conn)
		{
			audiences_.erase(conn);
		}

		void publish(const string& content, Timestamp time)
		{
			content_ = content;
			lastPubTime_ = time;
			string message = makeMessage();

			for (auto& audience : audiences_)
			{
				audience->send(message);
			}
		}

	private:

		string makeMessage()
		{
			return "pub " + topic_ + "\r\n" + content_ + "\r\n";
		}

		string topic_;
		string content_;
		Timestamp lastPubTime_;
		std::set<TcpConnectionPtr> audiences_;
	};
}
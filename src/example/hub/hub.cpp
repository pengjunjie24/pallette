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

    //主题，发布者把消息发到主题上，接收者接收该主题上的消息
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

        string topic_;//主题名称
        string content_;//在该主题上发布的内容
        Timestamp lastPubTime_;//最近一次在该主题上发布消息的时间
        std::set<TcpConnectionPtr> audiences_;//关注该主题的接收者
    };

    class PubSubServer : noncopyable
    {
    public:
        PubSubServer(EventLoop* loop, InetAddress& listenAddr)
            :loop_(loop)
            , server_(loop, listenAddr, "PubSbuServer")
        {
            server_.setConnectionCallback(std::bind(
                &PubSubServer::onConnection, this, _1));
            server_.setMessageCallback(std::bind(
                &PubSubServer::onMessage, this, _1, _2, _3));

            loop_->runEvery(1.0, std::bind(
                &PubSubServer::timePublish, this));
        }

        void start()
        {
            server_.start();
        }

    private:
        void onConnection(const TcpConnectionPtr& conn)
        {
            if (conn->connected())
            {
                conn->setContext(ConnectionSubscription());
            }
            else
            {
                const ConnectionSubscription& connSub
                    = any_cast<const ConnectionSubscription&>(conn->getContext());

                for (const auto subscription : connSub)
                {
                    
                }
            }
        }

        void onMessage(const TcpConnectionPtr& conn, Buffer* buf,
            Timestamp recvTime)
        {

        }

        void timePublish()
        {
            Timestamp now = Timestamp::now();
        }

        void doUnsubscribe(const TcpConnectionPtr& conn,
            const string& topic)
        {
            LOG_INFO << conn->name() << " unsubscribes " << topic;
            getTopic(topic).remove(conn);
        }

        Topic& getTopic(const string& topic)
        {
            auto it = topics_.find(topic);
            if (it == topics_.end())
            {
                it = topics_.insert(make_pair(topic, Topic(topic))).first;
            }
            return it->second;
        }

        EventLoop* loop_;
        TcpServer server_;
        std::map<string, Topic> topics_;
    };
}
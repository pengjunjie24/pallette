/*************************************************************************
> File Name: MultiplexerSimple.cpp
> Author: pengjunjie
> Mail: 1002398145@qq.com
> Created Time: 2019年03月04日 星期一 15时27分18秒
************************************************************************/

#include "../../base/Logging.h"
#include "../../base/EventLoop.h"
#include "../../base/InetAddress.h"
#include "../../base/TcpClient.h"
#include "../../base/TcpServer.h"
#include "../../base/Callbacks.h"

#include <queue>
#include <utility>
#include <stdio.h>
#include <unistd.h>

using namespace pallette;

const int kMaxConns = 10;  // 65535
const size_t kMaxPacketLen = 255;
const size_t kHeaderLen = 3;

//外部多个连接的接入，并可以把这个连接上的数据包汇总发到后端的一个连接上。
//就是一个N对1的数据处理器。把N个TCP数据流合成一个数据流,类似于数电中数据选择器

//数据协议:个数据片段有 3 字节的数据头。分别是数据长度1字节和2字节的连接id号。
//这个服务仅仅做数据流的合并，而不规定数据逻辑上的分包。对内的数据管道上看起来的数据流就是这样的：
//len id_lo id_hi content ... len id_lo id_hi content ...  len id_lo id_hi content ...
class MultiplexServer : noncopyable
{
public:
    MultiplexServer(EventLoop* loop, const InetAddress& listenAddr, const InetAddress& backendAddr)
        :server_(loop, listenAddr, "MultiplexServer"),
        backend_(loop, backendAddr, "MultiplexBackend")
    {
        server_.setConnectionCallback(
            std::bind(&MultiplexServer::onClientConnection, this, _1));
        server_.setMessageCallback(
            std::bind(&MultiplexServer::onClientMessage, this, _1, _2, _3));

        backend_.setConnectionCallback(
            std::bind(&MultiplexServer::onBackendConnection, this, _1));
        backend_.setMessageCallback(
            std::bind(&MultiplexServer::onBackendMessage, this, _1, _2, _3));
        backend_.enableRetry();
    }

    void start()
    {
        backend_.connect();
        server_.start();
    }

private:
    void onClientConnection(const TcpConnectionPtr& conn)
    {
        LOG_TRACE << "Client " << conn->peerAddress().toIpPort() << " -> "
            << conn->localAddress().toIpPort() << " is "
            << (conn->connected() ? "UP" : "DOWN");

        if (conn->connected())
        {
            int id = -1;
            if (!availIds_.empty())
            {
                id = availIds_.front();
                availIds_.pop();
                clientConns_[id] = conn;
            }

            if (id < 0)
            {
                //达到最大连接数
                conn->shutdown();
            }
            else
            {
                conn->setContext(id);
                char buf[256] = { 0 };
                snprintf(buf, sizeof(buf), "CONN %d FROM %s IS UP\r\n",
                    id, conn->peerAddress().toIpPort().c_str());
                sendBackendString(0, buf);
            }
        }
        else
        {
            if (!conn->getContext().empty())
            {
                int id = any_cast<int>(conn->getContext());
                assert(id > 0 && id <= kMaxConns);
                char buf[256] = { 0 };
                snprintf(buf, sizeof(buf), "CONN %d FROM %s IS DOWN\r\n",
                    id, conn->peerAddress().toIpPort().c_str());
                sendBackendString(0, buf);

                if (backendConn_)
                {
                    //将id重新加入可用列表，重复使用
                    availIds_.push(id);
                    clientConns_.erase(id);
                }
                else
                {
                    assert(availIds_.empty());
                    assert(clientConns_.empty());
                }
            }
        }
    }

    void sendBackendString(int id, const string& msg)
    {
        assert(msg.size() <= kMaxPacketLen);
        Buffer buf;
        buf.append(msg);
        sendBackendPacket(id, &buf);
    }


    void sendBackendPacket(int id, Buffer* buf)
    {
        size_t len = buf->readableBytes();
        LOG_DEBUG << "sendBackendPacket " << len;
        assert(len <= kMaxPacketLen);
        uint8_t header[kHeaderLen] = {
            static_cast<uint8_t>(len),
            static_cast<uint8_t>(id & 0xFF),
            static_cast<uint8_t>((id & 0xFF00) >> 8)
        };
        buf->prepend(header, kHeaderLen);
        if (backendConn_)
        {
            backendConn_->send(buf);
        }
    }

    void onClientMessage(const TcpConnectionPtr& conn, Buffer* buf, Timestamp)
    {
        if (!conn->getContext().empty())
        {
            int id = any_cast<int>(conn->getContext());
            sendBackendBuffer(id, buf);
        }
        else
        {
            buf->retrieveAll();
            //错误处理
            LOG_ERROR << "conn is not id, disconnecting...";
            conn->shutdown();
        }
    }

    //发送消息到逻辑服务器
    void sendBackendBuffer(int id, Buffer* buf)
    {
        while (buf->readableBytes() > kMaxPacketLen)
        {
            Buffer packet;
            packet.append(buf->peek(), kMaxPacketLen);
            buf->retrieve(kMaxPacketLen);
            sendBackendPacket(id, &packet);
        }

        if (buf->readableBytes() > 0)
        {
            sendBackendPacket(id, buf);
        }
    }

    void onBackendConnection(const TcpConnectionPtr& conn)
    {
        LOG_TRACE << "Backend " << conn->localAddress().toIpPort() << " -> "
            << conn->peerAddress().toIpPort() << " is "
            << (conn->connected() ? "UP" : "DOWN");

        if (conn->connected())
        {
            backendConn_ = conn;
            assert(availIds_.empty());
            for (int i = 1; i <= kMaxConns; ++i)
            {
                availIds_.push(i);//添加可用空闲id
            }
        }
        else
        {
            backendConn_.reset();
            //和逻辑服务器断开后，断开所有client,删除所有id
            for (auto& clientConn : clientConns_)
            {
                (clientConn.second)->shutdown();
            }
            clientConns_.clear();

            while (!availIds_.empty())
            {
                availIds_.pop();
            }
        }
    }

    void onBackendMessage(const TcpConnectionPtr& conn, Buffer* buf, Timestamp)
    {
        sendToClient(buf);
    }

    //发送给客户端
    void sendToClient(Buffer* buf)
    {
        while (buf->readableBytes() > kHeaderLen)
        {
            int len = static_cast<uint8_t>(*buf->peek());
            if (buf->readableBytes() < len + kHeaderLen)
            {
                break;
            }
            else
            {
                int id = static_cast<uint8_t>(buf->peek()[1]);
                id |= (static_cast<uint8_t>(buf->peek()[2]) << 8);

                if (id != 0)
                {
                    auto it = clientConns_.find(id);
                    if (it != clientConns_.end())
                    {
                        it->second->send(buf->peek() + kHeaderLen, len);
                    }
                }
                else
                {
                    std::string cmd(buf->peek() + kHeaderLen, len);
                    LOG_INFO << "Backend cmd " << cmd;
                    doCommand(cmd);
                }
                buf->retrieve(len + kHeaderLen);
            }
        }
    }

    void doCommand(const std::string& cmd)
    {
        static const string kDisconnectCmd = "DISCONNECT ";

        if (cmd.size() > kDisconnectCmd.size()
            && std::equal(kDisconnectCmd.begin(), kDisconnectCmd.end(), cmd.begin()))
        {
            int connId = atoi(&cmd[kDisconnectCmd.size()]);
            auto it = clientConns_.find(connId);
            if (it != clientConns_.end())
            {
                it->second->shutdown();
            }
        }
    }

    TcpServer server_;//作为客户端连接上来的服务器
    TcpClient backend_;//作为连接逻辑处理服务器的客户端
    TcpConnectionPtr backendConn_;//连接到逻辑处理服务器的连接
    std::map<int, TcpConnectionPtr> clientConns_;//id和连接对应的map
    std::queue<int> availIds_;//可用的id列表
};
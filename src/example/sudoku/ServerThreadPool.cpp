/*************************************************************************
  > File Name: ServerThreadPool.cpp
  > Author: pengjunjie
  > Mail: 1002398145@qq.com
  > Created Time: 2019年06月26日 星期三 14时21分28秒
 ************************************************************************/

#include "Sudoku.h"
#include "ServerDataConfig.h"

#include "../../base/TcpServer.h"
#include "../../base/Logging.h"
#include "../../base/EventLoop.h"
#include "../../base/InetAddress.h"
#include "../../base/ThreadPool.h"

#include <stdlib.h>

using namespace pallette;
using namespace std;

//数独服务器
//一个IO线程接收多条链接，数独的计算放进线程池中进行，高效利用多核优势
class SudokuServer
{
public:
    SudokuServer(EventLoop* loop, const InetAddress& listenAddr, int numThread)
        :server_(loop, listenAddr, "SudokuServer")
        , numThreads_(numThread)
        , startTime_(Timestamp::now())
    {
        server_.setConnectionCallback(std::bind(
            &SudokuServer::onConnection, this, _1));
        server_.setMessageCallback(std::bind(
            &SudokuServer::onMessage, this, _1, _2, _3));
    }

    void start()
    {
        LOG_INFO << "starting " << numThreads_ << " threads";
        threadPool_.start(numThreads_);
        server_.start();
    }

private:
    void onConnection(const TcpConnectionPtr& conn)
    {
        LOG_TRACE << conn->peerAddress().toIpPort() << " -> "
            << conn->localAddress().toIpPort() << " is "
            << (conn->connected() ? "UP" : "DOWN");
    }

    void onMessage(const TcpConnectionPtr& conn, Buffer* buf, Timestamp)
    {
        LOG_DEBUG << conn->name();
        size_t len = buf->readableBytes();
        while (len >= kCells + 2)//缓冲区数据达到最小长度
        {
            const char* crlf = buf->findStr("\r\n");//每条数据以/r/n结尾
            if (crlf)
            {
                string request(buf->peek(), crlf);
                buf->retrieveUntil(crlf + 2);
                len = buf->readableBytes();
                if (!processRequest(conn, request))
                {
                    conn->send("Bad Request!\r\n");
                    conn->shutdown();
                    break;
                }
            }
            else if (len > 100) // id + ":" + kCells + "\r\n"
            {
                conn->send("Id too long!\r\n");
                conn->shutdown();
                break;
            }
            else
            {
                break;
            }
        }
    }

    bool processRequest(const TcpConnectionPtr& conn, const string& request)//处理每道数独问题
    {
        string id;
        string puzzle;
        bool goodRequest = true;

        string::const_iterator colon = find(request.begin(), request.end(), ':');
        if (colon != request.end())
        {
            id.assign(request.begin(), colon);
            puzzle.assign(colon + 1, request.end());
        }
        else
        {
            puzzle = request;
        }

        if (static_cast<int>(puzzle.size()) == kCells)
        {
            threadPool_.run(std::bind(&solve, conn, puzzle, id));
        }
        else
        {
            goodRequest = false;
        }
        return goodRequest;
    }

    static void solve(const TcpConnectionPtr& conn,
        const string& puzzle, const string& id)
    {
        LOG_DEBUG << conn->name();
        string result = solveSudoku(puzzle);
        if (id.empty())
        {
            conn->send(result + "\r\n");
        }
        else
        {
            conn->send(id + ":" + result + "\r\n");
        }
    }

    TcpServer server_;
    ThreadPool threadPool_;
    int numThreads_;
    Timestamp startTime_;

};

int main(int argc, char* argv[])
{
    if (argc < 2)
    {
        printf("Usage: %s config\n", argv[0]);
        return 0;
    }
    ServerDataConfig serverConfig(argv[1]);
    int numThread = serverConfig.ServerThreadPoolNumber();
    InetAddress listenAddr(serverConfig.serverIp(), serverConfig.serverPort());

    LOG_INFO << "pid = " << process_info::pid() << ", tid = " << process_info::tid();
    EventLoop loop;
    SudokuServer server(&loop, listenAddr, numThread);

    server.start();
    loop.loop();
}
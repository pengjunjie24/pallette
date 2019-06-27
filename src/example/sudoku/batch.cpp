/*************************************************************************
  > File Name: batch.cpp
  > Author: pengjunjie
  > Mail: 1002398145@qq.com
  > Created Time: 2019年06月23日 星期日 16时12分43秒
 ************************************************************************/

#include "Sudoku.h"
#include "ClientDataConfig.h"

#include "../../base/Logging.h"
#include "../../base/EventLoop.h"
#include "../../base/TcpClient.h"
#include "../../base/Callbacks.h"

#include <fstream>
#include <stdio.h>

using namespace pallette;

//数独测试
//local为本地测试，测试求解数独的函数性能
//client连接到服务器，一次性将所有数据发往服务端，等待服务器返回所耗费时间
bool verify(const std::string& result)
{
    return (result == kNoSolution) ? false : true;
}

void runLocal(std::istream& in)//本地运行
{
    Timestamp start(Timestamp::now());
    std::string line;
    int count = 0;
    int succeed = 0;
    while (getline(in, line))//取走每一行数据
    {
        if (static_cast<int>(line.size()) == kCells)
        {
            ++count;
            if (verify(solveSudoku(line)))
            {
                ++succeed;
            }
        }
    }
    double elapsed = timeDifference(Timestamp::now(), start);

    printf("count: %d, success: %d\n", count, succeed);
    printf("%.3f sec, %.3f us per sudoku. \n", elapsed, 1000 * 1000 * elapsed / count);
}

typedef std::vector<std::string> Input;
typedef std::shared_ptr<Input> InputPtr;

InputPtr readInput(std::istream& in)//将输入的数独题目放入队列中
{
    InputPtr input(new Input);
    std::string line;
    while (getline(in, line))
    {
        if (static_cast<int>(line.size()) == kCells)
        {
            input->push_back(line.c_str());
        }
    }
    return input;
}

typedef std::function<void(const std::string&, double, int)> DoneCallback;
class SudokuClient : noncopyable
{
public:
    SudokuClient(EventLoop* loop, const InetAddress& serverAddr,
        const InputPtr& input, const std::string& name, const DoneCallback& cb)
        :name_(name)
        , client_(loop, serverAddr, name_)
        , input_(input)
        , success_(new Input)
        , cb_(cb)
        , count_(0)
    {
        client_.setConnectionCallback(std::bind(
            &SudokuClient::onConnection, this, _1));
        client_.setMessageCallback(std::bind(
            &SudokuClient::onMessage, this, _1, _2, _3));
    }

    void connect()
    {
        client_.connect();
    }

private:
    void onConnection(const TcpConnectionPtr& conn)
    {
        if (conn->connected())//连接成功后将所有题目发送到服务端解答
        {
            LOG_INFO << name_ << " connected";
            start_ = Timestamp::now();//记录开始时间
            for (size_t i = 0; i < input_->size(); ++i)
            {
                LogStream buf;
                buf << i + 1 << ":" << (*input_)[i] << "\r\n";
                conn->send(buf.getBuffer().getString());
            }
            LOG_INFO << name_ << " sent requests: " << input_->size();
        }
        else
        {
            LOG_INFO << name_ << " disconnected";
        }
    }

    void onMessage(const TcpConnectionPtr& conn, Buffer* buf, Timestamp)
    {
        size_t len = buf->readableBytes();
        while (len >= kCells + 2)//大于最小长度 Cells+"\r\n"
        {
            const char* crlf = buf->findStr("\r\n");
            if (crlf)//找到应答的结束标识"\r\n"
            {
                std::string response(buf->peek(), crlf);//取出数独结果
                buf->retrieveUntil(crlf + 2);//在缓冲区中清空该结果
                len = buf->readableBytes();//更新缓存区长度
                ++count_;
                if (!verify(response))
                {
                    LOG_ERROR << "Bad response:" << response;
                    conn->shutdown();
                    break;
                }
                success_->push_back(response);
            }
            else if (len > 100) // id + ":" + kCells + "\r\n"
            {
                LOG_ERROR << "Line is too long!";
                conn->shutdown();
                break;
            }
            else
            {
                break;
            }
        }

        if (count_ == static_cast<int>(input_->size()))//得到所有的数独结果
        {
            LOG_INFO << name_ << " done.";
            double elapsed = timeDifference(Timestamp::now(), start_);
            cb_(name_, elapsed, count_);
            conn->shutdown();
        }
    }

    std::string name_;//客户端名字
    TcpClient client_;
    InputPtr input_;//数独题目
    InputPtr success_;//服务器返回题目答案
    DoneCallback cb_;//完成后的回调函数
    int count_;//完成题目个数
    Timestamp start_;//开始时间
};


Timestamp gStart;//系统开始运行时间
int gConnections;//连接到服务器的个数
int gFinished;
EventLoop* gLoop;//所在IO线程


void done(const std::string& name, double elapsed, int count)//客户端运行完成时的回调函数
{
    LOG_INFO << name << " " << elapsed << " seconds,  "
        << 1000 * 1000 * elapsed / count << " us per request.";

    ++gFinished;
    if (gFinished == gConnections)//所有客户端都收到服务器应答
    {
        gLoop->runAfter(1.0, std::bind(&EventLoop::quit, gLoop));
        double total = timeDifference(Timestamp::now(), gStart);
        LOG_INFO << "total " << total << " seconds, "
            << (total / gConnections) << " seconds per client";
    }
}

void runClient(std::istream& in, const InetAddress& serverAddr, int conn)
{
    InputPtr input(readInput(in));
    EventLoop loop;
    gLoop = &loop;
    gConnections = conn;

    gStart = Timestamp::now();//开始运行时间
    std::vector<std::unique_ptr<SudokuClient>> clients;
    for (int i = 0; i < conn; ++i)
    {
        std::string name = "client-";
        name += i + 1;
        clients.emplace_back(new SudokuClient(&loop, serverAddr, input, name, done));
        clients.back()->connect();
    }

    loop.loop();
}

int main(int argc, char* argv[])
{
    if (argc < 2)
    {
        printf("Usage: %s config\n", argv[0]);
        return 0;
    }

    ClientDataConfig clientConfig(argv[1]);
    int conn = clientConfig.tcpClientNum();
    InetAddress serverAddr(clientConfig.serverIp(), clientConfig.serverPort());
    const char* input = clientConfig.inputFile().c_str();
    bool local = clientConfig.localSudoku();

    std::ifstream in(input);
    if (in)
    {
        if (local)
        {
            runLocal(in);
        }
        else
        {
            runClient(in, serverAddr, conn);
        }
    }
    else
    {
        printf("Cannot open %s\n", input);
    }
}

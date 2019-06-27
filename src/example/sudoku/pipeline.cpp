/*************************************************************************
  > File Name: pipeline.cpp
  > Author: pengjunjie
  > Mail: 1002398145@qq.com
  > Created Time: 2019年06月26日 星期三 16时20分45秒
 ************************************************************************/

#include "Sudoku.h"
#include "percentile.h"
#include "ClientDataConfig.h"

#include "../../base/Logging.h"
#include "../../base/EventLoop.h"
#include "../../base/TcpClient.h"
#include "../../base/Callbacks.h"

#include<unordered_map>
#include <fstream>
#include <stdlib.h>

using namespace pallette;

typedef std::vector<std::string> Input;
typedef std::shared_ptr<const Input> InputPtr;

//测试数独库服务器
//可以设置连接个数，流水线个数(每次发送多少个数独题目)
class SudokuClient : noncopyable
{
public:
    SudokuClient(EventLoop* loop, const InetAddress& serverAddr,
        const InputPtr& input, const std::string& name,
        int pipelines, bool nodelay)
        : name_(name)
        , pipelines_(pipelines)
        , tcpNoDelay_(nodelay)
        , client_(loop, serverAddr, name_)
        , input_(input)
        , count_(0)
    {
        client_.setConnectionCallback(
            std::bind(&SudokuClient::onConnection, this, _1));
        client_.setMessageCallback(
            std::bind(&SudokuClient::onMessage, this, _1, _2, _3));
    }

    void connect()
    {
        client_.connect();
    }

    void report(std::vector<int>* latency, int* infly)
    {
        latency->insert(latency->end(), latencies_.begin(), latencies_.end());
        latencies_.clear();
        *infly += static_cast<int>(sendTime_.size());
    }

private:
    void onConnection(const TcpConnectionPtr& conn)
    {
        if (conn->connected())
        {
            LOG_INFO << name_ << " connected";
            if (tcpNoDelay_)
            {
                conn->setTcpNoDelay(true);
            }
            conn_ = conn;
            send(pipelines_);//每次发送pipelines_条数独题目
        }
        else
        {
            LOG_INFO << name_ << " disconnected";
            conn_.reset();
            // FIXME: exit
        }
    }

    void send(int n)//发送n条数独的题目到数独服务器
    {
        Timestamp now(Timestamp::now());
        Buffer requests;
        for (int i = 0; i < n; ++i)
        {
            char buf[256] = { 0 };
            const std::string& req = (*input_)[count_ % input_->size()];
            int len = snprintf(buf, sizeof(buf), "%s-%08d:%s\r\n",
                name_.c_str(), count_, req.c_str());
            requests.append(buf, len);
            sendTime_[count_++] = now;//记录发送时的时间
        }
        conn_->send(&requests);
    }

    void onMessage(const TcpConnectionPtr& conn, Buffer* buf, Timestamp recvTime)
    {
        size_t len = buf->readableBytes();
        while (len >= kCells + 2)
        {
            const char* crlf = buf->findStr("\r\n");
            if (crlf)
            {
                std::string response(buf->peek(), crlf);
                buf->retrieveUntil(crlf + 2);
                len = buf->readableBytes();
                if (verify(response, recvTime))//回应数据是否合法
                {
                    send(1);//接收到服务器应答的一条就继续发送
                }
                else
                {
                    LOG_ERROR << "Bad response:" << response;
                    conn->shutdown();
                    break;
                }
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
    }

    bool verify(const std::string& response, Timestamp recvTime)//服务器应答数据是否合法
    {
        size_t colon = response.find(':');
        if (colon != std::string::npos)
        {
            size_t dash = response.find('-');
            if (dash != std::string::npos && dash < colon)
            {
                int id = atoi(response.c_str() + dash + 1);
                std::unordered_map<int, Timestamp>::iterator sendTime = sendTime_.find(id);
                if (sendTime != sendTime_.end())
                {
                    int64_t latencyUs = recvTime.getMicroSeconds()
                        - sendTime->second.getMicroSeconds();//计算出延迟时间
                    latencies_.push_back(static_cast<int>(latencyUs));
                    sendTime_.erase(sendTime);//从发送的记录中删除该条记录
                }
                else
                {
                    LOG_ERROR << "Unknown id " << id << " of " << name_;
                }
            }
        }
        else
        {
            LOG_ERROR << "response not ':'";
            return false;
        }
        return true;
    }

    const std::string name_;
    const int pipelines_;//每次发送的数独问题个数
    const bool tcpNoDelay_;//是否关闭Nagle算法
    TcpClient client_;
    TcpConnectionPtr conn_;
    const InputPtr input_;//从文件中读入的数独数据
    int count_;//记录当前客户端已发送数据个数，并作为每条数据的id
    std::unordered_map<int, Timestamp> sendTime_;//记录每条数独数据的发送时间
    std::vector<int> latencies_;//记录每条数独数据的延迟时间
};

void report(const std::vector<std::unique_ptr<SudokuClient>>& clients)
{
    static int count = 0;

    std::vector<int> latencies;
    int infly = 0;
    for (const auto& client : clients)
    {
        client->report(&latencies, &infly);//得到延迟的数据和未回收的数据的个数
    }

    Percentile p(latencies, infly);
    LOG_INFO << p.report();

    char buf[64] = { 0 };
    snprintf(buf, sizeof(buf), "p%04d", count);
    p.save(latencies, buf);
    ++count;
}

InputPtr readInput(std::istream& in)
{
    std::shared_ptr<Input> input(new Input);
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

void runClient(const InputPtr& input, const InetAddress& serverAddr,
    int conn, int pipelines, bool nodelay)
{
    EventLoop loop;
    std::vector<std::unique_ptr<SudokuClient>> clients;
    for (int i = 0; i < conn; ++i)
    {
        Fmt f("c%04d", i + 1);
        std::string name(f.data(), f.length());
        clients.emplace_back(new SudokuClient(&loop, serverAddr, input, name, pipelines, nodelay));
        clients.back()->connect();
    }

    loop.runEvery(1.0, std::bind(report, std::ref(clients)));
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

    std::string inputFile = clientConfig.inputFile();
    int conn = clientConfig.tcpClientNum();
    int pipelines = clientConfig.pipelineNum();
    bool nodelay = clientConfig.noDelay();
    InetAddress serverAddr(clientConfig.serverIp(), clientConfig.serverPort());

    std::ifstream in(inputFile.c_str());
    if (in)
    {
        InputPtr input(readInput(in));
        printf("%zd requests from %s\n", input->size(), inputFile.c_str());
        runClient(input, serverAddr, conn, pipelines, nodelay);
    }
    else
    {
        printf("Cannot open %s\n", argv[1]);
    }
}
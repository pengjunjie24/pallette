/*************************************************************************
	> File Name: Common.h
	> Author: pengjunjie
	> Mail: 1002398145@qq.com
	> Created Time: 2019年03月14日 星期四 22时30分36秒
 ************************************************************************/

#ifndef EXAMPLE_COMMON_H
#define EXAMPLE_COMMON_H

#include <string>
#include <stdint.h>

//配置文件中参数
struct Options
{
    uint16_t port;//连接的端口
    int length;//一次发送消息的长度
    int number;//发送的次数
    bool transmit;//客户端启动选项
    bool receive;//服务器启动选项
    bool nodelay;
    std::string host;//服务器地址
    Options()
        :port(0), length(0), number(0)
        , transmit(false), receive(false), nodelay(false)
    {
    }
};

bool parseConfigurationFile(const char* filename, Options* opt);
struct sockaddr_in resolveOrDie(const char* host, uint16_t port);

//消息的格式
struct SessionMessage
{
    int32_t number;//服务器客户端应答次数
    int32_t length;//每次发送消息的长度
}__attribute__((__packed__));//按照1个字节对齐

//消息的信息
struct PayloadMessage
{
    int32_t length;//消息长度
    char data[0];//放在结构体最后一个元素，由运行时决定，分配多少就有多少。消息的存储
};

void transmit(const Options& opt);//客户端
void receive(const Options& opt);//服务器

#endif

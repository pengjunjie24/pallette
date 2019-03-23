/*************************************************************************
> File Name: DataConfig.h
> Author: pengjunjie
> Mail: 1002398145@qq.com
> Created Time: 2019年02月26日 星期二 14时06分57秒
************************************************************************/

#ifndef EXAMPLE_DATA_CONFIG_H
#define EXAMPLE_DATA_CONFIG_H

#include "../base/Config.h"

#include <string>

//该配置文件内部各种参数不会改变
class DataConfig
{
public:
    DataConfig(std::string filename)
        : fixedConfig_(filename)
    {
    }

    std::string serverIp() { return fixedConfig_.read <std::string>("ServerIp", ""); }
    std::string domainName() { return fixedConfig_.read<std::string>("DomainName", ""); }
    uint16_t serverPort() { return fixedConfig_.read<uint16_t>("ServerPort", 10000); }
    std::string downloadServerFile() { return fixedConfig_.read<std::string>("ServerDownloadFile", "downloadServerTest.txt"); }//服务器存放文件
    std::string downloadClientFile() { return fixedConfig_.read<std::string>("ClientDownloadFile", "downloadClientTest.txt"); }//客户端下载文件

    int messageLength(){ return fixedConfig_.read<int>("MessageLength", 1024 * 1024); }//一条消息长度
    int messageNumber(){ return fixedConfig_.read<int>("MessageNumber", 1024); }//消息发送次数
    std::string transmitOrReceive(){ return fixedConfig_.read<std::string>("StartFunction", "transmit"); }//以服务器或者客户端方式启动

private:
    pallette::Config fixedConfig_;
};

#endif
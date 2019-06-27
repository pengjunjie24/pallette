/*************************************************************************
  > File Name: ServerDataConfig.h
  > Author: pengjunjie
  > Mail: 1002398145@qq.com
  > Created Time: 2019年06月26日 星期三 19时27分28秒
 ************************************************************************/

#ifndef EXAMPLE_SERVERDATACONFIG_H
#define EXAMPLE_SERVERDATACONFIG_H

#include "../../base/Config.h"

#include <string>

 //该配置文件内部各种参数不会改变
class ServerDataConfig
{
public:
    ServerDataConfig(std::string filename)
        : fixedConfig_(filename)
    {
    }

    std::string serverIp() { return fixedConfig_.read <std::string>("ServerIp", "127.0.0.1"); }
    uint16_t serverPort() { return fixedConfig_.read<uint16_t>("ServerPort", 10000); }

    int ServerThreadPoolNumber() { return fixedConfig_.read<int>("ThreadPoolNumber", 1); }//线程池个数
    int multiLoopNumber() { return fixedConfig_.read<int>("MultiLoopNumber", 1); }//IO线程池个数

private:
    pallette::Config fixedConfig_;
};

#endif

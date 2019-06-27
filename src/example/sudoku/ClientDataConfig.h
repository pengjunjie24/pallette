/*************************************************************************
  > File Name: ClientDataConfig.h
  > Author: pengjunjie
  > Mail: 1002398145@qq.com
  > Created Time: 2019年06月26日 星期三 19时27分38秒
 ************************************************************************/

#ifndef EXAMPLE_CLIENTDATACONFIG_H
#define EXAMPLE_CLIENTDATACONFIG_H


#include "../../base/Config.h"

#include <string>

class ClientDataConfig
{
public:
    ClientDataConfig(std::string filename)
        : fixedConfig_(filename)
    {
    }

    std::string serverIp() { return fixedConfig_.read <std::string>("ServerIp", "127.0.0.1"); }
    uint16_t serverPort() { return fixedConfig_.read<uint16_t>("ServerPort", 10000); }

    std::string inputFile() { return fixedConfig_.read<std::string>("SudoFile", "sudoku_small"); }
    int tcpClientNum() { return fixedConfig_.read<int>("TcpClientNum", 1); }//客户端个数
    int pipelineNum() { return fixedConfig_.read<int>("PipelineNum", 1); }//流水线个数
    int noDelay() { return fixedConfig_.read<int>("Nodelay", 0); }
    int localSudoku() { return fixedConfig_.read<int>("LocalSudoku", 0); }

private:
    pallette::Config fixedConfig_;
};

#endif

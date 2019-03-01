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
		, serverIp_(fixedConfig_.read <std::string>("ServerIp", "127.0.0.1"))
		, serverPort_(fixedConfig_.read<uint16_t>("ServerPort", 10000))
		, downloadServerFile_(fixedConfig_.read<std::string>("ServerDownloadFile", "downloadServerTest.txt"))
		, downloadClientFile_(fixedConfig_.read<std::string>("ClientDownloadFile", "downloadClientTest.txt"))
	{
	}

	const std::string& serverIp() { return serverIp_; }
	const uint16_t& serverPort() { return serverPort_; }
	const std::string& downloadServerFile() { return downloadServerFile_; }
	const std::string& downloadClientFile() { return downloadClientFile_; }

private:
	pallette::Config fixedConfig_;

	std::string serverIp_;
	uint16_t serverPort_;
	std::string downloadServerFile_;//服务器存放文件
	std::string downloadClientFile_;//客户端下载文件
};

#endif
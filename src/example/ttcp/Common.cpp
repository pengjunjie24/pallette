/*************************************************************************
	> File Name: Common.cpp
	> Author: pengjunjie
	> Mail: 1002398145@qq.com
	> Created Time: 2019年03月14日 星期四 22时30分43秒
 ************************************************************************/

#include "Common.h"

#include "../DataConfig.h"

#include <iostream>
#include <string>
#include <string.h>
#include <netdb.h>
#include <stdio.h>
#include <assert.h>

bool parseConfigurationFile(const char* filename, Options* opt)
{
    DataConfig config(std::string(filename, strlen(filename)));
    opt->port = config.serverPort();
    opt->length = config.messageLength();
    opt->number = config.messageNumber();
    opt->host = config.serverIp();

    std::string startFunc = config.transmitOrReceive();
    if (startFunc == "transmit")
    {
        opt->transmit = true;
    }
    else if (startFunc == "receive")
    {
        opt->receive = true;
    }
    else
    {
        printf("StartFunction parameter is error: %s", startFunc.c_str());
        return false;
    }
    return true;
}

struct sockaddr_in resolveOrDie(const char* host, uint16_t port)
{
    struct hostent* he = ::gethostbyname(host);
    if (!he)
    {
        perror("gethostbyname");
        exit(1);
    }

    assert(he->h_addrtype == AF_INET && he->h_length == sizeof(uint32_t));
    struct sockaddr_in addr;
    bzero(&addr, sizeof(addr));
    addr.sin_family = AF_INET;
    addr.sin_port = htons(port);
    addr.sin_addr = *reinterpret_cast<struct in_addr*>(he->h_addr);
    return addr;
}
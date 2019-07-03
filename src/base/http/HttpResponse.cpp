/*************************************************************************
  > File Name: HttpResponse.cpp
  > Author: pengjunjie
  > Mail: 1002398145@qq.com
  > Created Time: 2019年06月16日 星期日 11时53分48秒
 ************************************************************************/

#include "HttpResponse.h"
#include "../Buffer.h"

#include <stdio.h>

using namespace pallette;

void HttpResponse::appendToBuffer(Buffer* output) const
{
    char buf[32] = { 0 };
    snprintf(buf, sizeof(buf), "HTTP/1.1 %d", statusCode_);
    output->append(buf);
    output->append(statusMessage_);
    output->append("\r\n");

    if (closeConnection_)
    {
        output->append("Connection: close\r\n");
    }
    else
    {
        snprintf(buf, sizeof(buf), "Content-Length: %zd\r\n", body_.size());
        output->append(buf);
        output->append("Connection: Keep-Alive\r\n");
    }

    for (auto headValue : headers_)
    {
        output->append(headValue.first);
        output->append(": ");
        output->append(headValue.second);
        output->append("\r\n");
    }

    output->append("\r\n");
    output->append(body_);
}
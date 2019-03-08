/*************************************************************************
> File Name: codec.cpp
> Author: pengjunjie
> Mail: 1002398145@qq.com
> Created Time: 2019年03月05日 星期二 19时55分15秒
************************************************************************/

#include "codec.h"

using namespace pallette;
using namespace pubsub;

ParseResult pubsub::parseMessage(Buffer* buf,
    std::string* cmd,
    std::string* topic,
    std::string* content)
{
    ParseResult result = PARSE_ERROR;
    const char* crlf = buf->findCRLF();
    if (crlf)
    {
        const char* space = std::find(buf->peek(), crlf, ' ');
        if (space != crlf)
        {
            cmd->assign(buf->peek(), space);
            topic->assign(space + 1, crlf);
            if (*cmd == "pub")
            {
                const char* start = crlf + 2;
                crlf = buf->findCRLF(start);
                if (crlf)
                {
                    content->assign(start, crlf);
                    buf->retrieveUntil(crlf + 2);
                    result = PARSE_SUCCESS;
                }
                else
                {
                    result = PARSE_CONTINUE;
                }
            }
            else
            {
                buf->retrieveUntil(crlf + 2);
                result = PARSE_SUCCESS;
            }
        }
        else
        {
            result = PARSE_ERROR;
        }
    }
    else
    {
        result = PARSE_CONTINUE;
    }
    return result;
}
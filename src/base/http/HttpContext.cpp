/*************************************************************************
  > File Name: HttpContext.cpp
  > Author: pengjunjie
  > Mail: 1002398145@qq.com
  > Created Time: 2019年06月16日 星期日 13时38分20秒
 ************************************************************************/

#include "HttpContext.h"
#include "../Buffer.h"

#include <string.h>

using namespace pallette;

bool HttpContext::processRequestLine(const char* begin, const char* end)
{
    bool succeed = false;
    const char* start = begin;
    const char* space = std::find(start, end, ' ');
    if (space != end && request_.setMethod(start, space))
    {
        start = space + 1;
        space = std::find(start, end, ' ');
        if (space != end)
        {
            const char* question = std::find(start, space, '?');
            if (question != space)
            {
                request_.setPath(start, question);
                request_.setQuery(question, space);
            }
            else
            {
                request_.setPath(start, space);
            }
            start = space + 1;
            succeed = end - start == 8 && std::equal(start, end - 1, "HTTP/1.");
            if (succeed)
            {
                if (*(end - 1) == '1')
                {
                    request_.setVersion(HttpRequest::HTTP11);
                }
                else if (*(end - 1) == '0')
                {
                    request_.setVersion(HttpRequest::HTTP10);
                }
                else
                {
                    succeed = false;
                }
            }
        }
    }
    return succeed;
}

bool HttpContext::parseRequest(Buffer* buf, Timestamp receiveTime)
{
    const char kCRLF[] = "\r\n";
    bool ok = true;
    bool hasMore = true;
    while (hasMore)
    {
        if (state_ == EXPECT_REQUEST_LINE)
        {
            const char* crlf = buf->findStr(kCRLF);
            if (crlf)
            {
                ok = processRequestLine(buf->peek(), crlf);
                if (ok)
                {
                    request_.setReceiveTime(receiveTime);
                    buf->retrieveUntil(crlf + strlen(kCRLF));
                    state_ = EXPECT_HEADERS;
                }
                else
                {
                    hasMore = false;
                }
            }
            else
            {
                hasMore = false;
            }
        }
        else if (state_ == EXPECT_HEADERS)
        {
            const char* crlf = buf->findStr(kCRLF);
            if (crlf)
            {
                const char* colon = std::find(buf->peek(), crlf, ':');
                if (colon != crlf)
                {
                    request_.addHeader(buf->peek(), colon, crlf);
                }
                else
                {
                    if (request_.method() == HttpRequest::GET)
                    {
                        state_ = GOT_ALL;
                        hasMore = false;
                    }
                    else if (request_.method() == HttpRequest::POST)
                    {
                        state_ = EXPECT_BODY;
                    }
                }
                buf->retrieveUntil(crlf + strlen(kCRLF));
            }
            else
            {
                hasMore = false;
            }
        }
        else if (state_ == EXPECT_BODY)
        {
            std::string contentLengthStr = request_.getHeader("Content-Length");
            if (contentLengthStr.empty())
            {
                hasMore = false;
            }
            else
            {
                int contentLength = std::stoi(contentLengthStr);
                if (buf->readableBytes() >= static_cast<size_t>(contentLength))
                {
                    request_.setQuery(buf->peek(), buf->peek() + contentLength);
                    buf->retrieve(contentLength);

                    state_ = GOT_ALL;
                    hasMore = false;
                }
                else
                {
                    hasMore = false;
                }
            }
        }
    }

    return ok;
}
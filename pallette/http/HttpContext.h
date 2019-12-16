/*************************************************************************
  > File Name: HttpContext.h
  > Author: pengjunjie
  > Mail: 1002398145@qq.com
  > Created Time: 2019年06月16日 星期日 13时38分13秒
 ************************************************************************/

#ifndef PALLETTE_HTTP_CONTEXT_H
#define PALLETTE_HTTP_CONTEXT_H

#include <pallette/http/HttpRequest.h>

namespace pallette
{
    class Buffer;

    //http请求报文解析
    class HttpContext
    {
    public:
        //解析状态
        enum HttpRequestParseState
        {
            EXPECT_REQUEST_LINE,//解析请求行
            EXPECT_HEADERS,//解析请求头部
            EXPECT_BODY,//解析请求包体
            GOT_ALL,//得到所有数据，解析完成
        };

        HttpContext()
            : state_(EXPECT_REQUEST_LINE)
        {
        }

        //解析http请求报文
        bool parseRequest(Buffer* buf, Timestamp receiveTime);

        bool gotAll() const { return state_ == GOT_ALL; }

        void reset()
        {
            state_ = EXPECT_REQUEST_LINE;
            HttpRequest dummy;
            request_.swap(dummy);
        }

        const HttpRequest& request() const { return request_; }
        HttpRequest& request() { return request_; }

    private:
        //解析请求行
        bool processRequestLine(const char* begin, const char* end);

        HttpRequestParseState state_;//请求解析状态
        HttpRequest request_;//http请求
    };
}


#endif

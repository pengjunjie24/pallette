/*************************************************************************
  > File Name: HttpResponse.h
  > Author: pengjunjie
  > Mail: 1002398145@qq.com
  > Created Time: 2019年06月16日 星期日 11时53分31秒
 ************************************************************************/

#ifndef PALLETTE_HTTP_RESPONSE_H
#define PALLETTE_HTTP_RESPONSE_H

#include <string>
#include <map>

namespace pallette
{
    class Buffer;

    class HttpResponse
    {
    public:
        enum HttpStatusCode
        {
            KUNKNOWN,
            K200_OK = 200,
            K301_MOVED_PERMANENTLY = 301,
            K400_BAD_REQUEST = 400,
            K404_NOT_FOUND = 404,
        };

        explicit HttpResponse(bool close)
            : statusCode_(KUNKNOWN)
            , closeConnection_(close)
        {
        }

        void setStatusCode(HttpStatusCode code) { statusCode_ = code; }
        void setStatusMessage(const std::string& message) { statusMessage_ = message; }

        void setCloseConnection(bool on) { closeConnection_ = on; }
        bool closeConnection() const { return closeConnection_; }

        void setContentType(const std::string& contentType) { addHeader("Content-Type", contentType); }
        void addHeader(const std::string& key, const std::string& value) { headers_[key] = value; }
        void setBody(const std::string& body) { body_ = body; }

        void appendToBuffer(Buffer* output) const;

    private:
        std::map<std::string, std::string> headers_;//响应头部
        HttpStatusCode statusCode_;//状态码
        // FIXME: add http version
        std::string statusMessage_;//服务端软件信息
        bool closeConnection_;//连接方式
        std::string body_;//响应包体
    };
}

#endif

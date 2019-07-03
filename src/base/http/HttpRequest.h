/*************************************************************************
  > File Name: HttpRequest.h
  > Author: pengjunjie
  > Mail: 1002398145@qq.com
  > Created Time: 2019年06月16日 星期日 11时04分25秒
 ************************************************************************/

#ifndef PALLETTE_HTTP_REQUEST_H
#define PALLETTE_HTTP_REQUEST_H

#include "../Timestamp.h"

#include <string>
#include <map>

#include <assert.h>

namespace pallette
{
    //http请求报文内容
    class HttpRequest
    {
    public:
        //请求方法
        enum Method
        {
            INVALID,
            GET,
            POST,
            HEAD,
            PUT,
            DELETE
        };

        enum Version
        {
            UNKONOWN,
            HTTP10,
            HTTP11
        };

        HttpRequest()
            : method_(INVALID),
            version_(UNKONOWN)
        {
        }

        void setVersion(Version v) { version_ = v; }
        Version getVersion() const{ return version_; }

        bool setMethod(const char* start, const char* end)
        {
            assert(method_ == INVALID);
            std::string m(start, end);
            if (m == "GET")
            {
                method_ = GET;
            }
            else if (m == "POST")
            {
                method_ = POST;
            }
            else if (m == "HEAD")
            {
                method_ = HEAD;
            }
            else if (m == "PUT")
            {
                method_ = PUT;
            }
            else if (m == "DELETE")
            {
                method_ = DELETE;
            }
            else
            {
                method_ = INVALID;
            }
            return method_ != INVALID;
        }
        Method method() const { return method_; }

        const char* methodString() const
        {
            const char* result = "UNKNOWN";
            switch (method_)
            {
            case GET:
                result = "GET";
                break;
            case POST:
                result = "POST";
                break;
            case HEAD:
                result = "HEAD";
                break;
            case PUT:
                result = "PUT";
                break;
            case DELETE:
                result = "DELETE";
                break;
            default:
                break;
            }
            return result;
        }

        void setPath(const char* start, const char* end) { path_.assign(start, end); }
        const std::string& path() const { return path_; }

        void setQuery(const char* start, const char* end) { query_.assign(start, end); }
        const std::string& query() const { return query_; }

        void setReceiveTime(Timestamp t) { receiveTime_ = t; }
        Timestamp receiveTime() const { return receiveTime_; }

        //添加请求头部数据
        void addHeader(const char* start, const char* colon, const char* end)
        {
            std::string field(start, colon);
            ++colon;
            while (colon < end && isspace(*colon))
            {
                ++colon;
            }
            std::string value(colon, end);
            while (!value.empty() && isspace(value[value.size() - 1]))
            {
                value.resize(value.size() - 1);
            }
            headers_[field] = value;
        }

        std::string getHeader(const std::string& field) const
        {
            std::string result = "";
            std::map<std::string, std::string>::const_iterator it = headers_.find(field);
            if (it != headers_.end())
            {
                result = it->second;
            }
            return result;
        }

        const std::map<std::string, std::string>& headers() const { return headers_; }

        void swap(HttpRequest& that)
        {
            std::swap(method_, that.method_);
            std::swap(version_, that.version_);
            path_.swap(that.path_);
            query_.swap(that.query_);
            receiveTime_.swap(that.receiveTime_);
            headers_.swap(that.headers_);
        }

    private:
        Method method_;//请求方法
        Version version_;//协议版本1.0/1.1
        std::string path_;//请求路径url
        std::string query_;//请求数据
        pallette::Timestamp receiveTime_;//请求时间
        std::map<std::string, std::string> headers_;//头部字段列表
    };
}

#endif

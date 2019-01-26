/*************************************************************************
  > File Name: LogStream.h
  > Author: pengjunjie
  > Mail: 1002398145@qq.com
  > Created Time: 2018年12月23日 星期日 10时21分46秒
 ************************************************************************/

#ifndef PALLETTE_LOGSTREAM_H
#define PALLETTE_LOGSTREAM_H

#include "noncopyable.h"

#include <string>

namespace pallette
{
    namespace detail
    {
        const int kSmallBuffer = 4000;
        const int kLargeBuffer = 4000 * 1000;

        //日志缓冲模板类
        template <int SIZE>
        class LogBuffer
        {
        public:
            //剩余可写长度
            int avail() const
            {
                return kBufferLength_ - buffer_.length();
            }
            //向buffer尾部添加字符串
            void append(const char* buf, int len)
            {
                if (avail() > len)
                {
                    buffer_.append(buf, len);
                }
                else
                {
                    buffer_.append(buf, avail());
                }
            }
            const std::string& getString() const
            {
                return buffer_;
            }
            //字符串置空
            void resetBuffer()
            {
                buffer_ = "";
            }

        private:
            std::string buffer_;
            const int kBufferLength_ = SIZE;
        };
    }

    //日志流类，主要是重载<<，将日志内容放入到Buffer中
    class LogStream : noncopyable
    {
    public:
        typedef detail::LogBuffer<detail::kSmallBuffer> Buffer;
        typedef LogStream self;

        self& operator<<(bool);
        self& operator<<(short);
        self& operator<<(unsigned short);
        self& operator<<(int);
        self& operator<<(unsigned int);
        self& operator<<(long);
        self& operator<<(unsigned long);
        self& operator<<(long long);
        self& operator<<(unsigned long long);
        self& operator<<(float);
        self& operator<<(double);
        self& operator<<(char);
        self& operator<<(const char*);
        self& operator<<(const unsigned char*);
        self& operator<< (const void* p);
        self& operator<<(const std::string&);

        const Buffer& getBuffer() const
        {
            return buffer_;
        }
    private:
        template<typename T>
        void foramtInteger(T v);

        Buffer buffer_;
        const int kBufferLength_ = detail::kSmallBuffer;
        static const int kMaxNumberSize = 32;//整型转化为字符串的最大长度
    };
}

#endif

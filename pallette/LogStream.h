/*************************************************************************
  > File Name: LogStream.h
  > Author: pengjunjie
  > Mail: 1002398145@qq.com
  > Created Time: 2018年12月23日 星期日 10时21分46秒
 ************************************************************************/

#ifndef PALLETTE_LOGSTREAM_H
#define PALLETTE_LOGSTREAM_H

#include <pallette/noncopyable.h>

#include <string>
#include <string.h>

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
            LogBuffer()
                :cur_(data_)
            {
            }

            //向buffer尾部添加字符串
            void append(const char* buf, size_t len)
            {
                if (static_cast<size_t>(avail()) > len)
                {
                    memcpy(cur_, buf, len);
                    cur_ += len;
                }
                else
                {
                    memcpy(cur_, buf, avail());
                    cur_ = const_cast<char*>(end());
                }
            }

            int avail() const { return static_cast<int>(end() - cur_); }
            const char* data() const { return data_; }
            int length() const { return static_cast<int>(cur_ - data_); }

            char* current() { return cur_; }
            void add(size_t len) { cur_ += len; }

            void reset() { cur_ = data_; }
            void bzeros() { bzero(data_, sizeof(data_)); }

            std::string toString() const { return std::string(data_, length()); }

        private:
            const char* end() const { return data_ + sizeof(data_); }

            char data_[SIZE];
            char* cur_;
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
        self& operator<<(const Buffer& v);

        void append(const char* data, int len) { buffer_.append(data, len); }
        const Buffer& getBuffer() const { return buffer_; }
        void resetBuffer() { buffer_.reset(); }

    private:
        template<typename T>
        void foramtInteger(T v);

        Buffer buffer_;
        const int kBufferLength_ = detail::kSmallBuffer;
        static const int kMaxNumberSize = 32;//整型转化为字符串的最大长度
    };
}

#endif

/*************************************************************************
  > File Name: LogStream.cpp
  > Author: pengjunjie
  > Mail: 1002398145@qq.com
  > Created Time: 2018年12月23日 星期日 10时21分54秒
 ************************************************************************/

#include "LogStream.h"

#include <iostream>
#include <stdio.h>
#include <string.h>
#include <stdint.h>

namespace
{
    template<typename T>
    char* convert(char* buf, T value)
    {
        const char digits[] = "0123456789";
        char *ptr = buf;
        ptr += 31;
        bool signFlag = false;
        if (value < 0)
        {
            value = -value;
            signFlag = true;
        }

        do
        {
            *--ptr = digits[value % 10];
        } while (value /= 10);

        if (signFlag)
        {
            *--ptr = '-';
        }
        return ptr;
    }

    char* convertHex(char* buf, uintptr_t value)
    {
        const char digits[] = "0123456789ABCDEF";
        char *ptr = buf;
        ptr += 31;
        do
        {
            *--ptr = digits[value % 16];
        } while (value /= 16);
        return ptr;
    }
}

namespace pallette
{
    template<typename T>
    void LogStream::foramtInteger(T value)
    {
        if (buffer_.avail() > kMaxNumberSize)
        {
            char buf[kMaxNumberSize] = { 0 };
            char *ptr = convert(buf, value);
            buffer_.append(ptr, strlen(ptr));
        }
    }

    LogStream& LogStream::operator << (bool v)
    {
        buffer_.append((v ? "0" : "1"), 1);
        return *this;
    }

    LogStream& LogStream::operator<<(short v)
    {
        foramtInteger(v);
        return *this;
    }
    LogStream& LogStream::operator<<(unsigned short v)
    {
        foramtInteger(v);
        return *this;
    }
    LogStream& LogStream::operator<<(int v)
    {
        foramtInteger(v);
        return *this;
    }
    LogStream& LogStream::operator<<(unsigned int v)
    {
        foramtInteger(v);
        return *this;
    }
    LogStream& LogStream::operator<<(long v)
    {
        foramtInteger(v);
        return *this;
    }
    LogStream& LogStream::operator<<(unsigned long v)
    {
        foramtInteger(v);
        return *this;
    }
    LogStream& LogStream::operator<<(long long v)
    {
        foramtInteger(v);
        return *this;
    }
    LogStream& LogStream::operator<<(unsigned long long v)
    {
        foramtInteger(v);
        return *this;
    }

    LogStream& LogStream::operator<<(float v)
    {
        *this << static_cast<double>(v);
        return *this;
    }
    LogStream& LogStream::operator<<(double v)
    {
        if (buffer_.avail() >= kMaxNumberSize)
        {
            char buf[32] = { 0 };
            //%.15g 最多打印15个数，超出部分截断
            int len = snprintf(buf, sizeof(buf), "%.15g", v);
            buffer_.append(buf, len);
        }
        return *this;
    }
    LogStream& LogStream::operator<<(char v)
    {
        buffer_.append(&v, 1);
        return *this;
    }
    LogStream& LogStream::operator<<(const char* str)
    {
        if (str)
        {
            buffer_.append(str, strlen(str));
        }
        else
        {
            buffer_.append("(NULL)", 6);
        }
        return *this;
    }
    LogStream& LogStream::operator<<(const unsigned char* str)
    {
        *this << (reinterpret_cast<const char*>(str));
        return *this;
    }
    LogStream& LogStream::operator<< (const void* p)
    {
        //提高程序可移植性，这种类型是为了void*提供的
        uintptr_t v = reinterpret_cast<uintptr_t>(p);
        if (buffer_.avail() > kMaxNumberSize)
        {
            buffer_.append("0x", strlen("0x"));

            char buf[kMaxNumberSize] = { 0 };
            char *ptr = convertHex(buf, v);
            buffer_.append(ptr, strlen(ptr));
        }
        return *this;
    }
    LogStream& LogStream::operator<<(const std::string& v)
    {
        buffer_.append(v.c_str(), static_cast<int>(v.length()));
        return *this;
    }
}
/*************************************************************************
  > File Name: NonCopyable.h
  > Author: pengjunjie
  > Mail: 1002398145@qq.com
  > Created Time: 2018年12月23日 星期日 10时33分25秒
 ************************************************************************/

#ifndef PALLETTE_NONCOPYABLE_H
#define PALLETTE_NONCOPYABLE_H

namespace pallette
{
    class noncopyable
    {
    protected:
        noncopyable() = default;
        ~noncopyable() = default;

    private:
        noncopyable(const noncopyable&) = delete;
        noncopyable& operator=(const noncopyable&) = delete;
    };
}

#endif

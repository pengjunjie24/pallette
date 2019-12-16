/*************************************************************************
> File Name: Endian.h
> Author: pengjunjie
> Mail: 1002398145@qq.com
> Created Time: 2019年01月29日 星期二 11时43分19秒
************************************************************************/

#ifndef PALLETTE_ENDIAN_H
#define PALLETTE_ENDIAN_H

#include <stdint.h>
#include <endian.h>

//封装大小端转换操作
namespace pallette
{
    namespace sockets
    {
        inline uint64_t hostToNetwork64(uint64_t host64)
        {
            return htobe64(host64);
        }

        inline uint32_t hostToNetwork32(uint32_t host32)
        {
            return htobe32(host32);
        }

        inline uint16_t hostToNetwork16(uint16_t host16)
        {
            return htobe16(host16);
        }

        inline uint64_t networkToHost64(uint64_t net64)
        {
            return be64toh(net64);
        }

        inline uint32_t networkToHost32(uint32_t net32)
        {
            return be32toh(net32);
        }

        inline uint16_t networkToHost16(uint16_t net16)
        {
            return be16toh(net16);
        }
    }
}

#endif
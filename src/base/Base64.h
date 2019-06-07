/*************************************************************************
  > File Name: Base64.h
  > Author: pengjunjie
  > Mail: 1002398145@qq.com
  > Created Time: 2019年06月07日 星期五 11时09分13秒
 ************************************************************************/

#ifndef PALLETTE_BASE64_H
#define PALLETTE_BASE64_H

#include <string>

namespace pallette
{
    class Base64
    {
    public:

        //函数说明：将输入数据进行base64编码
        //参数说明：[in]pIn 需要进行编码的数据
        //[in]uInLen 输入参数的字节数
        //[out]strOut 输出的进行base64编码之后的字符串
        //返回值  ：true处理成功,false失败
        static bool base64Encode(const unsigned char *pIn, unsigned long uInLen, std::string& strOut);

        //函数说明：将输入数据进行base64解码
        //参数说明：[in]strIn 需要进行解码的数据
        //[out]strOut 输出的进行base64解码之后的字符串
        //返回值: true处理成功,false失败
        static bool base64Decode(const std::string& strIn, std::string& strOut);

    private:

        //函数说明：将输入数据进行base64编码
        //参数说明：[in]pIn需要进行编码的数据
        //[in]uInLen输入参数的字节数
        //[out]pOut输出的进行base64编码之后的字符串
        //[out]uOutLen输出的进行base64编码之后的字符串长度
        //返回值：true处理成功,false失败
        static bool base64Encode(const unsigned char *pIn, unsigned long uInLen, unsigned char *pOut, unsigned long *uOutLen);

        //函数说明：将输入数据进行base64解码
        //参数说明：[in]strIn 需要进行解码的数据
        //[out]pOut 输出解码之后的节数数据
        //[out]uOutLen 输出的解码之后的字节数长度
        //返回值: true处理成功,false失败
        static bool base64Decode(const std::string& strIn, unsigned char *pOut, unsigned long *uOutLen);
    };
}

#endif

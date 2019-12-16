/*************************************************************************
  > File Name: Strings.h
  > Author: pengjunjie
  > Mail: 1002398145@qq.com
  > Created Time: 2019年07月05日 星期五 14时22分58秒
 ************************************************************************/

#ifndef PALLETTE_UTILS_STRINGS_H
#define PALLETTE_UTILS_STRINGS_H

#include <vector>
#include <string>

//字符串处理
namespace pallette
{
    namespace utils
    {
        //用delim指定的正则表达式将字符串in分割，返回分割后的字符串数组splitResult
        //delim: 分割字符串的正则表达式
        void regexSplit(std::vector<std::string>& splitResult,
            const std::string& in, const std::string& delim);

        //格式化字符串输出，类似于sprintf函数，将第一个参数由char*变为string
        void stringFormat(std::string& formatText, const char* format, ...);
    }
}

#endif
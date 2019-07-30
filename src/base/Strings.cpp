/*************************************************************************
  > File Name: Strings.cpp
  > Author: pengjunjie
  > Mail: 1002398145@qq.com
  > Created Time: 2019年07月05日 星期五 14时23分06秒
 ************************************************************************/

#include "Strings.h"

#include <regex>

using namespace pallette;

std::vector<std::string> utils::regexSplit(const std::string& in,
    const std::string& delim)
{
    std::regex re{ delim };
    // 调用 std::vector::vector (InputIterator first, InputIterator last,const allocator_type& alloc = allocator_type())
    // 构造函数,完成字符串分割
    return std::vector<std::string>
    {
        std::sregex_token_iterator(in.begin(), in.end(), re, -1),
            std::sregex_token_iterator()
    };
}
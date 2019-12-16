/*************************************************************************
  > File Name: Strings.cpp
  > Author: pengjunjie
  > Mail: 1002398145@qq.com
  > Created Time: 2019年07月05日 星期五 14时23分06秒
 ************************************************************************/

#include <pallette/Strings.h>

#include <regex>
#include <stdarg.h>

using namespace pallette;

void utils::regexSplit(std::vector<std::string>& splitResult,
    const std::string& in, const std::string& delim)
{
    std::regex re{ delim };
    // 调用 std::vector::vector (InputIterator first, InputIterator last,const allocator_type& alloc = allocator_type())
    // 构造函数,完成字符串分割
    splitResult = std::vector<std::string>
    {
        std::sregex_token_iterator(in.begin(), in.end(), re, -1),
            std::sregex_token_iterator()
    };

    if (splitResult.size() == 1 && splitResult[0] == in)
    {
        splitResult.clear();
    }
    if (splitResult.size() >= 1 && splitResult.front() == "")
    {
        splitResult.erase(splitResult.begin());
    }
}

void utils::stringFormat(std::string& formatText, const char* format, ...)
{
    formatText = "";
    size_t stringSize = 100;
    va_list args;

    while (true)
    {
        formatText.resize(stringSize);
        va_start(args, format);
        int n = vsnprintf((char *)formatText.c_str(), stringSize, format, args);
        va_end(args);

        if (n > -1 && n < (int)stringSize)
        {
            formatText.resize(n);
            break;
        }
        stringSize = (n > -1) ? (n + 1) : (stringSize * 2);
    }
}

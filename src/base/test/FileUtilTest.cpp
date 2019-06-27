/*************************************************************************
	> File Name: FileUtilTest.cpp
	> Author: pengjunjie
	> Mail: 1002398145@qq.com
	> Created Time: 2019年03月19日 星期二 18时19分21秒
 ************************************************************************/

#include "../FileUtil.h"

#include <stdio.h>
#include <inttypes.h>
#include <errno.h>
#include <string.h>

using namespace pallette;

//将testFileUtil文件中"  "变为" 0 "，最终并去掉" "，输出到文件testFileUtil2中
void test()
{
    std::string contents;
    int errnum = file_util::readFile("testFileUtil", 1024 * 1024, &contents);
    if (errnum != 0)
    {
        printf("err: %s", strerror(errnum));
        return;
    }

    size_t place = 0;
    while ((place = contents.find("  ")) != std::string::npos)
    {
        contents.insert(place + 1, 1, '0');
    }

    while (((place = contents.find(" ")) != std::string::npos))
    {
        contents.erase(place, 1);
    }

    file_util::AppendFile file2("testFileUtil2");
    file2.append(contents.c_str(), contents.length());
    file2.flush();
}

int main()
{
    test();
}

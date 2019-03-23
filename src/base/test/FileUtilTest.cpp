/*************************************************************************
	> File Name: FileUtilTest.cpp
	> Author: pengjunjie
	> Mail: 1002398145@qq.com
	> Created Time: 2019年03月19日 星期二 18时19分21秒
 ************************************************************************/

#include "../FileUtil.h"

#include <stdio.h>
#include <inttypes.h>

int main(int argc, char* argv[])
{
    if (argc != 2)
    {
        printf("Usage: %s file\n", argv[0]);
        return 1;
    }
    std::string filename = argv[1];
    std::string readContent = "";
    int64_t size = 0;
    int err = pallette::file_util::readFile("../Makefile", 10240, &readContent, &size);
    printf("err = %d, Makefile size = %" PRIu64 "\n", err, size);
    printf("%s\n", readContent.c_str());

    return 0;
}
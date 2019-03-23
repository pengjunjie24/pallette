/*************************************************************************
	> File Name: main.cpp
	> Author: pengjunjie
	> Mail: 1002398145@qq.com
	> Created Time: 2019年03月15日 星期五 17时35分41秒
 ************************************************************************/

#include "Common.h"
#include <stdio.h>
#include <assert.h>

int main(int argc, char* argv[])
{
    if (argc != 2)
    {
        printf("Command line argument error");
        return -1;
    }

    Options options;
    if (parseConfigurationFile(argv[1], &options))
    {
        if (options.transmit)
        {
            transmit(options);
        }
        else if (options.receive)
        {
            receive(options);
        }
        else
        {
            assert(0);
        }
    }
}
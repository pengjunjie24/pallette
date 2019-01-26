/*************************************************************************
    > File Name: logstream_test.cpp
    > Author: pengjunjie
    > Mail: 1002398145@qq.com
    > Created Time: 2018年06月16日 星期六 10时05分27秒
 ************************************************************************/

#include "../LogStream.h"

#include <iostream>
using namespace std;

int main()
{
    pallette::LogStream ss;

    ss << -33.1234;
    cout << ss.getBuffer().getString() << endl;

    ss << 12345;
    cout << ss.getBuffer().getString() << endl;

    ss << "max = " << 325.987;
    cout << ss.getBuffer().getString() << endl;

    ss << "float = " << 8783432432.2345435;
    cout << ss.getBuffer().getString() << endl;
    return 0;
}
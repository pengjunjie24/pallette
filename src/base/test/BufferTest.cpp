/*************************************************************************
  > File Name: BufferTest.cpp
  > Author: pengjunjie
  > Mail: 1002398145@qq.com
  > Created Time: 2019年06月07日 星期五 15时59分39秒
 ************************************************************************/

#include "../Buffer.h"
#include <iostream>

using namespace pallette;

void test()
{
    char testChar[] = "ofae0rrfnndspdnvsa-12jri";
    Buffer testBuffer;
    testBuffer.append(testChar, sizeof(testChar));

    std::string searchString = "n";
    const char* findEnd = testBuffer.findStr(searchString);
    if (findEnd != NULL)
    {
        std::cout << "place: " << findEnd - testBuffer.peek() << std::endl;
    }
    else
    {
        std::cout << "not find" << std::endl;
    }

    unsigned char testChar2[] = { 0x7e, 0x30, 0x31, 0x32, 0x1c, 0x77, 0x76, 0x22 };
    Buffer testBuffer2;
    testBuffer2.append(testChar2, sizeof(testChar2));

    char searchChar[] = { 0x7e };
    std::string searchString2(searchChar, sizeof(searchChar));

    const char* findEnd2 = testBuffer2.findStr(searchString2);
    if (findEnd2 != NULL)
    {
        std::cout << "findEnd2 place: " << findEnd2 - testBuffer2.peek() << std::endl;
    }
    else
    {
        std::cout << "not find" << std::endl;
    }
}

int main()
{
    test();
    return 0;
}
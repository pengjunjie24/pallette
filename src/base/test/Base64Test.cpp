/*************************************************************************
  > File Name: Base64Test.cpp
  > Author: pengjunjie
  > Mail: 1002398145@qq.com
  > Created Time: 2019年06月07日 星期五 12时37分28秒
 ************************************************************************/

#include "../Base64.h"
#include "../../base/FileUtil.h"

#include <iostream>
#include <string.h>

using namespace pallette;
using namespace std;

void test1()
{
    unsigned long len = 10;

    unsigned char pIn[100];
    unsigned char pOut[100];

    unsigned long pInLen = strlen("你好");
    memcpy(pIn, "你好", pInLen);

    string strout;
    cout << (char*)pIn << endl;
    if (Base64::base64Encode(pIn, pInLen, strout))
        cout << strout << endl;
    else
        cout << "加密失败" << endl;

    string stroIn = strout;
    string decondeStr = "";
    cout << stroIn << endl;
    memset(pOut, 0, sizeof(pOut));

    if (Base64::base64Decode(stroIn, decondeStr))
    {
        cout << (char *)pOut << endl;
        cout << "len=" << decondeStr.length() << endl;
    }
    else
        cout << "解密失败" << endl;

    cout << len << endl;
}

void test2()
{
    std::string jpgStr = "";
    file_util::readFile("./test1.jpg", 1024 * 1024, &jpgStr);

    {
        string strout;
        if (Base64::base64Encode(reinterpret_cast<const unsigned char*>(jpgStr.c_str()),
            jpgStr.length(), strout))
        {
            file_util::AppendFile appendFile1("./base64test1.base64");
            appendFile1.append(strout.c_str(), strout.length());
        }
        else
        {
            cout << "加密失败" << endl;
            return;
        }
    }

    std::string encondeStr = "";
    file_util::readFile("./base64test1.base64", 1024 * 1024, &encondeStr);

    {
        string strout;
        if (Base64::base64Decode(encondeStr, strout))
        {
            file_util::AppendFile appendFile1("./test2.jpg");
            appendFile1.append(strout.c_str(), strout.length());
        }
        else
        {
            cout << "加密失败" << endl;
            return;
        }
    }

}

int main()
{
    test2();
    return 0;
}

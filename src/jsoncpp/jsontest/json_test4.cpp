/*************************************************************************
> File Name: json_test.cpp
> Author: pengjunjie
> Mail: 1002398145@qq.com 
> Created Time: 2018年12月01日 星期六 17时53分16秒
************************************************************************/

#include "../dist/json/json.h"
#include <iostream>
using namespace std;

int main()
{

    Json::Value root;
    Json::Value arrayObj;
    Json::Value item;


    item["uid"] = Json::Value("chechenluoyang@163.com");
    item["fileName"] = Json::Value("梅西.txt");
    item["time"] = Json::Value("2017.07.28 10:55:22");
    item["type"] = Json::Value("Libcurl HTTP POST JSON");
    item["authList"] = Json::Value("test");
    arrayObj.append(item);


    root = arrayObj;
    std::string jsonout = root.toStyledString();

    cout << jsonout << endl;
    return 0;
}

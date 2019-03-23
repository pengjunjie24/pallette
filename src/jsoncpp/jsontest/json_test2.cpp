/*************************************************************************
  > File Name: json_test2.cpp
  > Author: pengjunjie
  > Mail: 1002398145@qq.com
  > Created Time: 2018年12月02日 星期日 23时49分59秒
 ************************************************************************/

#include "../dist/json/json.h"

#include <iostream>
#include <stdio.h>

using namespace std;

void test01()
{
    cout << "----------test01---------" << endl;
    //jsoncpp中使用JSON::Value类型来包含Json对象
    Json::Value a, b, c, d, e;
    a = 12;     //int
    b = "hello";//string
    c = false;  //boolean
    d = true;   //boolean
    //eΪnull
    cout << "First" << endl;
    cout << a.asInt() << endl;
    cout << b.asString() << endl;
    cout << c.asString() << endl;
    cout << d.asString() << endl;
    cout << e.asString() << endl;

    //也可以使用复制构造函数来初始化生成JSON对象
    a = Json::Value(13);
    b = Json::Value("hello world");
    c = Json::Value(false);
    d = Json::Value(true);
    e = Json::Value();
    cout << "Second" << endl;
    cout << a.asInt() << endl;
    cout << b.asString() << endl;
    cout << c.asString() << endl;
    cout << d.asString() << endl;
    cout << e.asString() << endl;

    cout << "\n" << endl;
}


//从JSON字符流中读取json对象
void test02()
{
    cout << "----------test02---------" << endl;
    string message = "{ \"data\" : { \"username\" : \"对象\" }, \"type\" : 6 }";
    Json::Reader reader;
    Json::Value value;
    //如果message符合Json对象语法，则取出该对象存在value中
    if (reader.parse(message.c_str(), value))
    {
        Json::Value temp = value["data"]; //取出value对象中的data对象，对象嵌套的一种方式
        cout << "username:" << temp["username"].asString() << endl;
        cout << "type:" << value["type"].asInt() << endl;
        cout << "id:" << value["id"].asString() << endl; //取出null
    }

    cout << "\n" << endl;
}

//将json对象转为字符流
void test03()
{
    cout << "----------test03---------" << endl;
    Json::Value value;
    //使用赋值的方式产生JSON对象
    value["name"] = "kangkang";
    Json::Value data;
    data["id"] = "20140801";    //账号
    data["pwd"] = "123456";     //密码
    value["data"] = data;       //对象嵌套

    //使用toStyledString()函数直接将value对象转为JSON字符流
    cout << "toStyledString()" << endl;
    cout << value.toStyledString() << endl;

    //使用Json::FastWriter类型
    Json::FastWriter fw;
    cout << "Json::FastWriter()" << endl;
    cout << fw.write(value) << endl;

    cout << "\n" << endl;
}

int main()
{
    test01();
    test02();
    test03();
}
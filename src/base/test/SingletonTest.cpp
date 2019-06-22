/*************************************************************************
  > File Name: singleton_test.cpp
  > Author: pengjunjie
  > Mail: 1002398145@qq.com
  > Created Time: 2018年07月27日 星期五 17时11分06秒
 ************************************************************************/

#include "../singleton.hpp"

#include <string>
#include <iostream>

using namespace pallette;

class Test : public Singleton<Test>
{
public:
    const std::string& name() const { return name_; }
    void setName(const std::string& n) { name_ = n; }

private:
    friend Singleton<Test>;//因为基类在instance()中new T()时会调用到Test的构造函数
    Test() = default;

    std::string name_;
};

int main()
{
    Test::instance()->setName("aaaa");
    std::cout << Test::instance()->name() << std::endl;
}

/*************************************************************************
  > File Name: singleton_test.cpp
  > Author: pengjunjie
  > Mail: 1002398145@qq.com
  > Created Time: 2018��07��27�� ������ 17ʱ11��06��
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
    friend Singleton<Test>;//��Ϊ������instance()��new T()ʱ����õ�Test�Ĺ��캯��
    Test() = default;

    std::string name_;
};

int main()
{
    Test::instance()->setName("aaaa");
    std::cout << Test::instance()->name() << std::endl;
}

/*************************************************************************
  > File Name: SignalSlotTest.cpp
  > Author: pengjunjie
  > Mail: 1002398145@qq.com
  > Created Time: 2019年05月19日 星期日 10时43分27秒
 ************************************************************************/

#include "../SignalSlot.hpp"

#include <iostream>

using namespace pallette;

void hello()
{
    std::cout << "hello" << std::endl;
}

void print(int i)
{
    std::cout << "print " << i << std::endl;
}

void printA(int a, int b)
{
    std::cout << "printA " << a << "  " << b << std::endl;
}

void test()
{
    {
        Signal<void(void)> sig;
        Slot slot1 = sig.connect(&hello);
        sig.call();
    }
    Signal<void(int)> sig1;
    Slot slot1 = sig1.connect(&print);
    Slot slot2 = sig1.connect(std::bind(&print, std::placeholders::_1));
    std::function<void(int)> func1(std::bind(&print, std::placeholders::_1));
    Slot slot3 = sig1.connect(std::move(func1));
    {
        Slot slot4 = sig1.connect(std::bind(&print, 666));//???std::bind(&print, 666)的返回是void()
        sig1.call(4);
    }

    Slot slot5 = sig1.connect(std::bind(&printA, 777, std::placeholders::_1));
    sig1.call(4);
}

int main()
{
    test();
}
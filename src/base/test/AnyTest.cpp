
#include <iostream>
#include <string>
#include <vector>

#include "../any.hpp"

using namespace pallette;
void testany1()
{
    any anyA1(123);
    int a2 = any_cast<int>(anyA1);
    int* pA2 = any_cast<int>(&anyA1);
    std::cout << "a2 = " << a2 << " *pA2=" << *pA2 << std::endl;

    any anyB1(12.35);
    double b2 = any_cast<double>(anyB1);
    std::cout << "b2= " << b2 << std::endl;

    any anyStr(std::string("abc"));
    std::string msg = any_cast<std::string>(anyStr);
    std::cout << "msg= " << msg << std::endl;

    std::vector<float> values, newValues;
    for (int idx = 0; idx < 10; idx++)
    {
        values.push_back(0.5 + idx + 10);
    }
    any anyVector(values);
    newValues = any_cast<std::vector<float>>(anyVector);
    auto iter = newValues.begin();
    while (newValues.end() != iter)
    {
        std::cout << *iter << " ";
        ++iter;
    }
    std::cout << std::endl;

    any anyEmpty;
    if (anyEmpty.empty())
    {
        std::cout << "anyEmpty is empty" << std::endl;
    }

    anyEmpty = any(3);
    if (!anyEmpty.empty())
    {
        std::cout << "anyEmpty is not empty" << std::endl;
    }
}


//�ְ���Ϣ�ṹ��
struct SubContract
{
    bool isSubPackage;                            //�Ƿ�ְ�
    uint16_t sumPackage;                           //��Ϣ�ܰ���
    uint16_t currentPackage;                       //��ǰ����
};

//λ����Ϣ�㱨��������Ϣ
struct BaseLocationMessage
{
    uint32_t longitude;                             //γ��
    uint32_t latitude;                              //����
    uint16_t altitude;                              //���θ߶�
    char time[6];                                   //ʱ��
};
void testany2()
{
    SubContract content;
    content.isSubPackage = true;
    content.sumPackage = 12;
    content.currentPackage = 4;

    any testContent(content);

    if (testContent.type() == typeid(SubContract))
    {
        SubContract content2 = any_cast<SubContract>(testContent);
        std::cout << "content2.isSubPackage:" << content2.isSubPackage << std::endl;
        std::cout << "content2.sumPackage:" << content2.sumPackage << std::endl;
        std::cout << "content2.currentPackage" << content2.currentPackage << std::endl;
    }
    if (testContent.type() == typeid(BaseLocationMessage))
    {
        std::cout << "testContent.type() == typeid(BaseLocationMessage)" << std::endl;
    }
}

int main()
{
    testany1();
    //testany2();
}
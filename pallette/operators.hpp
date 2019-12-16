

#ifndef PALLETTE_OPERATOR_HPP
#define PALLETTE_OPERATOR_HPP

namespace pallette
{
    //�Զ������ز��������������<,���бȽ�������Զ�������
    template <class T>
    struct less_than_comparable
    {
        friend bool operator>(const T& x, const T& y)  { return y < x; }
        friend bool operator<=(const T& x, const T& y) { return !static_cast<bool>(y < x); }
        friend bool operator>=(const T& x, const T& y) { return !static_cast<bool>(x < y); }
    };

    //����=, ���бȽ�������Զ�������
    template<class T>
    struct equality_comparable
    {
        friend bool operator!=(const T& x, const T& y)  { return !static_cast<bool>(x == y); }
    };
}

#endif
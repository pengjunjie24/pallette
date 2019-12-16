

#ifndef PALLETTE_OPERATOR_HPP
#define PALLETTE_OPERATOR_HPP

namespace pallette
{
    //自定义重载操作运算符，重载<,下列比较运算符自动被重载
    template <class T>
    struct less_than_comparable
    {
        friend bool operator>(const T& x, const T& y)  { return y < x; }
        friend bool operator<=(const T& x, const T& y) { return !static_cast<bool>(y < x); }
        friend bool operator>=(const T& x, const T& y) { return !static_cast<bool>(x < y); }
    };

    //重载=, 下列比较运算符自动被重载
    template<class T>
    struct equality_comparable
    {
        friend bool operator!=(const T& x, const T& y)  { return !static_cast<bool>(x == y); }
    };
}

#endif
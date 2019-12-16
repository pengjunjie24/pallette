/*************************************************************************
> File Name: WeakCallback.h
> Author: pengjunjie
> Mail: 1002398145@qq.com
> Created Time: 2019年01月30日 星期三 13时50分25秒
************************************************************************/

#ifndef PALLETTE_WEAKCALLBACK_H
#define PALLETTE_WEAKCALLBACK_H

#include <functional>
#include <memory>

namespace pallette
{
    template<typename CLASS>
    class WeakCallback
    {
    public:

        WeakCallback(const std::weak_ptr<CLASS>& object, const std::function<void(CLASS*)>& function)
            : object_(object),
            function_(function)
        {
        }

        void operator()() const
        {
            std::shared_ptr<CLASS> ptr(object_.lock());
            if (ptr)
            {
                function_(ptr.get());
            }
        }

    private:
        std::weak_ptr<CLASS> object_;
        std::function<void(CLASS*)> function_;
    };

    template<typename CLASS>
    WeakCallback<CLASS> makeWeakCallback(const std::shared_ptr<CLASS>& object,
        void (CLASS::*function)())
    {
        return WeakCallback<CLASS>(object, function);
    }

    template<typename CLASS>
    WeakCallback<CLASS> makeWeakCallback(const std::shared_ptr<CLASS>& object,
        void (CLASS::*function)() const)
    {
        return WeakCallback<CLASS>(object, function);
    }
}

#endif
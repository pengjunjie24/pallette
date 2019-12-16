#ifndef PALLETTE_ANY_HPP
#define PALLETTE_ANY_HPP

#include <typeinfo>
#include <iostream>

namespace pallette
{
    class any
    {
    public:
        any() : content(0)
        {
        }
        template<typename ValueType>
        any(const ValueType& value) : content(new holder<ValueType>(value))
        {
        }
        any(const any & other)
            : content(other.content ? other.content->clone() : 0)
        {
        }

        ~any()
        {
            delete content;
        }

    public: // modifiers
        any & swap(any & rhs)
        {
            std::swap(content, rhs.content);
            return *this;
        }
        template<typename ValueType>
        any & operator=(const ValueType & rhs)
        {
            any(rhs).swap(*this);
            return *this;
        }

        any & operator=(any rhs)
        {
            any(rhs).swap(*this);
            return *this;
        }

    public: // queries
        bool empty() const
        {
            return !content;
        }

        void clear()
        {
            any().swap(*this);
        }

        const std::type_info & type() const
        {
            return content ? content->type() : typeid(void);
        }

        class placeholder
        {
        public: // structors
            virtual ~placeholder()
            {
            }
        public: // queries
            virtual const std::type_info & type() const = 0;
            virtual placeholder * clone() const = 0;
        };
    public:
        template<typename ValueType>
        class holder : public placeholder
        {
        public:
            holder(const ValueType& value) : held(value)
            {
            }
            virtual const std::type_info& type() const
            {
                return typeid(ValueType);
            }

            virtual placeholder * clone() const
            {
                return new holder(held);
            }
        public: // representation
            ValueType held;
        private: // intentionally left unimplemented
            holder & operator=(const holder &);
        };
        placeholder* content;
    };

    inline void swap(any & lhs, any & rhs)
    {
        lhs.swap(rhs);
    }

    class  bad_any_cast : public std::bad_cast
    {
    public:
        virtual const char* what() const throw()
        {
            return "bad_any_cast: failed conversion using any_cast";
        }
    };

    template<typename ValueType>
    ValueType * any_cast(any * operand)
    {
        return operand && operand->type() == typeid(ValueType)
            ? &static_cast<any::holder<ValueType> *>(operand->content)->held : 0;
    }

    template<typename ValueType>
    inline const ValueType * any_cast(const any * operand)
    {
        return any_cast<ValueType>(const_cast<any *>(operand));
    }

    template<typename ValueType>
    ValueType any_cast(any& value)
    {
        return static_cast<any::holder<ValueType>*>(value.content)->held;
    }
    template<typename ValueType>
    ValueType any_cast(const any& value)
    {
        return any_cast<ValueType>(const_cast<any &>(value));
    }
}
#endif

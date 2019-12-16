#ifndef PALLETTE_BASE_ERR0R_HPP
#define PALLETTE_BASE_ERR0R_HPP

#include <string>

namespace pallette
{
    template<typename T>
    class BaseError
    {
    public:
        BaseError()
        {
        }

        BaseError(T ret)
            :ret_(ret)
        {
        }

        BaseError(T ret, std::string errMsg)
            :ret_(ret)
            , errorMsg_(errMsg)
        {
        }

        void setErrorCode(T ret){ ret_ = ret; }
        void setErrorMsg(const std::string& msg) { errorMsg_ = msg }

        T getErrorCode() const{ return ret_; }
        const std::string& getErrorMsg() const{ return errorMsg_; }
    private:
        T ret_;
        std::string errorMsg_;
    };

}
#endif
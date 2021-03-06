/*************************************************************************
> File Name: Timestamp.h
> Author: pengjunjie
> Mail: 1002398145@qq.com
> Created Time: 2018年12月23日 星期日 10时51分24秒
************************************************************************/

#ifndef PALLETTE_TIMESTAMP_H
#define PALLETTE_TIMESTAMP_H

#include <pallette/operators.hpp>

#include <algorithm>
#include <string>
#include <stdint.h>

namespace pallette
{
    //时间戳类，返回的是UTC时间，以微妙为分辨率，最好通过值传递
    class Timestamp : public less_than_comparable<Timestamp>
        , public equality_comparable<Timestamp>
    {
    public:
        Timestamp()
            :microSeconds_(0)
        {
        }
        explicit Timestamp(int64_t microSecondsArg)
            :microSeconds_(microSecondsArg)
        {
        }

        bool valid() const { return microSeconds_ > 0; }
        int64_t getMicroSeconds() const { return microSeconds_; }
        void swap(Timestamp& that) { std::swap(microSeconds_, that.microSeconds_); }
        time_t getSeconds() const
        {
            return static_cast<time_t>(microSeconds_ / kMicroSecondsPerSecond);
        }

        std::string toString() const;
        std::string toFormatTedString(bool showMicroSeconds = true) const;
        static Timestamp now();
        static Timestamp invalid() { return Timestamp(); }

        static const int kMicroSecondsPerSecond = 1000 * 1000;
        static const int kSecondsPerHour = 60 * 60;

    private:
        int64_t microSeconds_;
    };

    inline bool operator<(Timestamp lhs, Timestamp rhs)
    {
        return lhs.getMicroSeconds() < rhs.getMicroSeconds();
    }

    inline bool operator==(Timestamp lhs, Timestamp rhs)
    {
        return lhs.getMicroSeconds() == rhs.getMicroSeconds();
    }

    inline double timeDifference(Timestamp high, Timestamp low)
    {
        int64_t diff = high.getMicroSeconds() - low.getMicroSeconds();
        return static_cast<double>(diff) / Timestamp::kMicroSecondsPerSecond;
    }

    inline Timestamp addTime(Timestamp timestamp, double seconds)
    {
        int64_t delta = static_cast<int64_t>(seconds * Timestamp::kMicroSecondsPerSecond);
        return Timestamp(timestamp.getMicroSeconds() + delta);
    }
}

#endif

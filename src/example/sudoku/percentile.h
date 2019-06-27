/*************************************************************************
  > File Name: percentile.h
  > Author: pengjunjie
  > Mail: 1002398145@qq.com
  > Created Time: 2019年06月26日 星期三 15时49分11秒
 ************************************************************************/

#ifndef PERCENTILE_H
#define PERCENTILE_H

#include "../../base/LogStream.h"
#include "../../base/FileUtil.h"
#include "../../base/Buffer.h"

#include <vector>
#include <algorithm>

#include <stdio.h>
#include <assert.h>

//统计延迟数据
//百分位数延迟，如99%的数据延迟要低于500us
//以5us为间隔进行百分位延迟数统计
class Percentile
{
public:
    Percentile(std::vector<int>& latencies, int infly)
    {
        stat << "recv " << latencies.size() << " in-fly " << infly;;
        if (!latencies.empty())
        {
            std::sort(latencies.begin(), latencies.end());
            int min = latencies.front();
            int max = latencies.back();
            int sum = std::accumulate(latencies.begin(), latencies.end(), 0);//延迟总时长
            int mean = sum / static_cast<int>(latencies.size());//平均延时
            int median = getPercentile(latencies, 50);
            int p90 = getPercentile(latencies, 90);
            int p99 = getPercentile(latencies, 99);
            stat << " min: " << min //最短延迟
                 << " max: " << max //最长延迟
                 << " avg: " << mean //平均延迟
                 << " median: " << median //50%的延迟数是多少
                 << " p90: " << p90 //90%的延迟数是多少
                 << " p99: " << p99; //99%上的延迟数是多少
        }
    }

    const pallette::LogStream::Buffer& report() const
    {
        return stat.getBuffer();
    }

    void save(const std::vector<int>& latencies, std::string name) const//保存延迟数据到文件中
    {
        if (latencies.empty())
        {
            return;
        }

        pallette::file_util::AppendFile f(name);//添加到文件
        f.append("# ", 2);
        f.append(stat.getBuffer().getString().c_str(),
            stat.getBuffer().getString().length());
        f.append("\n", 1);

        const int kInterval = 5; // 5 us per bucket,以5us为分界进行延迟数据的统计
        int low = latencies.front() / kInterval * kInterval;//向下对5取整，保证low比latencies中最小的数还要小
        int count = 0;
        int sum = 0;
        const double total = static_cast<double>(latencies.size());
        char buf[64] = { 0 };
#ifndef NDEBUG
        for (size_t i = 0; i < latencies.size(); ++i)
        {
            int n = snprintf(buf, sizeof buf, "# %d\n", latencies[i]);
            f.append(buf, n);
        }
#endif
        // FIXME: improve this O(N) algorithm, maybe use lower_bound().
        for (size_t i = 0; i < latencies.size(); ++i)
        {
            if (latencies[i] < low + kInterval)//不超过low+5中延迟数据的个数
            {
                ++count;
            }
            else
            {
                sum += count;
                int n = snprintf(buf, sizeof buf, "%4d %5d %5.2f\n", low, count, 100 * sum / total);//以5us为间隔计算延迟比
                f.append(buf, n);

                low = latencies[i] / kInterval * kInterval;//取的新的low值
                assert(latencies[i] < low + kInterval);
                count = 1;
            }
        }
        sum += count;
        assert(sum == total);
        int n = snprintf(buf, sizeof buf, "%4d %5d %5.1f\n", low, count, 100 * sum / total);
        f.append(buf, n);
    }

private:
    static int getPercentile(const std::vector<int>& latencies, int percent)//取latencies中下标为percent/100的数据进行返回
    {
        assert(latencies.size() > 0);
        size_t idx = 0;
        if (percent > 0)
        {
            idx = (latencies.size() * percent + 99) / 100 - 1;
            assert(idx < latencies.size());
        }
        return latencies[idx];
    }

    pallette::LogStream stat;
};

#endif

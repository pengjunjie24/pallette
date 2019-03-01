/*************************************************************************
  > File Name: FileUtil.cpp
  > Author: pengjunjie
  > Mail: 1002398145@qq.com
  > Created Time: 2018年12月31日 星期一 11时17分04秒
 ************************************************************************/

#include "FileUtil.h"

#include "Logging.h"

#include <stdio.h>
#include <assert.h>

using namespace pallette;

file_util::AppendFile::AppendFile(std::string filename)
    : fp_(::fopen(filename.c_str(), "ae"))
    , writtenBytes_(0)
{
    assert(fp_);
    ::setbuffer(fp_, buffer_, sizeof(buffer_));//XXX:需要学习
}

file_util::AppendFile::~AppendFile()
{
    ::fclose(fp_);
}

void file_util::AppendFile::append(const char* logline, const size_t len)
{
    size_t n = write(logline, len);
    size_t remain = len - n;
    while (remain > 0)
    {
        size_t x = write(logline + n, remain);
        if (x == 0)
        {
            int err = ferror(fp_);
            if (err)
            {
                fprintf(stderr, "AppendFile::append() failed %s\n", strerror_rl(err));
            }
            break;
        }
        n += x;
        remain = len - n;
    }

    writtenBytes_ += len;
}
void file_util::AppendFile::flush()
{
    ::fflush(fp_);
}

size_t file_util::AppendFile::write(const char* logline, size_t len)
{
    return ::fwrite(logline, 1, len, fp_);
}

void file_util::writeFile(const std::string& filename, const std::string& content)
{
    AppendFile file(filename);
    file.append(content.c_str(), content.length());
}
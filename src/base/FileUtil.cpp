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
#include <unistd.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <fcntl.h>

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

void file_util::appendFile(const std::string& filename, const std::string& content)
{
    AppendFile file(filename);
    file.append(content.c_str(), content.length());
}


file_util::ReadSmallFile::ReadSmallFile(std::string filename)
    : fd_(::open(filename.c_str(), O_RDONLY | O_CLOEXEC)),
     err_(0)
{
    buf_[0] = '\0';
    if (fd_ < 0)
    {
        err_ = errno;
    }
}

file_util::ReadSmallFile::~ReadSmallFile()
{
    if (fd_ >= 0)
    {
        ::close(fd_);
    }
}

// return errno
int file_util::ReadSmallFile::readToString(int maxSize, std::string* content,
    int64_t* fileSize, int64_t* modifyTime, int64_t* createTime)
{
    static_assert(sizeof(off_t) == 8, "_FILE_OFFSET_BITS = 64");
    assert(content != NULL);
    int err = err_;
    if (fd_ >= 0)
    {
        content->clear();

        if (fileSize)
        {
            struct stat statbuf;
            if (::fstat(fd_, &statbuf) == 0)
            {
                if (S_ISREG(statbuf.st_mode))
                {
                    if (fileSize)
                    {
                        *fileSize = statbuf.st_size;
                    }
                    content->reserve(static_cast<int>(std::min(static_cast<int64_t>(maxSize), statbuf.st_size)));
                }
                else if (S_ISDIR(statbuf.st_mode))
                {
                    err = EISDIR;
                }
                if (modifyTime)
                {
                    *modifyTime = statbuf.st_mtime;
                }
                if (createTime)
                {
                    *createTime = statbuf.st_ctime;
                }
            }
            else
            {
                err = errno;
            }
        }

        while (content->size() < static_cast<size_t>(maxSize))
        {
            size_t toRead = std::min(static_cast<size_t>(maxSize)-content->size(), sizeof(buf_));
            ssize_t n = ::read(fd_, buf_, toRead);//read读取文件到结尾，返回0
            if (n > 0)
            {
                content->append(buf_, n);
            }
            else
            {
                if (n < 0)
                {
                    err = errno;
                }
                break;
            }
        }
    }
    return err;
}

int file_util::ReadSmallFile::readToBuffer(int* size)
{
    int err = err_;
    if (fd_ >= 0)
    {
        ssize_t n = ::pread(fd_, buf_, sizeof(buf_)-1, 0);
        if (n >= 0)
        {
            if (size)
            {
                *size = static_cast<int>(n);
            }
            buf_[n] = '\0';
        }l
        else
        {
            err = errno;
        }
    }
    return err;
}

int file_util::readFile(std::string filename, int maxSize, std::string* content,
    int64_t* fileSize, int64_t* modifyTime, int64_t* createTime)
{
    file_util::ReadSmallFile file(filename);
    return file.readToString(maxSize, content, fileSize, modifyTime, createTime);
}
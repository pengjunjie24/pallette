/*************************************************************************
  > File Name: FileUtil.h
  > Author: pengjunjie
  > Mail: 1002398145@qq.com
  > Created Time: 2018年12月31日 星期一 11时16分59秒
 ************************************************************************/

#ifndef PALLETTE_FILEUTIL_H
#define PALLETTE_FILEUTIL_H

#include "noncopyable.h"
#include <string>

namespace pallette
{
    namespace file_util
    {
        class AppendFile : noncopyable
        {
        public:
            explicit AppendFile(std::string filename);
            ~AppendFile();
            void append(const char* logline, const size_t len);
            void flush();

            int writtenBytes() const { return writtenBytes_; }

        private:
            size_t write(const char* logline, size_t len);

            FILE* fp_;
            char buffer_[64 * 1024];
            int writtenBytes_;
        };
        void appendFile(const std::string& filename, const std::string& content);

        // read small file < 64KB
        class ReadSmallFile : noncopyable
        {
        public:
            ReadSmallFile(std::string filename);
            ~ReadSmallFile();

            // return errno
            int readToString(int maxSize,
                std::string* content,
                int64_t* fileSize,
                int64_t* modifyTime,
                int64_t* createTime);

            /// Read at maxium kBufferSize into buf_
            // return errno
            int readToBuffer(int* size);
            const char* buffer() const { return buf_; }

            static const int kBufferSize = 64 * 1024;

        private:
            int fd_;
            int err_;
            char buf_[kBufferSize];
        };

        int readFile(std::string filename, int maxSize, std::string* content,
            int64_t* fileSize = NULL, int64_t* modifyTime = NULL, int64_t* createTime = NULL);
    }
}

#endif

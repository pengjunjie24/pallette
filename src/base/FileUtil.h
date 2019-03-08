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
    }
}

#endif

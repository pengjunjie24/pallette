/*************************************************************************
  > File Name: Buffer.h
  > Author: pengjunjie
  > Mail: 1002398145@qq.com 
  > Created Time: 2019年01月29日 星期二 21时00分18秒
 ************************************************************************/

#ifndef PALLETTE_BUFFER_H
#define PALLETTE_BUFFER_H

#include <vector>
#include <string>
#include <stdint.h>

namespace pallette
{
	/// +-------------------+------------------+------------------+
	/// | prependable bytes |  readable bytes  |  writable bytes  |
	/// |                   |     (CONTENT)    |                  |
	/// +-------------------+------------------+------------------+
	/// |                   |                  |                  |
	/// 0      <=      read_index   <=   write_index     <=      size
	class Buffer
	{
	public:
		explicit Buffer(size_t initialSize = kInitalSize);

		void swap(Buffer& rhs);

		size_t readableBytes() const;//可读大小
		size_t writableBytes() const;//可写大小
		size_t prependableBytes() const;//可写入位置

		const char* peek() const;//可读空间起始指针位置

		const char* find(const void* start, int ch);//查询字符
		int findPlace(const void* start, int ch);

		void retrieve(size_t len);//取走len长度数据
		void retrieveUntil(const char* end);//end前数据全部取走
		void retrieveAll();//全部取走
		void retrieveAsChar(char* data, size_t& len);//取出len长度的数据，len是传入传出参数
		std::string retrieveAsString(size_t len);//Buffer中取走len长度数据
		std::string retrieveAllAsString();//Buffer全部取走

		int64_t peekInt64() const;
		int32_t peekInt32() const;
		int16_t peekInt16() const;
		int8_t peekInt8() const;

		int64_t readInt64();//读出这些类型需要进行大小端转换
		int32_t readInt32();
		int16_t readInt16();
		int8_t readInt8();

		void append(const char* data, size_t len);
		void append(const std::string& str);
		void append(const void* data, size_t len);

		void appendInt64(int64_t x);//写入这些类型需要进行大小端转换
		void appendInt32(int32_t x);
		void appendInt16(int16_t x);
		void appendInt8(int8_t x);

		void ensureWritableBytes(size_t len);//确保Buffer可以写入len长度数据

		char* beginWrite();//返回可写入位置的指针
		void hasWritten(size_t len);//写入len长度后，移动可写起始位置
		size_t buffferCapactity() const;

		void prepend(const void* data, size_t len);//向前缀写入数据
		void prependInt64(int64_t x);//向前缀以下类型，需要进行大小端转换
		void prependInt32(int32_t x);
		void prependInt16(int16_t x);
		void prependInt8(int8_t x);

		size_t readFd(int fd, int* savedErrno);//从socket读出到Buffer中

	private:

		const char* begin() const;//返回buffer起始位置
		char* begin();
		void makeSpace(size_t len);//resize或者移动数据，使buffer能容许len长度数据

		std::vector<char>buffer_;//用vector实现连续内存，自动增长的功能
		size_t readIndex_;//可读起始位置，不用char*而用int是为了防止迭代器失效
		size_t writeIndex_;//可写起始位置

		static const size_t kPrePrePend = 8;//默认预留8个字节
		static const size_t kInitalSize = 1024;//初始大小
	};
}

#endif
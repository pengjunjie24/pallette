/*************************************************************************
> File Name: Buffer.cpp
> Author: pengjunjie
> Mail: 1002398145@qq.com
> Created Time: 2019年01月29日 星期二 21时00分24秒
************************************************************************/

#include <pallette/Buffer.h>

#include <pallette/Logging.h>
#include <pallette/Endian.h>

#include <assert.h>
#include <endian.h>
#include <string.h>
#include <sys/uio.h>
#include <errno.h>
#include <algorithm>

using namespace pallette;

Buffer::Buffer(size_t initialSize)
:buffer_(kPrePrePend + initialSize),
readIndex_(kPrePrePend),
writeIndex_(kPrePrePend)
{
    assert(readableBytes() == 0);
    assert(writableBytes() == initialSize);
    assert(prependableBytes() == kPrePrePend);
}

void Buffer::swap(Buffer& rhs)
{
    buffer_.swap(rhs.buffer_);
    std::swap(readIndex_, rhs.readIndex_);
    std::swap(writeIndex_, rhs.writeIndex_);
}

size_t Buffer::readableBytes() const
{
    return writeIndex_ - readIndex_;
}

size_t Buffer::writableBytes() const
{
    return buffer_.size() - writeIndex_;
}

size_t Buffer::prependableBytes() const
{
    return readIndex_;
}

const char* Buffer::peek() const
{
    return begin() + readIndex_;
}

void Buffer::retrieve(size_t len)
{
    assert(len <= readableBytes());
    if (len < readableBytes())
    {
        readIndex_ += len;
    }
    else
    {
        retrieveAll();
    }
}

void Buffer::retrieveUntil(const char* end)
{
    assert(peek() < end);
    assert(end <= beginWrite());
    retrieve(end - peek());
}

void Buffer::retrieveAll()
{
    readIndex_ = kPrePrePend;
    writeIndex_ = kPrePrePend;
}

std::string Buffer::retrieveAsString(size_t len)
{
    assert(len <= readableBytes());
    std::string result(peek(), len);
    retrieve(len);
    return result;
}

std::string Buffer::retrieveAllAsString()
{
    return retrieveAsString(readableBytes());
}

std::string Buffer::BuffertoString()
{
    return std::string(peek(), static_cast<int>(readableBytes()));
}

void Buffer::append(const char* data, size_t len)
{
    ensureWritableBytes(len);
    std::copy(data, data + len, beginWrite());
    hasWritten(len);
}

void Buffer::append(const std::string& str)
{
    append(str.c_str(), str.length());
}

void Buffer::append(const void* data, size_t len)
{
    append(static_cast<const char*>(data), len);
}

void Buffer::appendInt64(int64_t x)
{
    int64_t be64 = sockets::hostToNetwork64(x);
    append(&be64, sizeof(be64));
}

void Buffer::appendInt32(int32_t x)
{
    int32_t be32 = sockets::hostToNetwork32(x);
    append(&be32, sizeof(be32));
}

void Buffer::appendInt16(int16_t x)
{
    int16_t be16 = sockets::hostToNetwork16(x);
    append(&be16, sizeof(be16));
}

void Buffer::appendInt8(int8_t x)
{
    append(&x, sizeof(x));
}

int64_t Buffer::peekInt64() const
{
    int64_t be64 = 0;
    ::memcpy(&be64, peek(), sizeof(be64));
    return sockets::networkToHost64(be64);
}

int32_t Buffer::peekInt32() const
{
    int32_t be32 = 0;
    ::memcpy(&be32, peek(), sizeof(be32));
    return sockets::networkToHost32(be32);
}

int16_t Buffer::peekInt16() const
{
    int16_t be16 = 0;
    ::memcpy(&be16, peek(), sizeof(be16));
    return sockets::networkToHost16(be16);
}

int8_t Buffer::peekInt8() const
{
    int8_t be8 = 0;
    ::memcpy(&be8, peek(), sizeof(be8));
    return be8;
}

int64_t Buffer::readInt64()
{
    int64_t result = peekInt64();
    retrieve(sizeof(result));
    return result;
}

int32_t Buffer::readInt32()
{
    int32_t result = peekInt32();
    retrieve(sizeof(result));
    return result;
}

int16_t Buffer::readInt16()
{
    int16_t result = peekInt16();
    retrieve(sizeof(result));
    return result;
}

int8_t Buffer::readInt8()
{
    int8_t result = peekInt8();
    retrieve(sizeof(result));
    return result;
}

void Buffer::ensureWritableBytes(size_t len)
{
    if (writableBytes() < len)
    {
        makeSpace(len);
    }
}

char* Buffer::beginWrite()
{
    return begin() + writeIndex_;
}

const char* Buffer::beginWrite() const
{
    return begin() + writeIndex_;
}

void Buffer::hasWritten(size_t len)
{
    writeIndex_ += len;
}

void Buffer::prepend(const void* data, size_t len)
{
    readIndex_ -= len;
    const char* d = static_cast<const char*>(data);
    std::copy(d, d + len, begin() + readIndex_);
}

void Buffer::prependInt64(int64_t x)
{
    int64_t be64 = sockets::hostToNetwork64(x);
    prepend(&be64, sizeof(be64));
}

void Buffer::prependInt32(int32_t x)
{
    int32_t be32 = sockets::hostToNetwork32(x);
    prepend(&be32, sizeof(be32));
}

void Buffer::prependInt16(int16_t x)
{
    int16_t be16 = sockets::hostToNetwork16(x);
    prepend(&be16, sizeof(be16));
}

void Buffer::prependInt8(int8_t x)
{
    prepend(&x, sizeof(x));
}

size_t Buffer::buffferCapactity() const
{
    return buffer_.capacity();
}

//结合栈上空间，避免内存使用过大，提高内存使用率
//如果有10k个连接，每个连接分配64k缓冲区，将占用640M内存
//大多数时候，这些缓冲区使用率很低
size_t Buffer::readFd(int fd, int* savedErrno)
{
    //节省一次ioctl()系统调用(获取当前有多少可读数据)
    //因为准备了足够大的extraBuf,那就不需要用ioctl查看fd有多少可读
    char extraBuf[65536] = { 0 };
    struct iovec vec[2];
    const size_t writable = writableBytes();

    vec[0].iov_base = begin() + writeIndex_;
    vec[0].iov_len = writable;
    vec[1].iov_base = extraBuf;
    vec[1].iov_len = sizeof(extraBuf);

    const int iovcnt = (writable < sizeof(extraBuf)) ? 2 : 1;
    const ssize_t n = ::readv(fd, vec, iovcnt);
    if (n < 0)
    {
        *savedErrno = errno;
    }
    else if (static_cast<size_t>(n) <= writable)
    {
        writeIndex_ += n;
    }
    else
    {
        writeIndex_ = buffer_.size();
        append(extraBuf, n - writable);
    }

    return n;
}

const char* Buffer::begin() const
{
    return &*buffer_.begin();
}

char* Buffer::begin()
{
    return &*buffer_.begin();
}

void Buffer::makeSpace(size_t len)
{
    //当前空间不足
    if (writableBytes() + prependableBytes() < len + kPrePrePend)
    {
        assert(kPrePrePend <= readIndex_);
        size_t readable = readableBytes();
        std::copy(begin() + readIndex_, begin() + writeIndex_,
            begin() + kPrePrePend);
        readIndex_ = kPrePrePend;
        writeIndex_ = kPrePrePend + readable;

        buffer_.resize(writeIndex_ + len);
    }
    else
    {
        assert(kPrePrePend <= readIndex_);
        size_t readable = readableBytes();
        std::copy(begin() + readIndex_, begin() + writeIndex_,
            begin() + kPrePrePend);

        readIndex_ = kPrePrePend;
        writeIndex_ = kPrePrePend + readable;
    }
}

const char* Buffer::findSubString(const std::string& searchString, const char* start) const
{
    const char* startPlace = ((start == NULL) ? peek() : start);
    assert(peek() <= startPlace);
    assert(startPlace <= beginWrite());

    const char* findStrPlace = static_cast<const char*>(memmem(startPlace,
        beginWrite() - startPlace, searchString.c_str(), searchString.length()));

    return findStrPlace == beginWrite() ? NULL : findStrPlace;
}

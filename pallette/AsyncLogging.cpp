/*************************************************************************
  > File Name: AsyncLogging.cpp
  > Author: pengjunjie
  > Mail: 1002398145@qq.com
  > Created Time: 2018年12月23日 星期日 10时23分27秒
 ************************************************************************/

#include <pallette/AsyncLogging.h>

#include <pallette/LogFile.h>

#include <iostream>
#include <functional>
#include <chrono>
#include <assert.h>

using namespace pallette;

AsyncLogging::AsyncLogging(const std::string& basename,
    int rollSize, int flushInterval)
    : flushInterval_(flushInterval)
    , running_(false)
    , basename_(basename)
    , rollSize_(rollSize)
    , thread_(std::bind(&AsyncLogging::threadFunc, this), "Logging")
    , latch_(1)
    , currentBuffer_(new Buffer)
    , nextBuffer_(new Buffer)
{
    currentBuffer_->bzeros();
    nextBuffer_->bzeros();
    buffers_.reserve(16);
}

AsyncLogging::~AsyncLogging()
{
    if (running_)
    {
        stop();
    }
}

void AsyncLogging::append(const char* logline, int len)
{
    std::lock_guard<std::mutex> lock(mutex_);
    if (currentBuffer_->avail() > len)
    {
        currentBuffer_->append(logline, len);
    }
    else
    {
        buffers_.push_back(std::move(currentBuffer_));

        if (nextBuffer_)
        {
            currentBuffer_ = std::move(nextBuffer_);
        }
        else
        {
            currentBuffer_.reset(new Buffer);
        }
        currentBuffer_->append(logline, len);
        cond_.notify_one();
    }
}

void AsyncLogging::start()
{
    running_ = true;
    thread_.start();
    latch_.wait();
}

void AsyncLogging::stop()
{
    running_ = false;
    cond_.notify_one();
    thread_.join();
}

void AsyncLogging::threadFunc()
{
    assert(running_ == true);
    latch_.countDown();//线程启动后通知主线程
    LogFile output(basename_, rollSize_);
    BufferPtr newBuffer1(new Buffer);
    BufferPtr newBuffer2(new Buffer);
    newBuffer1->bzeros();
    newBuffer2->bzeros();
    BufferVector buffersToWrite;
    buffersToWrite.reserve(16);

    while (running_)
    {
        assert(buffersToWrite.empty());

        {
            std::unique_lock<std::mutex> lock(mutex_);
            if (buffers_.empty())
            {
                cond_.wait_for(lock, std::chrono::seconds(flushInterval_));
            }
            buffers_.push_back(std::move(currentBuffer_));
            currentBuffer_ = std::move(newBuffer1);//将备用缓冲区给前端，防止前端缓冲区阻塞
            buffersToWrite.swap(buffers_);//交换缓冲区队列，减小临界区域
            if (!nextBuffer_)
            {
                nextBuffer_ = std::move(newBuffer2);
            }
        }

        assert(!buffersToWrite.empty());
        for (auto& bufferVaule : buffersToWrite)
        {
            // FIXME: use unbuffered stdio FILE ? or use ::writev ?
            output.append(bufferVaule->data(), bufferVaule->length());
        }

        if (buffersToWrite.size() > 2)
        {
            buffersToWrite.resize(2);//这种情况下最多只需要两块buffer，去填充newBuffer1和newBuffer2
        }

        if (!newBuffer1)
        {
            assert(!buffersToWrite.empty());
            newBuffer1 = std::move(buffersToWrite.back());
            buffersToWrite.pop_back();
            newBuffer1->reset();
        }
        if (!newBuffer2)
        {
            assert(!buffersToWrite.empty());
            newBuffer2 = std::move(buffersToWrite.back());
            buffersToWrite.pop_back();
            newBuffer2->reset();
        }
        buffersToWrite.clear();
        output.flush();
    }

    output.flush();
}

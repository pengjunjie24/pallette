/*************************************************************************
  > File Name: AsyncLogging.cpp
  > Author: pengjunjie
  > Mail: 1002398145@qq.com
  > Created Time: 2018年12月23日 星期日 10时23分27秒
 ************************************************************************/

#include "AsyncLogging.h"

#include "LogFile.h"

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
    , thread_(std::bind(&AsyncLogging::threadFunc, this))
    , latch_(1)
    , currentBuffer_(new Buffer)
    , nextBuffer_(new Buffer)
{
    currentBuffer_->resetBuffer();
    nextBuffer_->resetBuffer();
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
    std::unique_lock<std::mutex> lock(mutex_);
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
    thread_.detach();
    latch_.wait();
}

void AsyncLogging::stop()
{
    running_ = false;
    cond_.notify_one();
}

void AsyncLogging::threadFunc()
{
    assert(running_ == true);
    latch_.countDown();
    LogFile output(basename_, rollSize_);
    BufferPtr newBuffer1(new Buffer);
    BufferPtr newBuffer2(new Buffer);
    newBuffer1->resetBuffer();
    newBuffer2->resetBuffer();
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
            currentBuffer_ = std::move(newBuffer1);
            buffersToWrite.swap(buffers_);
            if (!nextBuffer_)
            {
                nextBuffer_ = std::move(newBuffer2);
            }
        }

        assert(!buffersToWrite.empty());
        for (auto& bufferVaule : buffersToWrite)
        {
            output.append(bufferVaule->getString().c_str(), bufferVaule->getString().length());
        }
        //std::cout << "buffersToWrite.size(): " << buffersToWrite.size() << std::endl;

        if (buffersToWrite.size() > 2)
        {
            buffersToWrite.resize(2);
        }

        if (!newBuffer1)
        {
            assert(!buffersToWrite.empty());
            newBuffer1 = std::move(buffersToWrite.back());
            buffersToWrite.pop_back();
            newBuffer1->resetBuffer();
        }
        if (!newBuffer2)
        {
            assert(!buffersToWrite.empty());
            newBuffer2 = std::move(buffersToWrite.back());
            buffersToWrite.pop_back();
            newBuffer2->resetBuffer();
        }
        buffersToWrite.clear();
        output.flush();
    }

    output.flush();
}

#ifndef PALLETTE_OBJECT_POOL_H
#define PALLETTE_OBJECT_POOL_H

#include <memory>
#include <vector>
#include <functional>
#include <mutex>
#include <atomic>

#include <assert.h>
#include <stdio.h>

//对象池，性能较低
namespace pallette
{
    template <class T>
    class SimpleObjectPool
    {
    public:
        using ObjectDeleterType = std::function<void(T*)>;

        SimpleObjectPool(int maxSize = 1024, bool threadSafe = false)
            : maxSize_(maxSize)
            , useSize_(0)
            , objectPoolExist_(true)
            , mutex_(threadSafe ? new std::mutex : NULL)
        {
            addObject(maxSize_ / 100 + 1);
        }

        ~SimpleObjectPool()
        {
            objectPoolExist_ = false;
            useSize_ = 0;
        }

        std::unique_ptr<T, ObjectDeleterType> allotObject()
        {
            if (mutex_)
            {
                std::unique_lock<std::mutex> lock(*mutex_);
                return allotObjectWithUnlocked();
            }
            else
            {
                return allotObjectWithUnlocked();
            }
        }

        inline bool empty() const { return pool_.empty(); }
        int useSize() const{ return useSize_.load(); }

    private:
        void addObject(int addSize)
        {
            pool_.reserve(pool_.capacity() + addSize);
            for (int i = 0; i < addSize; ++i)
            {
                pool_.emplace_back(new T);
            }
        }

        std::unique_ptr<T, ObjectDeleterType> allotObjectWithUnlocked()
        {
            if (pool_.empty())
            {
                int usedSize = static_cast<int>(useSize());
                if ((usedSize * 2) < maxSize_)
                {
                    addObject(usedSize);
                }
                else if (usedSize < maxSize_)
                {
                    addObject(maxSize_ - usedSize);
                }
                else
                {
                    fprintf(stderr, "no more object");
                    return NULL;
                }
            }

            //every time add custom deleter for default unique_ptr
            std::unique_ptr<T, ObjectDeleterType> ptr(pool_.back().release(), [this](T* t)
            {
                //FIXME: objectPoolExist_被销毁
                if (objectPoolExist_)
                {
                    pool_.emplace_back(std::unique_ptr<T>(t));
                    --useSize_;
                }
                else
                {
                    delete t;
                }
            });

            pool_.pop_back();
            ++useSize_;
            return std::move(ptr);
        }

        const int maxSize_;//最大容量
        std::atomic<int> useSize_;
        bool objectPoolExist_;
        std::unique_ptr<std::mutex> mutex_;
        std::vector<std::unique_ptr<T>> pool_;
    };

}

#endif
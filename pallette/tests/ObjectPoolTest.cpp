
#include <pallette/ObjectPool.hpp>
#include <sys/time.h>

using namespace pallette;

struct TestStruct
{
    int aaa;
    long bbb;
    int ccc;
    long ddd;
    double ffff;
    char buf[124];
};

void testObjectPool()
{
    {
        SimpleObjectPool<TestStruct> test(50);

        printf("test.size(): %d\n", (int)test.useSize());
        {
            auto a = test.allotObject();
            auto b = test.allotObject();
            auto c = test.allotObject();

            printf("test.size(): %d\n", (int)test.useSize());
        }
        printf("test.size(): %d\n", (int)test.useSize());

        {
            std::vector<std::unique_ptr<TestStruct, SimpleObjectPool<TestStruct>::ObjectDeleterType>> vecTest;

            for (int i = 0; i < 40; ++i)
            {
                vecTest.emplace_back(test.allotObject());
            }
            printf("test.size(): %d\n", (int)test.useSize());
        }
        printf("test.size(): %d\n", (int)test.useSize());
    }
    printf("\n\n\n");

    {
        SimpleObjectPool<TestStruct>* objPool;
        {
            std::vector<std::unique_ptr<TestStruct, SimpleObjectPool<TestStruct>::ObjectDeleterType>> vecTest1;
            std::unique_ptr<TestStruct, SimpleObjectPool<TestStruct>::ObjectDeleterType> aa;
            std::unique_ptr<TestStruct, SimpleObjectPool<TestStruct>::ObjectDeleterType> bb;
            std::unique_ptr<TestStruct, SimpleObjectPool<TestStruct>::ObjectDeleterType> cc;

            {
                SimpleObjectPool<TestStruct> test1(50);
                objPool = &test1;

                for (int i = 0; i < 40; ++i)
                {
                    vecTest1.emplace_back(test1.allotObject());
                }
                aa = std::move(test1.allotObject());
                printf("test1.size(): %d\n", (int)test1.useSize());
            }
            bb = std::move(objPool->allotObject());
            cc = std::move(objPool->allotObject());
            printf("after1 test1.size(): %d\n", (int)objPool->useSize());
        }
        printf("after2 test1.size(): %d\n", (int)objPool->useSize());
    }
}


uint64_t getMicroSecondNow()
{
    const int kMicroSecondsPerSecond = 1000 * 1000;

    struct timeval tv;
    gettimeofday(&tv, NULL);
    uint64_t seconds = tv.tv_sec;
    return seconds * kMicroSecondsPerSecond + tv.tv_usec;
}

SimpleObjectPool<TestStruct> test(10000000);

void alloct(int size)
{
    printf("alloct: %d\n", size);

    std::vector<std::unique_ptr<TestStruct>> testUnique;
    uint64_t startTime = getMicroSecondNow();
    for (int i = 0; i < size; ++i)
    {
        testUnique.emplace_back(new TestStruct);
    }
    testUnique.clear();

    uint64_t endTime = getMicroSecondNow();
    uint64_t ptrSpendTime = endTime - startTime;
    printf("new unique_ptr total time: %.6fs\n", 1.0 * ptrSpendTime / 1000 / 1000);

    std::vector<TestStruct*> testNew;
    startTime = getMicroSecondNow();
    for (int i = 0; i < size; ++i)
    {
        testNew.emplace_back(new TestStruct);
    }

    for (auto& testValue : testNew)
    {
        delete testValue;
    }

    endTime = getMicroSecondNow();
    uint64_t newSpendTime = endTime - startTime;
    printf("new total time: %.6fs\n", 1.0 * newSpendTime / 1000 / 1000);

    std::vector<std::unique_ptr<TestStruct, SimpleObjectPool<TestStruct>::ObjectDeleterType>> testAlloct;
    startTime = getMicroSecondNow();
    for (int i = 0; i < size; ++i)
    {
        testAlloct.emplace_back(std::move(test.allotObject()));
    }
    testAlloct.clear();
    endTime = getMicroSecondNow();
    uint64_t alloctSpendTime = endTime - startTime;
    printf("SimpleObjectPool total time: %.6fs\n", 1.0 * alloctSpendTime / 1000 / 1000);
    
    double ptrToNew = 1.0 * ptrSpendTime / newSpendTime;
    double alloctToNew = 1.0 * alloctSpendTime / newSpendTime;

    printf("%lf : %lf : 1\n", alloctToNew, ptrToNew);

    printf("\n\n");

}

void testBench()
{
    alloct(10);
    alloct(1000);
    alloct(1000000);
    alloct(10000000);
    alloct(1000000);
    alloct(10000);
}

int main()
{
    //testObjectPool();
    testBench();
}

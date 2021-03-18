
#include <pallette/StatElapseTime.h>

using namespace pallette;

void fib(int32_t n, int32_t& result)
{
    if (n == 0)
    {
        result = 0;
    }
    if (n == 1)
    {
        result = 1;
    }
    int32_t f1 = 1;
    int32_t f2 = 0;
    result = 0;
    for (int i = 2; i <= n; i++)
    {
        result = f2 + f1;
        f2 = f1;
        f1 = result;
    }
}

int main()
{
    StatElapsedTimeus statElapsedTime;
    int32_t result = 0;
    for (int32_t i = 2; i < 40; i += 5)
    {
        statElapsedTime.execStatElapsedTime(std::bind(fib, i, std::ref(result)));
        printf("number %d is result %d, elapse %ld\n", i, result, statElapsedTime.getElapsedTimeus());
    }

    printf("max elapse %ld\n", statElapsedTime.getMaxElapsedTimeus());
    return 0;
}
#include <pallette/Thread.h>
#include <pallette/CurrentThread.h>

#include <string>
#include <functional>
#include <stdio.h>
#include <unistd.h>

using namespace std;
void mysleep(int seconds)
{
    timespec t = { seconds, 0 };
    nanosleep(&t, NULL);
}

void threadFunc()
{
    printf("tid=%d\n", pallette::current_thread::tid());
}

void threadFunc2(int x)
{
    printf("tid=%d, x=%d\n", pallette::current_thread::tid(), x);
}

void threadFunc3()
{
    printf("tid=%d\n", pallette::current_thread::tid());
    mysleep(1);
}

class Foo
{
public:
    explicit Foo(double x)
        : x_(x)
    {
    }

    void memberFunc()
    {
        printf("tid=%d, Foo::x_=%f\n", pallette::current_thread::tid(), x_);
    }

    void memberFunc2(const std::string& text)
    {
        printf("tid=%d, Foo::x_=%f, text=%s\n", pallette::current_thread::tid(), x_, text.c_str());
    }

private:
    double x_;
};

int main()
{
    printf("pid=%d, tid=%d\n", ::getpid(), pallette::current_thread::tid());

    pallette::Thread t1(threadFunc);
    t1.start();
    printf("t1.tid=%d\n", t1.tid());
    t1.join();

    pallette::Thread t2(bind(threadFunc2, 42),
        "thread for free function with argument");
    t2.start();
    printf("t2.tid=%d\n", t2.tid());
    t2.join();

    Foo foo(87.53);
    pallette::Thread t3(bind(&Foo::memberFunc, &foo),
        "thread for member function without argument");
    t3.start();
    t3.join();

    pallette::Thread t4(bind(&Foo::memberFunc2, ::ref(foo), std::string("Shuo Chen")));
    t4.start();
    t4.join();

    {
        pallette::Thread t5(threadFunc3);
        t5.start();
        // t5 may destruct eariler than thread creation.
    }
    mysleep(2);
    {
        pallette::Thread t6(threadFunc3);
        t6.start();
        mysleep(2);
        // t6 destruct later than thread creation.
    }
    sleep(2);
    printf("number of created threads %d\n", pallette::Thread::numCreated());
}

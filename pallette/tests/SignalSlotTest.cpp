#include <pallette/SignalSlot.hpp>
#include <pallette/noncopyable.h>
#include <pallette/Callbacks.h>

#include <stdio.h>

using namespace pallette;

class String
{
public:
    String(const char* str)
    {
        printf("String ctor this %p\n", this);
    }

    String(const String& rhs)
    {
        printf("String copy ctor this %p, rhs %p\n", this, &rhs);
    }

    String(String&& rhs)
    {
        printf("String move ctor this %p, rhs %p\n", this, &rhs);
    }
};

class Foo : noncopyable
{
public:
    void zero();
    void zeroc() const;
    void one(int);
    void oner(int&);
    void onec(int) const;
    void oneString(const String& str);
    // void oneStringRR(String&& str);
    static void szero();
    static void sone(int);
    static void soneString(const String& str);
};

void Foo::zero()
{
    printf("Foo::zero()\n");
}

void Foo::zeroc() const
{
    printf("Foo::zeroc()\n");
}

void Foo::szero()
{
    printf("Foo::szero()\n");
}

void Foo::one(int x)
{
    printf("Foo::one() x=%d\n", x);
}

void Foo::onec(int x) const
{
    printf("Foo::onec() x=%d\n", x);
}

void Foo::sone(int x)
{
    printf("Foo::sone() x=%d\n", x);
}

void Foo::oneString(const String& str)
{
    printf("Foo::oneString\n");
}

void Foo::soneString(const String& str)
{
    printf("Foo::soneString\n");
}


void testSignalSlotZero()
{
    Signal<void()> signal;

    printf("==== testSignalSlotZero ====\n");
    signal.call();

    Slot s1 = signal.connect(&Foo::szero);

    printf("========\n");
    signal.call();

    Foo f;
    Slot s2 = signal.connect(std::bind(&Foo::zero, &f));

    printf("========\n");
    signal.call();

    Slot s3 = signal.connect(std::bind(&Foo::one, &f, 42));

    printf("========\n");
    signal.call();

    const Foo cf;
    Slot s4 = signal.connect(std::bind(&Foo::zeroc, &cf));

    printf("========\n");
    signal.call();

    Slot s5 = signal.connect(std::bind(&Foo::onec, &cf, 128));

    printf("========\n");
    signal.call();

    s1 = Slot();
    printf("========\n");
    signal.call();


    s4 = s3 = s2 = Slot();
    printf("========\n");
    signal.call();

}

void testSignalSlotOne()
{
    Signal<void(int)> signal;

    printf("==== testSignalSlotOne ====\n");
    signal.call(50);

    Slot s4;
    {
        Slot s1 = signal.connect(&Foo::sone);

        printf("========\n");
        signal.call(51);

        Foo f;
        Slot s2 = signal.connect(std::bind(&Foo::one, &f, _1));

        printf("========\n");
        signal.call(52);

        const Foo cf;
        Slot s3 = signal.connect(std::bind(&Foo::onec, &cf, _1));

        printf("========\n");
        signal.call(53);

        s4 = s3;
    }

    printf("========\n");
    signal.call(54);
}

void testSignalSlotLife()
{
    Slot s1;
    printf("==== testSignalSlotLife ====\n");
    {
        Signal<void()> signal;
        s1 = signal.connect(&Foo::szero);

        printf("========\n");
        signal.call();

        Foo f;
        std::function<void()> func = std::bind(&Foo::zero, &f);

        s1 = signal.connect(std::move(func));

        printf("========\n");
        signal.call();
    }

}

int main()
{
    testSignalSlotZero();
    testSignalSlotOne();
    testSignalSlotLife();
}


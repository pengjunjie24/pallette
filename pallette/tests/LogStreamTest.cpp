
#include <pallette/LogStream.h>

#include <assert.h>

using namespace pallette;
using namespace std;

#define PALLETTE_CHECK_EQUAL(srcString, dstString) assert(srcString == dstString)

void testBooleans()
{
    LogStream os;
    const LogStream::Buffer& buf = os.getBuffer();

    PALLETTE_CHECK_EQUAL(buf.toString(), string(""));
    os << true;
    PALLETTE_CHECK_EQUAL(buf.toString(), string("1"));
    os << '\n';
    PALLETTE_CHECK_EQUAL(buf.toString(), string("1\n"));
    os << false;
    PALLETTE_CHECK_EQUAL(buf.toString(), string("1\n0"));
}

void testVoid()
{
    LogStream os;
    const LogStream::Buffer& buf = os.getBuffer();

    os << static_cast<void*>(0);
    PALLETTE_CHECK_EQUAL(buf.toString(), string("0x0"));
    os.resetBuffer();

    os << reinterpret_cast<void*>(8888);
    PALLETTE_CHECK_EQUAL(buf.toString(), string("0x22B8"));
    os.resetBuffer();
}

void testStrings()
{
    LogStream os;
    const LogStream::Buffer& buf = os.getBuffer();

    os << "Hello ";
    PALLETTE_CHECK_EQUAL(buf.toString(), string("Hello "));

    string name = "pengjunjie";
    os << name;
    PALLETTE_CHECK_EQUAL(buf.toString(), string("Hello pengjunjie"));
}

void testLong()
{
    LogStream os;
    const LogStream::Buffer& buf = os.getBuffer();
    for (int i = 0; i < 399; ++i)
    {
        os << "123456789 ";
        PALLETTE_CHECK_EQUAL(buf.length(), 10 * (i + 1));
        PALLETTE_CHECK_EQUAL(buf.avail(), 4000 - 10 * (i + 1));
    }

    os << "abcdefghi";
    PALLETTE_CHECK_EQUAL(buf.length(), 3999);
    PALLETTE_CHECK_EQUAL(buf.avail(), 1);
}

int main()
{
    testBooleans();
    testVoid();
    testLong();
}

#include <pallette/Buffer.h>

#include <assert.h>

#define PALLETTE_CHECK_EQUAL(srcString, dstString) assert(srcString == dstString)

using namespace pallette;
using namespace std;

void testBufferAppendRetrieve()
{
  Buffer buf;
  PALLETTE_CHECK_EQUAL(buf.readableBytes(), 0);
  PALLETTE_CHECK_EQUAL(buf.writableBytes(), Buffer::kInitalSize);
  PALLETTE_CHECK_EQUAL(buf.prependableBytes(), Buffer::kPrePrePend);

  const string str(200, 'x');
  buf.append(str);
  PALLETTE_CHECK_EQUAL(buf.readableBytes(), str.size());
  PALLETTE_CHECK_EQUAL(buf.writableBytes(), Buffer::kInitalSize - str.size());
  PALLETTE_CHECK_EQUAL(buf.prependableBytes(), Buffer::kPrePrePend);

  const string str2 =  buf.retrieveAsString(50);
  PALLETTE_CHECK_EQUAL(str2.size(), 50);
  PALLETTE_CHECK_EQUAL(buf.readableBytes(), str.size() - str2.size());
  PALLETTE_CHECK_EQUAL(buf.writableBytes(), Buffer::kInitalSize - str.size());
  PALLETTE_CHECK_EQUAL(buf.prependableBytes(), Buffer::kPrePrePend + str2.size());
  PALLETTE_CHECK_EQUAL(str2, string(50, 'x'));

  buf.append(str);
  PALLETTE_CHECK_EQUAL(buf.readableBytes(), 2 * str.size() - str2.size());
  PALLETTE_CHECK_EQUAL(buf.writableBytes(), Buffer::kInitalSize - 2 * str.size());
  PALLETTE_CHECK_EQUAL(buf.prependableBytes(), Buffer::kPrePrePend + str2.size());

  const string str3 =  buf.retrieveAllAsString();
  PALLETTE_CHECK_EQUAL(str3.size(), 350);
  PALLETTE_CHECK_EQUAL(buf.readableBytes(), 0);
  PALLETTE_CHECK_EQUAL(buf.writableBytes(), Buffer::kInitalSize);
  PALLETTE_CHECK_EQUAL(buf.prependableBytes(), Buffer::kPrePrePend);
  PALLETTE_CHECK_EQUAL(str3, string(350, 'x'));
}

#if 1
void testBufferGrow()
{
  Buffer buf;
  buf.append(string(400, 'y'));
  PALLETTE_CHECK_EQUAL(buf.readableBytes(), 400);
  PALLETTE_CHECK_EQUAL(buf.writableBytes(), Buffer::kInitalSize - 400);

  buf.retrieve(50);
  PALLETTE_CHECK_EQUAL(buf.readableBytes(), 350);
  PALLETTE_CHECK_EQUAL(buf.writableBytes(), Buffer::kInitalSize - 400);
  PALLETTE_CHECK_EQUAL(buf.prependableBytes(), Buffer::kPrePrePend + 50);

  buf.append(string(1000, 'z'));
  PALLETTE_CHECK_EQUAL(buf.readableBytes(), 1350);
  PALLETTE_CHECK_EQUAL(buf.writableBytes(), 0);
  PALLETTE_CHECK_EQUAL(buf.prependableBytes(), Buffer::kPrePrePend + 50); // FIXME

  buf.retrieveAll();
  PALLETTE_CHECK_EQUAL(buf.readableBytes(), 0);
  PALLETTE_CHECK_EQUAL(buf.writableBytes(), 1400); // FIXME
  PALLETTE_CHECK_EQUAL(buf.prependableBytes(), Buffer::kPrePrePend);
}

void testBufferInsideGrow()
{
  Buffer buf;
  buf.append(string(800, 'y'));
  PALLETTE_CHECK_EQUAL(buf.readableBytes(), 800);
  PALLETTE_CHECK_EQUAL(buf.writableBytes(), Buffer::kInitalSize - 800);

  buf.retrieve(500);
  PALLETTE_CHECK_EQUAL(buf.readableBytes(), 300);
  PALLETTE_CHECK_EQUAL(buf.writableBytes(), Buffer::kInitalSize - 800);
  PALLETTE_CHECK_EQUAL(buf.prependableBytes(), Buffer::kPrePrePend + 500);

  buf.append(string(300, 'z'));
  PALLETTE_CHECK_EQUAL(buf.readableBytes(), 600);
  PALLETTE_CHECK_EQUAL(buf.writableBytes(), Buffer::kInitalSize - 600);
  PALLETTE_CHECK_EQUAL(buf.prependableBytes(), Buffer::kPrePrePend);
}

void testBufferPrepend()
{
  Buffer buf;
  buf.append(string(200, 'y'));
  PALLETTE_CHECK_EQUAL(buf.readableBytes(), 200);
  PALLETTE_CHECK_EQUAL(buf.writableBytes(), Buffer::kInitalSize - 200);
  PALLETTE_CHECK_EQUAL(buf.prependableBytes(), Buffer::kPrePrePend);

  int x = 0;
  buf.prepend(&x, sizeof x);
  PALLETTE_CHECK_EQUAL(buf.readableBytes(), 204);
  PALLETTE_CHECK_EQUAL(buf.writableBytes(), Buffer::kInitalSize - 200);
  PALLETTE_CHECK_EQUAL(buf.prependableBytes(), Buffer::kPrePrePend - 4);
}

void testBufferReadInt()
{
  Buffer buf;
  buf.append("HTTP");

  PALLETTE_CHECK_EQUAL(buf.readableBytes(), 4);
  PALLETTE_CHECK_EQUAL(buf.peekInt8(), 'H');
  int top16 = buf.peekInt16();
  PALLETTE_CHECK_EQUAL(top16, 'H'*256 + 'T');
  PALLETTE_CHECK_EQUAL(buf.peekInt32(), top16*65536 + 'T'*256 + 'P');

  PALLETTE_CHECK_EQUAL(buf.readInt8(), 'H');
  PALLETTE_CHECK_EQUAL(buf.readInt16(), 'T'*256 + 'T');
  PALLETTE_CHECK_EQUAL(buf.readInt8(), 'P');
  PALLETTE_CHECK_EQUAL(buf.readableBytes(), 0);
  PALLETTE_CHECK_EQUAL(buf.writableBytes(), Buffer::kInitalSize);

  buf.appendInt8(-1);
  buf.appendInt16(-2);
  buf.appendInt32(-3);
  PALLETTE_CHECK_EQUAL(buf.readableBytes(), 7);
  PALLETTE_CHECK_EQUAL(buf.readInt8(), -1);
  PALLETTE_CHECK_EQUAL(buf.readInt16(), -2);
  PALLETTE_CHECK_EQUAL(buf.readInt32(), -3);
}

void testBufferFindEOL()
{
  Buffer buf;
  buf.append(string(100000, 'x'));
  const char* null = NULL;
  PALLETTE_CHECK_EQUAL(buf.findSubString("\r\n"), null);
  PALLETTE_CHECK_EQUAL(buf.findSubString("\r\n", buf.peek() + 90000), null);
}
#endif

int main()
{
    testBufferAppendRetrieve();
#if 1
    testBufferGrow();
    testBufferInsideGrow();
    testBufferPrepend();
    testBufferReadInt();
    testBufferFindEOL();
#endif
}

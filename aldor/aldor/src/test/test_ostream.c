#include "axlobs.h"
#include "foam.h"
#include "testlib.h"
#include "ostream.h"

void testBuffer(void);
void testBuffer2(void);
void testNull(void);

void 
ostreamTest()
{
	TEST(testBuffer);
	TEST(testBuffer2);
	TEST(testNull);
}

void 
testBuffer()
{
	int i;
	char *orig_txt = "abcdefghi";
	char *txt = "abcdefghi";
	for (i=0; i<strlen(txt); i++) {
		Buffer buffer = bufNew();
		OStream s = ostreamNewFrBuffer(buffer);
		int n = ostreamWrite(s, txt, i);
		testIntEqual("ostreamWriteReturn", i, n);
		String txt2 = bufLiberate(buffer);
		String expect = strnCopy(txt, i);
		testStringEqual("ostreamWrite-no-src-change", orig_txt, txt);
		testStringEqual("ostreamWrite-Substring", expect, txt2);
		ostreamClose(s);
		ostreamFree(s);
	}
}

void 
testBuffer2()
{
	StringList l = listList(String)(3, "", "hello", "a");
	while (l != listNil(String)) {
		Buffer buffer = bufNew();
		String txt = car(l);
		OStream s = ostreamNewFrBuffer(buffer);
		int n = ostreamWrite(s, txt, -1);
		String txt2 = bufLiberate(buffer);
		testIntEqual("ostreamWriteReturn", strlen(txt), n);
		testStringEqual("ostreamWriteVal", txt, txt2);

		l = listFreeCons(String)(l);
	}
}

void 
testNull()
{
	int i;
	char *orig_txt = "abcdefghi";
	char *txt = "abcdefghi";
	for (i=0; i<strlen(txt); i++) {
		OStream s = ostreamNewFrDevNull();
		int n = ostreamWrite(s, txt, i);
		testIntEqual("ostreamWriteReturn", i, n);
		testStringEqual("ostreamWrite-no-src-change", orig_txt, txt);
		ostreamClose(s);
		ostreamFree(s);
	}
}

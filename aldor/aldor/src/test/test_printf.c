#include "axlobs.h"
#include "format.h"
#include "testlib.h"
#include "strops.h"

local void testPrintf1();
local void testPrintf2();

void printfTest()
{
	TEST(testPrintf1);
	TEST(testPrintf2);
}

char arr[100];
char *arrptr = &arr[0];

local int 
arrWrite(const char *s, int n)
{
	if (n == -1) {
		char * r = strcpy(arrptr, s);
		int c = strlen(s);

		arrptr += c;
		return c;
	}
	else {
		strncpy(arrptr, s, n);
		arrptr += n;
		return n;
	}
}


local void
testPrintf1()
{
	int cc;
	String fmt = "foo: %s";
	String arg1 = "bar";
	char arr2[100];
	memset(arr, 'X', 100);
	sprintf(arr2, fmt, arg1);
	cc = xprintf(arrWrite, fmt, arg1);
	
	testStringEqual("content", arr2, arr);
	testIntEqual("retval", cc, strlen(arr));
}

local void checkPrintfCase(int w, const char *txt);
local void
testPrintf2()
{
	checkPrintfCase(0, "");
	checkPrintfCase(1, "");
	checkPrintfCase(2, "");
	checkPrintfCase(0, "x");
	checkPrintfCase(1, "x");
	checkPrintfCase(2, "x");
	checkPrintfCase(1, "xy");
	checkPrintfCase(5, "xy");

}

local void
checkPrintfCase(int w, const char *txt)
{
	String name = strPrintf("%d-[%s]", w, txt);
	String s2;
	char buf[20];
	sprintf(buf, "%*s", w, txt);
	s2 = strPrintf("%*s", w, txt);

	testStringEqual(name, buf, s2);
	strFree(name);
}

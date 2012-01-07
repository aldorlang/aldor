#include "axlobs.h"
#include "ostream.h"
#include "testlib.h"

local void testPrintf1();

void printfTest()
{
	TEST(testPrintf1);
}

char arr[100];
char *arrptr = &arr[0];

local int 
arrWrite(const char *s, int n)
{
	if (n == -1) {
		char * r = strcpy(arrptr, s);
		int c = strlen(s);
		strlen(s);
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
	char arr2[100];
	memset(arr, 'X', 100);
	String fmt = "foo: %s";
	String arg1 = "bar";
	sprintf(arr2, fmt, arg1);
	int cc = xprintf(arrWrite, fmt, arg1);
	
	testStringEqual("content", arr2, arr);
	testIntEqual("retval", cc, strlen(arr));
}

/*****************************************************************************
 *
 * strops_t.c: Test string operations.
 *
 * Copyright (c) 1990-2007 Aldor Software Organization Ltd (Aldor.org).
 *
 ****************************************************************************/

#if !defined(TEST_STROPS) && !defined(TEST_ALL)

void testString(void) { }

#else

#include "axlgen.h"
#include "opsys.h"
#include "strops.h"
#include "util.h"

String
testStrVPrintf(const char *fmt, ...)
{
	String	s;
	va_list	argp;
	va_start(argp, fmt);
	s = strVPrintf(fmt, argp);
	va_end(argp);
	return s;
}

void
testMisc(void)
{
	String	s,s0,s1,s2,s3;
	int	n;

	s1 = "Pre";  s2 = "Prefix";  s3 = "Postfix";
	printf("strIsPrefix:\n");
	s = strIsPrefix(s1, s2);
	if (s != 0) {
		printf("The prefix is \"%s\", ",s1);
		printf("the rest is \"%s\".\n",s);
	}
	printf("strIsSuffix:\n");
	s = strIsSuffix("fix", s3);
	if (s != 0) {
		printf("\"%s\" is a suffix of \"%s\".\n",s,s3);
	}
	printf("strMatch:\n");
	printf("The first %d characters of \"%s\" and \"%s\" match.\n",
	       	(int) strMatch(s2, s3),s2,s3);

	printf("strCopy:\n");
	s0 = strCopy(s2);
	printf("\"%s\" and \"%s\" match\n",s2,s0);

	printf("strFree:\n");
	strFree(s0);

	n = 3;
	printf("strnCopy:\n");
	s0 = strnCopy(s2,n);
	printf("The first %d characters of \"%s\" are \"%s\".\n",n,s2,s0);
	strFree(s0);

	printf("strConcat, strLength:\n");
	printf("Concatenating \"%s\" and \"%s\" yields:\n",s1,s3);
	s0 = strConcat(s1,s3);
	printf("\"%s\", of length %d.\n",s0, (int) strLength(s0));
	strFree(s0);

	printf("strlConcat:\n");
	printf("Concatenating \"%s\", \"%s\" and \"%s\" yields:\n",s1,s2,s3);
	s = strlConcat(s1,s2,s3,NULL);
	printf("\"%s\"\n",s);
	strFree(s);

	printf("strPrintf/strVPrintf:\n");
	printf             ("%s--%s--%s--%d--%10.4g\n", s1, s2, s3, 13, 3.14);
	s0 = strPrintf     ("%s--%s--%s--%d--%10.4g\n", s1, s2, s3, 13, 3.14);
	printf("%s", s0);
	strFree(s0);
	s0 = testStrVPrintf("%s--%s--%s--%d--%10.4g\n", s1, s2, s3, 13, 3.14);
	printf("%s", s0);
	strFree(s0);

	printf("strPrint:\n");
	strPrint(osStdout, "The rain's flat by `\\/' gum!, he \"said\". \012",
		'"', '"', '\\', "\\%#o");
	printf("\n");
	strPrint(osStdout, "The rain's flat by `\\/' gum!, he \"said\". \012",
		'`', '\'', '!', "!x%x");
	printf("\n");
}

#define CHECK(x) {if (!(x)) bug("Test failed %s %d", __FILE__, __LINE__);}

void
testSplit(void)
{
	String lhs, rhs;

	strSplitLast(strCopy("a.b.c"), '.', &lhs, &rhs);
	CHECK(strEqual("a.b", lhs));
	CHECK(strEqual("c", rhs));

	strSplitLast(strCopy("a.b."), '.', &lhs, &rhs);
	CHECK(strEqual("a.b", lhs));
	CHECK(strEqual("", rhs));

	strSplitLast(strCopy("b"), '.', &lhs, &rhs);
	CHECK(lhs == NULL);
	CHECK(strEqual("b", rhs));

}

void
testString(void)
{
	testMisc();
	testSplit();
}

#endif

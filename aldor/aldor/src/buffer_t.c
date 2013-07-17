/*****************************************************************************
 *
 * buffer_t.c: Test grow-on-demand-buffers.
 *
 * Copyright (c) 1990-2007 Aldor Software Organization Ltd (Aldor.org).
 *
 ****************************************************************************/

#if !defined(TEST_BUFFER) && !defined(TEST_ALL)

void testBuffer(void) { }

#else

#include "axlgen.h"
#include "buffer.h"
#include "opsys.h"

void
testBuffer(void)
{
	Buffer	b;
	int	i, n;
	String	t, s = "The rain in Iberia falls mainly in the cafeteria. ";

#if EDIT_1_0_n1_07
	printf("bufNew: ");
	b = bufNew();
	printf("[%d/%d]\n\n", (int) bufPosition(b), (int) bufSize(b));
	
	printf("BUF_START / BUF_ADD1:\n");
	BUF_START(b);
	for (i = 0, n = strlen(s); i < n; i++)  BUF_ADD1(b, s[i]);
	BUF_ADD1(b, char0);
	t = bufChars(b);
	printf("\"%s\" [%d]\n", t, n);
	printf("[%d/%d]\n\n", (int) bufPosition(b), (int) bufSize(b));

	printf("bufNeed(%d): ", (int) BUF_INIT_SIZE + 5);
	bufNeed(b, BUF_INIT_SIZE + 5);
	printf("[%d/%d]\n", (int) bufPosition(b), (int) bufSize(b));
	printf("bufChars: ");
	printf("\"%s\" [%d]\n\n", bufChars(b), (int) strlen(bufChars(b)));

	printf("bufGrow(%d): ", 5);
	b = bufGrow(b, 5);
	printf("[%d/%d]\n", (int) bufPosition(b), (int) bufSize(b));
	printf("bufChars: ");
	printf("\"%s\" [%d]\n\n", bufChars(b), (int) strlen(bufChars(b)));

	printf("bufFree: ");
	bufFree(b);
	printf("OK\n\n");

	printf("bufNew: ");
	b = bufNew();
	printf("[%d/%d]\n\n", (int) bufPosition(b), (int) bufSize(b));

	printf("BUF_ADD1 across a boundary: \n");
	n = bufSize(b);
	BUF_START(b);
	for (i = 0; i < n-5; i++)
		BUF_ADD1(b, "1234567890"[i%10]);
	for (i = 0; i < 10; i++) {
		int c = "abcdefghij"[i%10];
		BUF_ADD1(b, c);
		printf(" %c: [%d/%d]", c, (int) bufPosition(b), (int) bufSize(b));
	}
	BUF_ADD1(b, char0);
	t = bufChars(b);
	printf("\n");

	printf("bufChars: ");
	printf("\"%s\" [%d]\n\n", t, (int) strlen(t));
#else
	printf("bufNew: ");
	b = bufNew();
	printf("[%d/%d]\n\n", bufPosition(b), bufSize(b));
	
	printf("BUF_START / BUF_ADD1:\n");
	BUF_START(b);
	for (i = 0, n = strlen(s); i < n; i++)  BUF_ADD1(b, s[i]);
	BUF_ADD1(b, char0);
	t = bufChars(b);
	printf("\"%s\" [%d]\n", t, n);
	printf("[%d/%d]\n\n", bufPosition(b), bufSize(b));

	printf("bufNeed(%d): ", BUF_INIT_SIZE + 5);
	bufNeed(b, BUF_INIT_SIZE + 5);
	printf("[%d/%d]\n", bufPosition(b), bufSize(b));
	printf("bufChars: ");
	printf("\"%s\" [%d]\n\n", bufChars(b), strlen(bufChars(b)));

	printf("bufGrow(%d): ", 5);
	b = bufGrow(b, 5);
	printf("[%d/%d]\n", bufPosition(b), bufSize(b));
	printf("bufChars: ");
	printf("\"%s\" [%d]\n\n", bufChars(b), strlen(bufChars(b)));

	printf("bufFree: ");
	bufFree(b);
	printf("OK\n\n");

	printf("bufNew: ");
	b = bufNew();
	printf("[%d/%d]\n\n", bufPosition(b), bufSize(b));

	printf("BUF_ADD1 across a boundary: \n");
	n = bufSize(b);
	BUF_START(b);
	for (i = 0; i < n-5; i++)
		BUF_ADD1(b, "1234567890"[i%10]);
	for (i = 0; i < 10; i++) {
		int c = "abcdefghij"[i%10];
		BUF_ADD1(b, c);
		printf(" %c: [%d/%d]", c, bufPosition(b), bufSize(b));
	}
	BUF_ADD1(b, char0);
	t = bufChars(b);
	printf("\n");

	printf("bufChars: ");
	printf("\"%s\" [%d]\n\n", t, strlen(t));
#endif


	printf("bufPrint: ");
	bufPrint(osStdout, b);
	printf("\n");

	printf("bufLiberate: ");
	t = bufLiberate(b);
	printf("\"%s\"\n\n", t);

	printf("strFree: ");
	strFree(t);
	printf("OK\n\n");

	printf("bufPrintf:\n");
	b = bufNew();
	n  = bufPrintf(b, "This %s a %s command.  ", "is", "format");
	n += bufPrintf(b, "! marks the %d-th character.\n", n+1);
	printf("%d: %s", n, bufChars(b));
	bufFree(b);

}

#endif

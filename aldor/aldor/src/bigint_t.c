/*****************************************************************************
 *
 * bigint_t.c:	Test big integer operations.
 *
 * Copyright (c) 1990-2007 Aldor Software Organization Ltd (Aldor.org).
 *
 ****************************************************************************/

#if !defined(TEST_BIGINT) && !defined(TEST_ALL)

void testBigint(void) { }

#else

#include "axlgen.h"
#include "bigint.h"

#define testBI_Limit (sizeof(testBI)/sizeof(testBI[0]))
#define ReallyBig   303030

struct {
	long	n;
	char	*s;
	BInt	b;
} testBI[] = {
	{ ReallyBig,   "-3431053733523206522895434139"},
	{ ReallyBig,   "-3431053733523205522895434139"},
	{ ReallyBig,   "-8796093153280"},
	{ ReallyBig,   "-4294967296"},
	{-2147483647-1,"-2147483648"},
	{-2147483647,  "-2147483647"},
	{-2000000000,  "-2000000000"},
	{-1073741824,  "-1073741824"},
	{-1073741823,  "-1073741823"},
	{-1073741822,  "-1073741822"},
	{-1000000000,  "-1000000000"},
	{-65537,       "-65537"     },
	{-65536,       "-65536"     },
	{-65535,       "-65535"     },
	{-16385,       "-16385"     },
	{-16384,       "-16384"     },
	{-16383,       "-16383"     },
	{-8193,        "-8193"      },
	{-8192,        "-8192"      },
	{-8191,        "-8191"      },
	{-4096,        "-4096"      },
	{-5,	       "-5"         },
	{-1,	       "-1"         },
	{ 0,	       "0"          },
	{ 1,	       "1"          },
	{ 5,	       "5"          },
	{ 4096,        "4096"       },
	{ 8191,        "8191"       },
	{ 8192,        "8192"       },
	{ 8193,        "8193"       },
	{ 16382,       "16382"      },
	{ 16383,       "16383"      },
	{ 16384,       "16384"      },
	{ 65534,       "65534"      },
	{ 65535,       "65535"      },
	{ 65536,       "65536"      },
	{ 1000000000,  "1000000000" },
	{ 1073741822,  "1073741822" },
	{ 1073741823,  "1073741823" },
	{ 1073741824,  "1073741824" },
	{ 2000000000,  "2000000000" },
	{ 2147483646,  "2147483646" },
	{ 2147483647,  "2147483647" },
	{ ReallyBig,   "4294967296"},
	{ ReallyBig,   "8796093153280"},
	{ ReallyBig,   "769169471909453733523206522895434140"},
	{ ReallyBig,   "769169471909453734523206522895434140"} 
};

void
testBI_Start(char *what)
{
	printf("*** Test %s:\n", what);
}

void
testBI_Bullet(int ix)
{
	long	l = testBI[ix].n;

	printf((l == ReallyBig) ? " long: " : " %ld: ", l);
}

void
testBI_End(void)
{
	printf("\n");
}

void
testBigint(void)
{
	long	n;
	int	i, j, k;
	BInt	a, b, c, d, t0, t1, t2;
	String	s;

	testBI_Start("bintAlloc bintAllocPlaces bintFree");
	for (i = 0; i < 65; i++) {
		b = bintAlloc(i);
		printf("%d bits. ", i);
		bintFree(b);

		b = bintAllocPlaces(i);
		printf("%d places.\n", i);
		bintFree(b);
	}
	testBI_End();

	testBI_Start("bintFrString (+ iintTimesPlusI)");
	for (i = 0; i < testBI_Limit; i++) {
		testBI_Bullet(i);

		s = testBI[i].s;
		b = testBI[i].b = bintFrString(s);
		bintPrint(osStdout, b);
		printf("\n");
	}
	testBI_End();

	testBI_Start("bintToString (+ iintDivideI)");
	for (i = 0; i < testBI_Limit; i++) {
		testBI_Bullet(i);

		b = testBI[i].b;
		s = bintToString(b);
		printf("\"%s\"\n", s);
		strFree(s);
	}
	testBI_End();
	
	testBI_Start("bintPrint bintPrint16 bintPrint2");
	for (i = 0; i < testBI_Limit; i++) {
		testBI_Bullet(i);

		b = testBI[i].b;
		bintPrint(osStdout, b);
		printf(" = ");
		bintPrint16(osStdout, b);
		printf(" =\n\t");
		bintPrint2(osStdout, b);
		printf("\n");
	}
	testBI_End();

	testBI_Start("bintNew bintCopy");
	for (i = 0; i < testBI_Limit; i++) {
		testBI_Bullet(i);

		b = testBI[i].b;
		c = bintCopy(b);
		bintPrint(osStdout, b);
		printf(" = ");
		bintPrint(osStdout, c);
		printf("\n");
		bintFree(c);
	}
	testBI_End();
		
	testBI_Start("bintIsSmall bintSmall");
	for (i = 0; i < testBI_Limit; i++) {
		testBI_Bullet(i);

		n = testBI[i].n;
		b = testBI[i].b;
		if (bintIsSmall(b)) 
			printf("small = %ld\n", bintSmall(b));
		else
			printf("not small\n");
	}
	testBI_End();
		
	testBI_Start("xintStoreI xintStore xintImmedIfCan");
	for (i = 0; i < testBI_Limit; i++) {
		n = testBI[i].n;
		if (n == ReallyBig) continue;
		testBI_Bullet(i);

		printf("\n");

		printf("    N ");
		a = xintStoreI(n);
		bintPrint2(osStdout, a);
		printf("\n");

		b = xintImmedIfCan(a);
		printf("    I ");
		bintPrint2(osStdout, b);
		printf("\n");
		bintFree(b);

		printf("    S ");
		a = bintNew(n);
		b = xintStore(a);
		bintPrint2(osStdout, b);
		printf("\n");
		bintFree(b);
	}
	testBI_End();

	testBI_Start("xintCopyInI");
	for (i = 0; i < testBI_Limit; i++) {
		n = testBI[i].n;
		if (n == ReallyBig) continue;
		testBI_Bullet(i);

		b = bintAlloc(30);
		b = xintCopyInI(b, n);
		bintPrint2(osStdout, b);
		printf("\n");
		bintFree(b);
	}
	testBI_End();

	testBI_Start("xintNeeds");
	for (i = 0; i < testBI_Limit; i++) {
		testBI_Bullet(i);

		b = bintCopy(testBI[i].b);
		printf("\n");
		for (j = 0; j <= 126; j += 21) {
			printf("   needs %3d: ", j);
			b = xintNeeds(b, j);
			bintPrint2(osStdout, b);
			printf("\n");
		}
		bintFree(b);
	}
	testBI_End();

	testBI_Start("bint0 bint1");
	printf(" 0: "); bintPrint(osStdout, bint0); printf("\n");
	printf(" 1: "); bintPrint(osStdout, bint1); printf("\n");
	testBI_End();

	testBI_Start("bintIsNeg bintIsZero bintIsPos");
	for (i = 0; i < testBI_Limit; i++) {
		testBI_Bullet(i);

		b = testBI[i].b;
		if (bintIsNeg(b))  printf("neg ");
		if (bintIsZero(b)) printf("zero ");
		if (bintIsPos(b))  printf("pos ");
		printf("\n");
	}
	testBI_End();

	testBI_Start("bintEQ");
	for (i = 0; i < testBI_Limit; i++) {
		a = testBI[i].b;
		for (j = 0; j < testBI_Limit; j++) {
			b = testBI[j].b;
			printf("%s", bintEQ(a,b)?"T":"-");
		}
		printf("\n");
	}
	testBI_End();

	testBI_Start("bintNE");
	for (i = 0; i < testBI_Limit; i++) {
		a = testBI[i].b;
		for (j = 0; j < testBI_Limit; j++) {
			b = testBI[j].b;
			printf("%s", bintNE(a,b)?"T":"-");
		}
		printf("\n");
	}
	testBI_End();

	testBI_Start("bintLT");
	for (i = 0; i < testBI_Limit; i++) {
		a = testBI[i].b;
		for (j = 0; j < testBI_Limit; j++) {
			b = testBI[j].b;
			printf("%s", bintLT(a,b)?"T":"-");
		}
		printf("\n");
	}
	testBI_End();

	testBI_Start("bintLE");
	for (i = 0; i < testBI_Limit; i++) {
		a = testBI[i].b;
		for (j = 0; j < testBI_Limit; j++) {
			b = testBI[j].b;
			printf("%s", bintLE(a,b)?"T":"-");
		}
		printf("\n");
	}
	testBI_End();

	testBI_Start("bintGT");
	for (i = 0; i < testBI_Limit; i++) {
		a = testBI[i].b;
		for (j = 0; j < testBI_Limit; j++) {
			b = testBI[j].b;
			printf("%s", bintGT(a,b)?"T":"-");
		}
		printf("\n");
	}
	testBI_End();

	testBI_Start("bintGE");
	for (i = 0; i < testBI_Limit; i++) {
		a = testBI[i].b;
		for (j = 0; j < testBI_Limit; j++) {
			b = testBI[j].b;
			printf("%s", bintGE(a,b)?"T":"-");
		}
		printf("\n");
	}
	testBI_End();

	testBI_Start("bintAbs");
	for (i = 0; i < testBI_Limit; i++) {
		testBI_Bullet(i);

		b = testBI[i].b;
		printf("abs(");
		bintPrint(osStdout, b);

		printf(") = ");
		c = bintAbs(b);
		bintPrint(osStdout, c);
		printf("\n");

		bintFree(c);
	}
	testBI_End();

	testBI_Start("bintNegate");
	for (i = 0; i < testBI_Limit; i++) {
		testBI_Bullet(i);

		b = testBI[i].b;
		printf("negative ");
		bintPrint(osStdout, b);

		printf(" = ");
		c = bintNegate(b);
		bintPrint(osStdout, c);
		printf("\n");

		bintFree(c);
	}
	testBI_End();

	testBI_Start("bintPlus");
	d = bintNew(int0);
	for (i = 0; i < testBI_Limit; i++) {
		b = testBI[i].b;
		t1 = d;
		  d = bintPlus(b, d);
		bintFree(t1);
	}
	printf("   Sum is ");
	bintPrint(osStdout, d);
	printf("\n");

	bintFree(d);
	printf("\n");

	for (i = 0; i < testBI_Limit; i++) {
		a = testBI[i].b;
		for (j = 0; j < testBI_Limit; j++) {
			b = testBI[j].b;
			c = bintPlus(a,b);

			bintPrint(osStdout, a);
			printf(" + ");
			bintPrint(osStdout, b);
			printf(" = ");
			bintPrint(osStdout, c);
			printf("\n");

			bintFree(c);
		}
	}
	printf("\n");

	d = bintNew(int0);
	for (i = 0; i < testBI_Limit; i++) {
		a = testBI[i].b;
		for (j = 0; j < testBI_Limit; j++) {
			b = testBI[j].b;

			t1 = bintPlus(a,b);
			t2 = d;
			  d = bintPlus(t1, d);
			bintFree(t2);
			bintFree(t1);
		}
	}
	printf("   Sum of addition table is ");
	bintPrint(osStdout, d);
	printf("\n");
	bintFree(d);
	testBI_End();

	testBI_Start("bintMinus");
	for (i = 0; i < testBI_Limit; i++) {
		a = testBI[i].b;
		for (j = 0; j < testBI_Limit; j++) {
			b = testBI[j].b;
			c = bintMinus(a,b);

			bintPrint(osStdout, a);
			printf(" - ");
			bintPrint(osStdout, b);
			printf(" = ");
			bintPrint(osStdout, c);
			printf("\n");

			bintFree(c);
		}
	}
	printf("\n");
	testBI_End();

	testBI_Start("bintTimes");
	for (i = 0; i < testBI_Limit; i++) {
		a = testBI[i].b;
		for (j = 0; j < testBI_Limit; j++) {
			b = testBI[j].b;
			c = bintTimes(a,b);

			bintPrint(osStdout, a);
			printf(" * ");
			bintPrint(osStdout, b);
			printf(" = ");
			bintPrint(osStdout, c);
			printf("\n");

			bintFree(c);
		}
	}
	testBI_End();

	testBI_Start("bintDivide");
	for (i = 0; i < testBI_Limit; i++) {
		a = testBI[i].b;

		for (j = 0; j < testBI_Limit; j++) {
			b = testBI[j].b;

			if (bintIsZero(b)) continue;

			c = bintDivide(&d, a,b);

			printf("[ ");
			bintPrint(osStdout, a);
			printf(" ] = [ ");
			bintPrint(osStdout, b);
			printf(" ] ");

			printf("* ");
			bintPrint(osStdout, c);
			printf(" + ");
			bintPrint(osStdout, d);

			t0 = bintIsNeg(b) ? bintNegate(b) : bintCopy(b);
			t1 = bintTimes(b, c);
			t2 = bintPlus(t1, d);

			if (bintNE(a, t2)) {
				printf(" ***** Bad Value ***** = ");
				bintPrint(osStdout, t2);
			}
			if (bintIsNeg(d)) {
				t0 = xintNegate(t0);
				if (bintLE(d, t0))
					printf(" ***** Out of range ***** ");
			}
			else {
				if (bintGE(d, t0)) 
					printf(" ***** Out of range ***** ");
			}

			printf("\n");


			bintFree(c);
			bintFree(d);
			bintFree(t0);
			bintFree(t1);
			bintFree(t2);
		}
	}
	testBI_End();

	testBI_Start("bintLength bintBit");
	for (i = 0; i < testBI_Limit; i++) {
		testBI_Bullet(i);

		b = testBI[i].b;
		k = bintLength(b);
		printf("length = %d, ", k);
		printf("leading bits = ");
		for (j = k - 1; j >= 0 && j + 4 > k; j--)
			printf(bintBit(b,j) ? "1 " : "0 ");
		printf("\n");
	}
	testBI_End();

	testBI_Start("bintShift (+iintShift)");
	for (i = 0; i < testBI_Limit; i++) {
		int jO, jF;

		testBI_Bullet(i);

		b = testBI[i].b;
		bintPrint2(osStdout, b);
		printf("\n");

		jO = -(int)bintLength(b)-2;
		jF =  35;
		for (j = jO; j <= jF; j++) {
			d = bintShift(b, j);
			printf("    >>%4d: ", j);
			bintPrint(osStdout, d);
			bintFree(d);
			printf("\n");
		}
	}
	testBI_End();

	for (i = 0; i < testBI_Limit; i++) bintFree(testBI[i].b);
}

#endif

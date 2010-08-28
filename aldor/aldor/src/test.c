/*****************************************************************************
 *
 * test.c: Library self test.
 *
 * Copyright (c) 1990-2007 Aldor Software Organization Ltd (Aldor.org).
 *
 ****************************************************************************/

# include "axlgen0.h"


#ifdef TEST_STAND_ALONE
int
main(int argc, char **argv)
{
	if (argc == 1)
		testSelf("show");
	else
		while (argc-- > 1) testSelf(*++argv);

	return EXIT_SUCCESS;
}
#endif


struct test {
  	char	* name;
	void	(*fun)(void);
};

void	testStartNetStore (void);
void	testShowNetStore  (void);
void	testRun		  (String name, void (*fun)(void));

MostAlignedType *
testSelfStoHandler(int errnum)
{
	String msg;

	switch (errnum) {
	case StoErr_OutOfMemory:  msg = "out of memory";                  break;
	case StoErr_UsedNonalloc: msg = "using non-allocated space";	  break;
	case StoErr_CantBuild:	  msg = "can't build internal structure"; break;
	default:		  msg = "unexpected";			  break;
	}
	fprintf(stderr, "Storage allocation error (%s).\n", msg);
	exitFailure();
	NotReached(return 0);
}

void
testSelf(String testName)
{
	int	            i;
	StoErrorFun	    h0;
	extern struct test  suite[];


	/* Run with default error handlers. */
	h0 = stoSetHandler(testSelfStoHandler);

  	if (strcmp(testName, "all") == 0) {
	  	for (i=0; suite[i].name != 0; i++)
			testRun(suite[i].name, suite[i].fun);
	}
	else {
	  	for (i=0; suite[i].name != 0; i++)
		  	if (strcmp(testName, suite[i].name) == 0) break;

		if (suite[i].name == 0)
		  	printf("\nTest \"%s\" not found.\n", testName);
		else
			testRun(suite[i].name, suite[i].fun);
	}
	stoSetHandler(h0);
}


void
testRun(String name, void (*fun)(void))
{
	printf("===========================================================\n");
	printf("===  Test \"%s\"\n", name);
	printf("===========================================================\n");
	
        osInit();

	testStartNetStore();

	fun();

	printf("\nTest \"%s\" completed.\n", name);
	testShowNetStore();
}
	

static long	alloc0, free0, gc0;

void
testStartNetStore(void)
{
	alloc0 = stoBytesAlloc; free0 = stoBytesFree; gc0 = stoBytesGc;
}

void
testShowNetStore(void)
{
	long allocD = stoBytesAlloc - alloc0;
	long freeD  = stoBytesFree  - free0;
	long gcD    = stoBytesGc    - gc0;

	printf("Net store allocated is %ld\n", allocD - freeD - gcD);
}


/*
 * The diagnostic suite
 */

#ifdef TEST_ALL
extern void	testShow	(void);

extern void	testCPort	(void);
extern void	testOpsys	(void);

extern void	testStore1a	(void);
extern void	testStore1b	(void);
extern void	testStore1c	(void);
extern void	testStore1d	(void);
extern void	testStore2	(void);
extern void	testStore3	(void);
	
extern void	testUtil	(void);
extern void	testFloat	(void);
extern void	testFluid	(void);
extern void	testFormat	(void);


extern void	testBigint	(void);
extern void	testBitv	(void);
extern void	testBtree	(void);
extern void	testBuffer	(void);
extern void	testDnf		(void);
extern void	testFname	(void);
extern void	testList	(void);
extern void	testPriq	(void);
extern void	testString	(void);
extern void	testSymbol	(void);
extern void	testTable	(void);
extern void	testXFloat	(void);

extern void	testCCode	(void);
extern void	testMsg		(void);
extern void	testFile	(void);

extern void	testLink	(void);
#endif

struct test suite[] = {
#ifdef TEST_ALL
   	/* name         function   */
	{"show",	testShow},

	{"cport",	testCPort},
	{"opsys",	testOpsys},

  	{"store1a",	testStore1a},
  	{"store1b",	testStore1b},
  	{"store1c",	testStore1c},
	{"store1d",	testStore1d},
  	{"store2",	testStore2},
  	{"store3",	testStore3},

   	{"util",	testUtil},
	{"float", 	testFloat},
   	{"fluid",	testFluid},
   	{"format",	testFormat},

	{"bigint",	testBigint},
	{"bitv",	testBitv},
   	{"btree",	testBtree},
	{"buffer",	testBuffer},
	{"dnf",		testDnf},
   	{"fname",	testFname},
   	{"list",	testList},
   	{"priq",	testPriq},
   	{"string",	testString},
   	{"symbol",	testSymbol},
   	{"table",	testTable},
   	{"xfloat",	testXFloat},

	{"ccode",	testCCode},
   	{"msg",		testMsg},
   	{"file",	testFile},
	{"link",	testLink},
#endif
  	{0,		0}
};

void
testShow(void)
{
	int	i;

	printf("\nThe tests are:");
	for (i = 0; suite[i].name != 0; i++) printf(" \"%s\"", suite[i].name);
	printf("\n");
}

/*****************************************************************************
 *
 * fluid_t.c: Test fluid variables.
 *
 * Copyright (c) 1990-2007 Aldor Software Organization Ltd (Aldor.org).
 *
 ****************************************************************************/

#if !defined(TEST_FLUID) && !defined(TEST_ALL)

void testFluid(void) { }

#else

#include "axlgen.h"
#include "fluid.h"

JmpBuf	testFluidBuffer;
int	testFluidI = 32;
double	testFluidF = 32.1;

void	testFluidReport			(void);
void	testFluidPeek			(String);
void	testFluidLookAndReturn		(void);
void	testFluidLookAndLongJmp		(void);
void	testFluidEventuallyLongJmp	(void);

void
testFluid(void)
{
	fluidIncrement = 1;         /* Grow slowly to catch bugs. */

	testFluidI = 42;
	testFluidF = 42.42;

	testFluidReport();

	testFluidPeek("testFluid");	      	/* 1. */
	testFluidLookAndReturn();	      	/* 2. */

	if (!SetJmp(testFluidBuffer))
		testFluidEventuallyLongJmp(); 	/* 3. */
	else
		testFluidPeek("testFluid");   	/* 4. */

	testFluidLookAndReturn();	      	/* 5. */
	testFluidPeek("testFluid");   	      	/* 6. */

	testFluidReport();
}

void
testFluidReport(void)
{
	printf("fluidLevel = %d, fluidLimit = %d\n", fluidLevel, fluidLimit);
}

void
testFluidPeek(String title)
{
	printf("%s:  The values are %d, %g\n", title, testFluidI, testFluidF);
}

void
testFluidLookAndReturn(void)
{
	Scope("Look and Return");

	int	fluid(testFluidI);
	double	fluid(testFluidF);

	testFluidI = 1001;
	testFluidF = 1001.2002;
	testFluidPeek("testFluidLookAndReturn");

	ReturnNothing;
}

void
testFluidEventuallyLongJmp(void)
{
	Scope("Eventually LongJmp");

	int	i, fluid(testFluidI), j;
	double	fluid(testFluidF);

	i = j = 0;
	testFluidI = 100 + i + j;
	testFluidF = 100.0;

	testFluidPeek("testFluidEventuallyLongJmp");

	testFluidLookAndLongJmp();

	printf("This should not be printed!!\n");
	ReturnNothing;
}

void
testFluidLookAndLongJmp(void)
{
	Scope("Look and Leap");

	int	fluid(testFluidI);
	double	fluid(testFluidF);

	testFluidI = 64;
	testFluidF = 64.2;

	testFluidPeek("testFluidLookAndLongJmp");
	LongJmp(testFluidBuffer, 1);

	printf("This should not be printed!!\n");
	ReturnNothing;
}

#endif

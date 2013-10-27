#if !defined(TEST_XFLOAT) && !defined(TEST_ALL)

void testFloat(void) { }


#else  /* defined(TEST_FLOAT) || defined(TEST_ALL) */

#include "axlgen.h"
#include "bigint.h"
#include "debug.h"
#include "foam_c.h"
#include "opsys.h"
#include "strops.h"
#include "util.h"

static FiSFlo  fv[] = {0.0f, 0.1f, 0.9f, 1.0f, 1.2f, 1.6f, 2.0f, 2.5f, 4.0f, 
	       4.990934234f, 5.8f, 9.0f, 9.9999999999999f, 100.0f,
	       100.1232402340925f, 10000.0f, 10000.5343999f, 10.0793f,
	       1e10f /* , 5e20, 3e23, 345.45454e10, 4e32 */
};

#define bintPrintDb(b) { bintPrint(osStdout, (BInt) (b)); printf("\n");}

#define ubPrintBitsDF(a)	_ubPrintBitsDF((UByte *)&(a))

void
_ubPrintBitsDF(UByte *ub)
{
	UByte	data[sizeof(FiDFlo)];

	unsigned int i;

	for (i = 0; i < sizeof(FiDFlo); i++)
		data[i] = DF_UByte(ub, i);
	ubPrintBits0(sizeof(FiDFlo), data);
}

void
testSFloInRange(FiSFlo *sf0, FiSFlo *sf1)
{
	unsigned long	count = 0;
	FiSFlo		sft = *sf0;
	FiSFlo		epsilon = fiSFloNext(sft) - sft;

	printf("Epsilon = %g\n", epsilon);

	while (sft != *sf1) {
		sft = fiSFloNext(sft);
		if (++count == 0) {
			printf("Counter overflow!\n");
			break;
		}
		if (sft > *sf1) {
			printf("Skipped the endpoint!\n");
			break;
		}
		if (count % 10000 == 0)
			printf("#");
	}

	printf("There are %lu sflo in the range.\n", count);

	printf("(s1 - s2) / epsilon = %g\n", (*sf1 - *sf0) / epsilon);

	while (sft != *sf0) {
		if (count-- == 0) printf("Counter overflow!\n");
		sft = fiSFloPrev(sft);
	}

	if (count != 0)
		printf("Reverse test failed!");
}

void
testDFloInRange(FiDFlo *df0, FiDFlo *df1)
{
	unsigned long	count = 0;
	FiDFlo		dft = *df0;
	FiDFlo		dftmp = fiDFloNext(*df0);
	FiDFlo		epsilon = fiDFloNext(dft) - dft;

	if (epsilon != fiDFloEpsilon())
		printf("Different epsilon!: %g != %g\n", 
		       epsilon, fiDFloEpsilon());

	ubPrintBitsDF(*df0);
	printf("\n");
	ubPrintBitsDF(*df1);
	printf("\n");
	ubPrintBitsDF(dftmp);
	printf("\n");

	while (dft != *df1) {
		dftmp = fiDFloNext(dft);

		if (epsilon != dftmp - dft)
			printf("%g, %g, %g, %g\n", 
			       dftmp, dft, dftmp-dft, epsilon);

		dft = dftmp;
		if (++count == 0) {
			printf("Counter overflow!\n");
			break;
		}
		if (dft > *df1) {
			printf("Skipped the endpoint!\n");
			break;
		}
	}

	ubPrintBitsDF(dft);
	printf("\n");

	printf("There are %lu dflo in the range\n", count);

	printf("(d1 - d2) / epsilon = %g\n", (*df1 - *df0) / epsilon);
}

Bool
testSFloParts(FiSFlo sf, BInt integer, FiSFlo frac)
{
	char strOrig[100];
	char strFrac[50];
	char strNew[100];
	String s, sign;

	sprintf(strOrig, "%f", sf);
	sprintf(strFrac, "%f", frac);
	s = strFrac + (frac < 0 ? 3 : 2);

	sign = "";
	if ((-1.0 < sf && sf < 0.0) || (sf == 0.0 && strOrig[0] == '-'))
		sign = "-";

	sprintf(strNew, "%s%s.%s", sign, bintToString(integer), s);

	if (strEqual(strOrig, strNew))
		return true;

	printf("testSFloParts failed: [%s] != [%s]\n", strOrig,strNew);
	return false;
}

Bool
testDFloParts(FiDFlo df, BInt integer, FiDFlo frac)
{
	char strOrig[100];
	char strFrac[50];
	char strNew[100];
	String s, sign;

	sprintf(strOrig, "%f", df);
	sprintf(strFrac, "%f", frac);
	s = strFrac + (frac < 0 ? 3 : 2);

	sign = "";
	if ((-1.0 < df && df < 0.0) || (df == 0.0 && strOrig[0] == '-'))
		sign = "-";

	sprintf(strNew, "%s%s.%s", sign, bintToString(integer), s);

	if (strEqual(strOrig, strNew))
		return true;

	printf("testDFloParts failed: [%s] != [%s]\n", strOrig,strNew);
	return false;
}

void
testPrevNext()
{
	FiSFlo	sf0, sf1;
	FiDFlo	df0, df1;

	printf("Starting prev/next test. (Please, wait!)\n");

	printf("SFlo Range [0.01, 0.010001]\n");
	sf0 = 0.01f; sf1 = 0.010001f;
	testSFloInRange(&sf0, &sf1);

	printf("SFlo Range [100.0, 100.1]\n");
	sf0 = 100.0f; sf1 = 100.1f;
	testSFloInRange(&sf0, &sf1);

	printf("SFlo Range [2.0e-38, 2.0001e-38]\n");
	sf0 = 2.0e-38f; sf1 = 2.0001e-38f;
	testSFloInRange(&sf0, &sf1);

	printf("DFlo Range [0.01, 0.010000000000001]\n");
	df0 = 0.01; df1 = 0.0100000000001;
	testDFloInRange(&df0, &df1);

	printf("DFlo Range [100.0, 100.00000000001]\n");
	df0 = 100.0; df1 = 100.000000001;
	testDFloInRange(&df0, &df1);
}

void
testRound()
{
	int   i, sign;

	printf("------------ Rounding (SFlo): ------------\n");

	for (sign = 0; sign < 2; sign++)
		for (i = 0; i < sizeof(fv) / sizeof(FiSFlo); i++) {
			FiSFlo	sf = (sign? -fv[i] : fv[i]);
			printf("Orig: %f \n", sf);
			printf(" up:\t"); 
			bintPrintDb(fiSFloRound(sf, fiRoundUp()));
			printf(" down:\t"); 
			bintPrintDb(fiSFloRound(sf, fiRoundDown()));
			printf(" zero:\t"); 
			bintPrintDb(fiSFloRound(sf, fiRoundZero()));
			printf(" nearest:\t"); 
			bintPrintDb(fiSFloRound(sf, fiRoundNearest()));
			printf(" any:\t"); 
			bintPrintDb(fiSFloRound(sf, fiRoundDontCare()));
		} 

	printf("------------ Rounding (DFlo): ------------\n");


	for (sign = 0; sign < 2; sign++)
		for (i = 0; i < sizeof(fv) / sizeof(FiSFlo); i++) {
			FiDFlo	df = (FiDFlo) (sign? -fv[i] : fv[i]);
			printf("Orig: %f \n", df);
			printf(" up:\t"); 
			bintPrintDb(fiDFloRound(df, fiRoundUp()));
			printf(" down:\t"); 
			bintPrintDb(fiDFloRound(df, fiRoundDown()));
			printf(" zero:\t"); 
			bintPrintDb(fiDFloRound(df, fiRoundZero()));
			printf(" nearest:\t"); 
			bintPrintDb(fiDFloRound(df, fiRoundNearest()));
			printf(" no care:\t"); 
			bintPrintDb(fiDFloRound(df, fiRoundDontCare()));

		} 

	printf("---------- Rounding tests finished. --------------\n");

}

void
testRangeInteractive()
{
	FiSFlo	sf0, sf1;
	FiDFlo	df0, df1;

	while (1) {

		printf("SFlo0= ");
		IgnoreResult(scanf("%f", &sf0));
		printf("SFlo1= ");
		IgnoreResult(scanf("%f", &sf1));

		testSFloInRange(&sf0, &sf1);

		printf("DFlo0= ");
		IgnoreResult(scanf("%lf", &df0));
		printf("DFlo1= ");
		IgnoreResult(scanf("%lf", &df1));

		testDFloInRange(&df0, &df1);
	}
}

void
testInteractive()
{
	FiSFlo  sfl, sfl0;
	FiDFlo  dfl, dfl0;
	FiSFlo	sfrac;
	FiDFlo  dfrac;
	BInt	bi;

	while (1) {

		printf("\nEnter a single float: ");
		IgnoreResult(scanf("%f", &sfl));

		printf("\ntrunc(%.*g) = ", 9, sfl);

		bi = (BInt) fiSFloTruncate(sfl);
		bintPrint(osStdout, bi);

		sfrac = fiSFloFraction(sfl);
		printf("\nfrac = %g \n", sfrac);

		testSFloParts(sfl, bi, sfrac);

		sfl0 = fiSFloNext(sfl);
		printf("Next: %.*g (eq = %d)\n", 9, sfl0, sfl == sfl0);

		printf("\nEnter a double float: ");
		IgnoreResult(scanf("%lf", &dfl));

		printf("\ntrunc(%.*g) = ", 50, dfl);

		bi = (BInt) fiDFloTruncate(dfl);
		bintPrint(osStdout, bi);

		dfrac = fiDFloFraction(dfl);
		printf("\nfrac = %g \n", dfrac);

		testDFloParts(dfl, bi, dfrac);

		dfl0 = fiDFloNext(dfl);
		printf("Next: %.*g (diff = %.*g)\n", 50, dfl0, 50, dfl0 - dfl);

		/* for (i = 0; i < sizeof(FiDFlo) */
	}

}

local void
testTruncate()
{
	int 	sign, i;
	Bool	stateOk = true;

	printf("Starting truncate self-test...\n");

	for (sign = 0; sign < 2; sign++)
		for (i = 0; i < sizeof(fv)/sizeof(FiSFlo); i++) {
			FiSFlo sf = (sign? -fv[i] : fv[i]);

			stateOk = stateOk && 
				testSFloParts(sf,(BInt) fiSFloTruncate(sf),
				      fiSFloFraction(sf));
		}


	for (sign = 0; sign < 2; sign++)
		for (i = 0; i < sizeof(fv)/sizeof(FiDFlo); i++) {
			FiDFlo df = (sign? -fv[i] : fv[i]);

			stateOk = stateOk &&
				testDFloParts(df,(BInt) fiDFloTruncate(df),
				      fiDFloFraction(df));
		}

	printf("Truncate self-test finished [%s].\n",
		(stateOk? "success" : "failure"));

	return;
}


/******************************************************************************
 *
 * :: Main Entry Point.
 *
 *****************************************************************************/

void
testFloat()
{
	testRound();
	testTruncate();
	testPrevNext();

	/* Use these for interactive testing. */
	/* testInteractive(); */
	/* testRangeInteractive(); */
}

#endif /* defined(TEST_FLOAT) || defined(TEST_ALL) */

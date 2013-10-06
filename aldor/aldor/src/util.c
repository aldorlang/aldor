/*****************************************************************************
 *
 * util.c: General functions used throughout.
 *
 * Copyright (c) 1990-2007 Aldor Software Organization Ltd (Aldor.org).
 *
 ****************************************************************************/

#include "axlgen0.h"
#include "opsys.h"
#include "util.h"

/*****************************************************************************
 *
 * :: Exits
 *
 ****************************************************************************/

local   void	exitDefaultHandler  (int status);

static ExitFun 	exitHandler   	= exitDefaultHandler;

void
exitSuccess(void)
{
	(*exitHandler)(EXIT_SUCCESS);
        osExit(EXIT_SUCCESS);
}

void
exitFailure(void)
{
	(*exitHandler)(EXIT_FAILURE);
        osExit(EXIT_FAILURE);
}

local void
exitDefaultHandler(int status)
{
}

ExitFun
exitSetHandler(ExitFun f)
{
	ExitFun oldf = exitHandler;
	exitHandler = f ? f : (ExitFun) exitDefaultHandler;
	return oldf;
}

/*****************************************************************************
 *
 * :: internalWarning
 *
 ****************************************************************************/

/* Display a warning msg only if in -Wcheck mode.
 * Used from audit procedures.
 * Note: a msg of this kind means that a dangerous condition has been
 *       verified, but not necessarily it's a bug.
 */
void
bugWarning(String fmt, ...)
{
	va_list argp;

	if (_dont_assert) return;   /* no -Wcheck opt. */

	printf("Internal Warning: ");
	va_start(argp, fmt); vprintf(fmt, argp); va_end(argp);
	printf("\n");

}

/*****************************************************************************
 *
 * :: Bugs
 *
 ****************************************************************************/

void
bug(String fmt, ...)
{
	va_list argp;
	osDisplayMessage("Compiler bug...");
	printf("Bug: ");
	va_start(argp, fmt); vprintf(fmt, argp); va_end(argp);
	printf("\n");

	if (_fatal_assert)
		exitFailure();
}

/*****************************************************************************
 *
 * :: Numerics
 *
 ****************************************************************************/

ULong
binPrimeArray[33] = {
	1, 2, 3, 7, 13, 31, 61, 127, 251, 509, 1021, 2039, 4093, 8191, 16381,
	32749, 65521, 131071, 262139, 524287, 1048573, 2097143, 4194301,
	8388593, 16777213, 33554393, 67108859, 134217689, 268435399,
	536870909, 1073741789, 2147483647, 4294967291u
};

ULong
binPrime(ULong nbits)
{
	return binPrimeArray[nbits];
}

int
cielLg(ULong n)
{
	register int    i;
	register ULong  p;

	for (i = 0, p = 1; ; i++, p <<= 1)
		if (n <= p) return i;
}

/*****************************************************************************
 *
 * :: Strings
 *
 ****************************************************************************/

String
bite(String buf, String str, int sep)
{
	while (*str && *str != sep) *buf++ = *str++;
	if (*str == sep) str++;
	*buf = 0;
	return str;
}

/*****************************************************************************
 *
 * :: Sorting
 *
 ****************************************************************************/

#define _pa(i)	((Pointer) (((char *) a) + (i)*sz))

void
lisort(Pointer a, Length n, Length sz, 
       int (*cmpfn)(ConstPointer, ConstPointer))
{
	Length		i, j;

	for (i = 1; i < n; i += 1)
		for (j = i; j > 0 && cmpfn(_pa(j-1), _pa(j)) > 0; j -= 1)
			memswap(_pa(j-1), _pa(j), sz);
}

#undef _pa

/*****************************************************************************
 *
 * :: Memory operations
 *
 ****************************************************************************/

void
memswap(Pointer targ, Pointer src, Length n)
{
	char *	ct = (char *) targ;
	char *	cs = (char *) src;
	char	t;
	long	d;

	/* Ensure that the memory blocks do not overlap. */
	d = ptrDiff(ct, cs);
	if (d < 0) d = -d;
	if (d < n) return;

	while (n-- > 0) { t = *ct; *ct = *cs; *cs = t; ct++; cs++; }

        return;
}

#define memchunk	32000	/* Guaranteed addressible in one segment. */

Pointer
memlset(Pointer p, int c, ULong l)
{
	char	*s = (char *) p;
	ULong	l0 = l;
	int	i;

	while (l0 >= memchunk) {
		s = (char *) ptrCanon(s);

		for (i = 0; i < memchunk; i++) *s++ = c;

		l0 -= memchunk;
	}
	if (l0 > 0) {
		s = (char *) ptrCanon(s);

		for (i = 0; i < l0; i++) *s++ = c;
	}
	assert(l == ptrDiff((char *) s, (char *) p));
	return p;
}

Bool
memltest(Pointer p, int c, ULong l)
{
	char	*s = (char *) p;
	int	i;

	while (l >= memchunk) {
		s = (char *) ptrCanon(s);

		for (i = 0; i < memchunk; i++) {
			if (*s != c) return false;
			s++;
		}
		l -= memchunk;
	}
	if (l > 0) {
		s = (char *) ptrCanon(s);

		for (i = 0; i < l; i++) {
			if (*s != c) return false;
			s++;
		}
	}
	return true;
}


/*****************************************************************************
 *
 * :: Bit-fiddling
 *
 ****************************************************************************/

/*
 * bfShiftUp
 *	 shifts bits up (toward lower address) by 'nsh' bit positions. 
 *      's' is a pointer to 'nb' bytes treated as a bit string.
 *	't' is a pointer to 'nb' bytes to hold the result.
 *	Bits shifted out are lost.  'bF' is the bit to shift in.
 *
 *      Example:   bv = 00010001 , nsh = 2, (nb = 1), bF = 1
 *              -> br = 01000111
 *
 *      Example:   bv = 00010000 , nsh = 2, (nb = 1), bF = 0
 *              -> br = 01000000
 *
 */
void	
bfShiftUp(int nb, UByte *br, int nsh, UByte *bv, int bF)
{
	int	i, ov, b, xbyte, xbit, BF;

	assert(nsh >= 0);

	BF = bF ? (1 << CHAR_BIT) - 1 : 0;

	xbyte = nsh / CHAR_BIT;
	xbit  = nsh % CHAR_BIT;

	for (i = 0; i < nb - xbyte; i++)
		br[i] = bv[i + xbyte]; 
	for (     ; i < nb; i++)
		br[i] = BF; 
	
	ov = BF;
	for (i = nb - 1; i >= 0; i--) {
		b     = bv[i];
		br[i] = (b << xbit) | ov;
		ov    = b >> (CHAR_BIT - xbit);
	}
}

/* bfShiftDn
 *	shifts bits down (toward higher address) by nsh bit positions. 
 *      's' is a pointer to 'nb' bytes treated as a bit string.
 *	't' is a pointer to 'nb' bytest to hold the result.
 *	Bits shifted out are lost. 
 *	'b1' is the first bit to shift in the top.
 *	'b0' is shifted in for the subsequent bits.
 *
 * NOTE: 'b1' (the first bit to shift) is usefull because some floating point
 *	representations have an implicit normalization bit.
 */
void	
bfShiftDn(int nb, UByte *br, int nsh, UByte *bv, int b0, int b1)
{
	int	i, ov, b, xbyte, xbit, B0, B1;

	assert(nsh >= 0);

	B0 = b0 ? (1 << CHAR_BIT) - 1 : 0;
	B1 = b1 ? B0 | 1 : B0 & ~1;

	xbyte = nsh / CHAR_BIT;
	xbit  = nsh % CHAR_BIT;

	for (i = nb - 1; i >= xbyte; i--)
		br[i] = bv[i - xbyte]; 
	for (i = 0; i < xbyte && i < nb; i++)
		br[i] = (i == xbyte-1) ? B1 : B0;

	ov = (!xbyte ? B1 : B0) << (CHAR_BIT - xbit);
	for (i = 0; i < nb; i++) {
		b     = bv[i];
		br[i] = (b >> xbit) | (ov & ((1 << CHAR_BIT) -1));
		ov    = b << (CHAR_BIT - xbit);
	}
}

/* bfFirst1
 *	finds the bit index of the lowest address 1-bit in 't'.
 *	The most significant bit of t[0] is bit 0 and the least
 *	significant bit of t[nb-1] is bit nb * CHAR_BIT - 1.
 * 
 *      Returns -1 if there is no 1-bit.
 */ 
int
bfFirst1(int nb, UByte *bv)
{
	int	xbyte, xbit;
	
	for (xbyte = 0; xbyte < nb; xbyte++)
		if (bv[xbyte]) break;

	if (xbyte == nb) return -1;
	
	for (xbit = 0; xbit < CHAR_BIT; xbit++)
		if (bv[xbyte] & (1 << (CHAR_BIT - xbit - 1))) break;

	assert(xbit < CHAR_BIT);

	return xbyte * CHAR_BIT + xbit;
}


/*****************************************************************************
 *
 * :: Input-output
 *
 ****************************************************************************/

void
prompt(FILE *fin, FILE *fout, String fmt, ...)
{
	va_list	argp;

	if (!osIsInteractive(fin)) return;

	va_start(argp, fmt);
	vfprintf(fout, fmt, argp);
	va_end(argp);

	fflush(fout);
}

int
fputcTimes(int c, int n, FILE *fout)
{
	int	i;
	for (i = 0; i < n; i++) fputc(c, fout);
	return n;
}

int
fputsUntab(String s, int tabstop, FILE *fout)
{
        int     cc;
        for (cc = 0; *s; s++) {
                if (*s != '\t') {
                        fputc(*s, fout); cc++;
                }
                else {
                        fputc(' ', fout); cc++;
                        while (cc % tabstop != 0) {
                                fputc(' ', fout); cc++;
                        }
                }
        }
	return cc;
}

Bool     cmdFloatRepFlag = false;    /* Decrease double precision -Wfloatrep */

String
DFloatSprint(String buf, DFloat d)
{
	if (cmdFloatRepFlag) {
		if (d == 0.0)
			/*ugly hack to fix output of 0.0 under windows*/
			sprintf(buf, "0.0000000000000000");
		else
			sprintf(buf, "%#.*g", DBL_DIG, d);
	} else {
#if 1
		if (d == 0.0)
			/*ugly hack to fix output of 0.0 under windows*/
			sprintf(buf, "0.0000000000000000");
		else
			sprintf(buf, "%#.*g", DBL_DIG+2, d);
#else
		sprintf(buf, "%#.*g", DBL_DIG+3, d);
#endif
	}

	return buf;
}

double
DFloatScan(String buf)
{
	return atof(buf);
}



local void
ubPrintUByteBits(UByte ub)
{
	int i;
	UByte mask = 0x80;

	for (i = 0; i < 8; i++) {
		if (ub & mask)
			putchar('1');
		else
			putchar('0');
		mask >>= 1;
		if (i == 3) putchar(',');
	}
	putchar('.');
}

/* NOTE: use the macro: ubPrintBits(x) to print the bit mask for `x'. 
 */
void
ubPrintBits0(int n, UByte *ub)
{
	int i;

	printf("[");
	for (i = 0; i < n; i++)
		ubPrintUByteBits(ub[i]);
	printf("]");
}


int
hashCombinePair(int i1, int i2)
{
	/* http://opendatastructures.org/ods-java/5_3_Hash_Codes.html */
	assert(sizeof(long) >= 8);
	long z1 = 0x419ac241;
	long z2 = 0x5577f8e1;
	long zz = 0x440badfc05072367;

        long h1 = i1 & ((1L<<32)-1);
        long h2 = i2 & ((1L<<32)-1);

        int tmp = (int)(((z1*h1 + z2*h2) * zz) >> 32);

	return tmp & 0x3FFFFFFF;
}

/* Used this for digging out random numbers:

long gcd(long a, long b)
{
	if (a < 0) a = -a;
	if (b < 0) b = -b;

	while (b != 0) {
		long t = a;
		a = b;
		b = t % b;
	}
	return a;
}


int main(int argc, char *argv[])
{
	while (1) {
		long h1 = rand();
		long h2 = rand();
		long h3 = ((long) rand()) << 32 | rand();

		int g1 = gcd(h1, h2);
		int g2 = gcd(h2, h3);
		int g3 = gcd(h1, h3);

		if (g1 + g2 + g3 == 3) {
			printf("%lx %lx %lx\n", h1, h2, h3);
		}
	}
}
*/

  /*****************************************************************************
   *
   * :: Strings
   *
   ****************************************************************************/

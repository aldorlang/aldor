/*****************************************************************************
 *
 * bigint.c: Big integer arithmetic.
 *
 * Copyright (c) 1990-2007 Aldor Software Organization Ltd (Aldor.org).
 *
 ****************************************************************************/

/*
 * Operations of the form "bintAaaa" do not modify their arguments,
 * and always allocate new values as their results. E.g., the statement
 *
 *      b1 = bintShift(b0, i);
 *
 * causes the BInt value b0 to be shifted by i bits, creating
 * a new BInt value b1.
 * These operations work on normalized numbers.
 * I.e. When a number is small enough, it is represented immediately.
 * Otherwise it is stored and Placec(b) > 0 && Placev(b)[Placec(b)-1] != 0.
 *
 * Operations of the form "xintAaaa" can modify their arguments or,
 * if necessary, reallocate and free their input.
 *
 * Operations of the form "iintAaaa" do no storage allocation whatsoever
 * and perform the big integer arithmetic algorithms.
 */

#include "axlgen.h"
#include "bigint.h"
#include "debug.h"
#include "store.h"
#include "util.h"
#include "compopt.h"

/*
 * The following symbols are used to conditionalize code in this file:
 *
 * BIGINT_DO_DEBUG	  -- Turns on DEBUG(bint) blocks.
 * BIGINT_DEBUG_IMMED	  -- Turns on (bad) code for generalized ImmedIfCan.
 * BIGINT_SHORT_IMMED	  -- Defines IInt as "short" rather than "int".
 * BIGINT_USE_WHOLE_WORDS -- Turns on (incomplete) code for long bint digits.
 * BIGINT_SHHH		  -- Turns off all debugging code.
 *
 */


/*
 * Define BIGINT_DO_DEBUG to display debug info.
 */
#ifdef BIGINT_DO_DEBUG
# define bintDebug	true
#else
# define bintDebug	false
#endif

#define bintDEBUG	DEBUG_IF(bint)	fprintf

#ifdef FOAM_RTS
# define dbOut stdout
#endif

/*****************************************************************************
 *
 * :: Remove linkage to String (and hence format.c etc)
 *
 ****************************************************************************/

local String
localStrAlloc(Length n)
{
	String  s;
	s = (String) stoAlloc((unsigned) OB_String, n + 1);
	s[0] = 0;
	s[n] = 0;
	return s;
}

local void
localStrFree(String s)
{
	stoFree((Pointer) s);
}

/*****************************************************************************
 *
 * :: Integer range defintitions
 *
 *    Should work for 2's complement n-bit ints, for even n >= 4.
 *
 *****************************************************************************/


#ifdef BIGINT_SHORT_IMMED
typedef short		IInt;
typedef UShort		UIInt;
#else
typedef long		IInt;
typedef unsigned long	UIInt;
#endif


/*
 * Conversion between ints and immediate BInts.
 */
#define IsImmed(b)	((long)(b) &  1)
#define MkImmed(n)	((((long)(n)) << 1) | 1)
#define UnImmed(n)	((n) >> 1)

#define BIntToInt(b) 	((IInt) UnImmed(ptrToLong(b)))
#define IntToBInt(n)	((BInt) ptrFrLong(MkImmed(n)))
#define BIntZero	((BInt) MkImmed(0))
#define BIntOne		((BInt) MkImmed(1))

/*
 * Fields of stored BInts.
 */
#define Beq(a,b)	((a) == (b))
#define IsNeg(b)	((b)->isNeg)
#define Placec(b)	((b)->placec)
#define Placea(b)	((b)->placea)
#define Placev(b)	((b)->placev)

/*
 * The minimum and maximum values of type int.
 */
#ifndef INT_MIN
# define INT_MIN	((IInt) ((-1)<<(bitsizeof(IInt)-1)))
# define INT_MAX	(- (INT_MIN + 1))		  /* 2's complement */
#endif

/*
 * These are the min + max int values which are represented as immediate BInts.
 * These must satisfy BIntToInt(IntToBInt(n)) == n,
 *   hence the "-2" in INT_MIN_IMMED.
 */
#define INT_LG_IMMED	(bitsizeof(IInt)-2)
#define INT_MIN_IMMED	( (IInt) ((((long) -1)<<INT_LG_IMMED)+1)) /*+1 => symmetric */
#define INT_MAX_IMMED	(- (long) INT_MIN_IMMED)
#define INT_IS_IMMED(n)	(INT_MIN_IMMED<= (long)(n) && (long)(n)<= INT_MAX_IMMED)

#define INT_LG_HALF	((bitsizeof(IInt)-2)/2)
#define INT_MIN_HALF	( (IInt) ((((long) -1)<<INT_LG_HALF)+1))
#define INT_MAX_HALF	(- (long) INT_MIN_HALF)
#define INT_IS_HALF(n)	(INT_MIN_HALF <= (long)(n) && (long)(n)<= INT_MAX_HALF)

/*
 * Calculation radix.
 * Must be possible to detect overflow on addition and implement double word
 *   multiply and divide.
 */

#ifdef BIGINT_DO_DEBUG
# define BINT_LG_RADIX		7
#else
# define BINT_LG_RADIX		(bitsizeof(BIntS))
#endif
#define BINT_RADIX		(((unsigned long) 1) << BINT_LG_RADIX)
					/* Might not be representable! */
#define BINT_RADIX_MASK		(BINT_RADIX - 1)
#define BINT_RADIX_MINUS_1	(BINT_RADIX - 1)

/*****************************************************************************
 *
 * :: Double precision arithmetic macros
 *
 *****************************************************************************/

typedef ULong	BIntD;		/* To contain two BInt digits. */

#define TestGTDouble(b, h1, l1, h2, l2) {		\
	BIntS h1_ = (h1), h2_ = (h2);			\
	(b) = (h1_ > h2_) || (h1_ == h2_ && (l1)>(l2));	\
}

#define TimesDouble(h, l, a, b) {			\
	BIntD t_ = (BIntD)(a)*(BIntD)(b);		\
	(h) = t_ / BINT_RADIX;				\
	(l) = t_ % BINT_RADIX;				\
}

#define DivideDouble(q, r, nh, nl, d) {			\
	BIntD n_ = (BIntD)(nh)*(BIntD)BINT_RADIX + (nl);\
	BIntD d_ = (d);					\
	(q)      = n_ / d_;				\
	(r)	 = n_ % d_;				\
}

#define PlusStep(kout, r, a, b, kin) {			\
	BIntD	r_ = (BIntD)(a)+(BIntD)(b)+(BIntD)(kin);\
	BIntS	k_ = (r_ >= (BIntD) BINT_RADIX);	\
	if (k_) r_ -= BINT_RADIX; 			\
	(r)    = r_;					\
	(kout) = k_;					\
}

#define MinusStep(kp1out, r, a, b, kp1in)  		\
	PlusStep(kp1out, r, a, (BINT_RADIX_MINUS_1 - (b)), kp1in)

#define TimesStep(kout, r, a, b, c, kin) {		\
	BIntD	t_ = (BIntD)(a)*(BIntD)(b)+(BIntD)(c)+(BIntD)(kin);	\
	(r)    = t_ % BINT_RADIX;			\
	(kout) = t_ / BINT_RADIX;			\
}



/*****************************************************************************
 *
 * :: Related machine integer operations
 *
 *****************************************************************************/

/*
 * The length of n, in bits. I.e. cieling(lg(abs(n))).
 * Define the bit length of zero to be one bit.
 */
Length
uintLength(unsigned long u)
{
	register Length i;
	register UIInt  p;

	/* Eventually the unsigned quantity p is bigger than u = abs(n). */
	/* At each stage of the loop, any u < p can fit in i bits. */
	for (i = 1, p = 2; ; i += 1, p <<= 1)
		if (!p || u < p) break;
	return i;
}

Length
intLength(long n)
{
	return uintLength((n < 0) ? -n : n);
}


Bool
uintBit(unsigned long si, Length ix)
{
	return (ix < bitsizeof(UIInt)) && ( ((ULong)si) & (1L<<ix)) != 0;
}

Bool
intBit(long si, Length ix)
{
	return uintBit((si < 0) ? -si : si, ix);
}

/****************************************************************************
 *
 * :: Input/output
 *
 ****************************************************************************/

int
bintPrint(FILE *fout, BInt b)
{
	int	cc;
	String	s;

	if (IsImmed(b)) return fprintf(fout, "%ld", BIntToInt(b));

	s  = bintToString(b);
	cc = fprintf(fout, "%s", s);
	localStrFree(s);

	return cc;
}

void
bintPrintDb(BInt b)
{
	bintPrint(dbOut, b);
	fprintf(dbOut, "\n");
}

int
bintPrint16(FILE *fout, BInt b)
{
	int	i, cc;

	if (IsImmed(b)) return fprintf(fout, "0x%lx", BIntToInt(b));

	cc  = fprintf(fout, IsNeg(b) ? "-" : "+");
	cc += fprintf(fout, "0x");

	for (i = Placec(b) - 1; i >= 0; i--)
		cc += fprintf(fout, "%0*x.",
			      (int) QUO_ROUND_UP(BINT_LG_RADIX,4),
			      Placev(b)[i]);

	return cc;
}

/*
 * +0b[3/4]10,10100,00110.
 *
 * [3/4] -- 3 of 4 allocated places are used
 */

int
bintPrint2(FILE *fout, BInt b)
{
	int	i, n, cc = 0;

	if (bintIsNeg(b)) cc += fprintf(fout, "-");

	cc += fprintf(fout, "0b");

	if (IsImmed(b))
		cc += fprintf(fout, "[0/0]");
	else
		cc += fprintf(fout, "[%d/%d]", (int) Placec(b), (int) Placea(b));

	n = bintLength(b);

	for (i = n-1; i >= 0; i--)
		cc += fprintf(fout, bintBit(b, i) ? "1" : "0");

	return cc;
}

/*
 * Overestimate the size of a base 10 string representation for b,
 * by dividing by 3 (not log(10)/log(2) > 3.32).
 * +3   for signs, \0, etc.
 * +dio for temp leading zeros.
 */

int
bintStringSize(BInt b)
{
	IInt	dio;
	ULong	rio;

	if (IsImmed(b)) return INT_LG_IMMED;

	/*
	 * Determine power of output radix for divisions.
	 * This is the largest power of 10 < BINT_RADIX.
	 */
	for (rio = 10, dio = 1; 10*rio <= BINT_RADIX; rio *= 10, dio++)
		;
	return bintLength(b)/3+3+dio;
}


/*
 * Compute a base 10 string representation of b.
 * The string representation includes a terminating \0 character.
 */
String
bintIntoString(String s, BInt b)
{
	Bool	isNeg;
	IInt	digEst, i, j, dio;
	ULong	rio;
	BIntS	r;

	if (IsImmed(b)) {
		sprintf(s, "%ld", BIntToInt(b));
		return s;
	}

	/*
	 * Determine power of output radix for divisions.
	 * This is the largest power of 10 < BINT_RADIX.
	 */
	for (rio = 10, dio = 1; 10*rio <= BINT_RADIX; rio *= 10, dio++)
		;

	/*
	 * Make copy for in-place divisions.
	 */
	b      = bintCopy(b);
	isNeg  = IsNeg(b);

	digEst = bintStringSize(b);
	i      = digEst;
	s[--i] = 0;

	while (Placec(b) > 0) {
		iintDivideS(b, &r, b, (BIntS) rio);
		for (j = 0; j < dio; j++) {
			s[--i] = '0' + r % 10;
			r /= 10;
		}
	}

	while (s[i] == '0') i++;	/* Cd have leading zeros. */
	if (s[i] == 0) s[--i] = '0';	/* Ensure at least one digit. */
	if (isNeg) s[--i] = '-';	/* Put on sign. */

	/*
	 * Move text to the beginning of the string.
	 */
	assert(i >= 0);
	if (i > 0) for (j = 0; i < digEst; i++, j++) s[j] = s[i];

	bintFree(b);

	return s;
}


/*
 * Compute a base 10 string representation of b.
 * The string representation includes a terminating \0 character.
 */
String
bintToString(BInt b)
{
	IInt	digEst = bintStringSize(b);
	String	s      = localStrAlloc(digEst);

	bintIntoString(s, b);

	return s;
}

/*
 * Convert from the textual representation of an Aldor number into a big
 * integer. This function may be invoked (indirectly) by convert()$Machine
 * and cannot assume base 10.
 *
 * General Aldor integer format is RRrWW
 *
 *    RR  is the radix part (2-36)
 *    'r' is the radix character.
 *    WW  is the whole part ([0-9A-Z]+).
 */
BInt
bintFrString(String s)
{
	return bintRadixScanFrString(s, NULL);
}

#define fiRadixChar	('r')
#define fiPlusChar	('+')
#define fiMinusChar	('-')

/* Scan a big integer with possible radix specification */
BInt
bintRadixScanFrString(String num, String *pEnd)
{
	Bool	isNeg = false;
	String	rpos = (String)NULL;
	String	end;
	long	radix = 10;
	BInt	retval;
	Length	rlen, ndigs, bpd;
	IInt	dim, dio, ires;
	IInt	l, l0, n;
	ULong	rim, rio, nbits;
	ULong	maxi;


	/* Skip over leading whitespace */
	while (isspace(*num))
		num++;


	/* Deal with the sign (if any) */
	if ((*num == fiPlusChar) || (*num == fiMinusChar))
	{
		isNeg = (*num == fiMinusChar) ? true : false;
		num++;
	}


	/*
	 * Locate the end of the string: start by skipping over
	 * the leading part (which must be in base 10). This
	 * might be the radix or it might be the whole part.
	 */
	for (end = num; isdigit(*end); end++);


	/* Is there a radix marker? */
	if (*end == fiRadixChar)
	{
		/* Yes - note its position */
		rpos = end;
		end++;


		/* Skip past the whole part */
		for (;isdigit(*end) || isupper(*end);end++);
	}
	else
	{
		/* NOOOOOOOOOOOOOOO!!!!!!!!!!!!!! */
		/* Skip past the decimal whole part */
		for (;isdigit(*end);end++);
	}


	/* Did the caller want to know where the number ends? */
	if (pEnd) *pEnd = end;
	

	/* There must be something before the radix marker */
	if (rpos == num)
		return IntToBInt((long) 0); /* Bad number */


	/* Extract the radix (if any) */
	if (rpos)
	{
		/* Extract the radix as a (small) decimal integer */
		rlen  = rpos - num;
		radix = ulongSmallIntFrString(num, rlen, 10);


		/* Valid radix: 2-36 inclusive */
		if (errno || (radix < 2) || (radix > 36))
			return IntToBInt((long) 0);


		/* Move to the start of the whole part */
		num = rpos + 1;
	}


	/*
	 * Determine the largest power `rim' of the input radix that
	 * can be used for immediate representation. We want `rim' such
	 * that (INT_MAX_IMMED/radix < rim <= INT_MAX_IMMED). We can
	 * not check whether (radix*rim <= INT_MAX_IMMED) because the
	 * LHS may overflow for large `radix'. Instead we `rim' such
	 * that (INT_MAX_IMMED/(radix^2) < rim <= INT_MAX_IMMED/radix).
	 * This means that for all radix, (radix*rim <= INT_MAX_IMMED).
	 */
	maxi = INT_MAX_IMMED / radix;
	for (
		rim = radix, dim = 1;
		radix*rim <= maxi;
		rim *= radix, dim++
	);


	/* Now satisfy (INT_MAX_IMMED/radix < rim <= INT_MAX_IMMED) */
	rim *= radix, dim++;


	/*
	 * Determine the largest power `rio' of input radix for
	 * multiplications s.t. (BINT_RADIX/radix <= rio < BINT_RADIX).
	 */
	maxi = BINT_RADIX / radix;
	for (
		rio = radix, dio = 1;
		radix*rio < maxi;
		rio *= radix, dio++
	);


	/* Now satisfy (BINT_RADIX/radix <= rio < BINT_RADIX) */
	rio *= radix, dio++;


	/*
	 * Convert the whole part using the specified base. First we
	 * need to estimate the number of bits needed for the result.
	 */
	bpd   = (ULong)(log((double)radix)/log(2.0)) + 1;
	ndigs = (Length)(end - num);
	nbits = ndigs*bpd;


	/* If safe, compute as a C integer. */
	if (nbits <= INT_LG_IMMED)
	{
		/* Scan the small immediate integer */
		ires = ulongSmallIntFrString(num, ndigs, radix);


		/*
		 * Return the immediate result. If the number
		 * contained invalid characters then the result
		 * will be invalid. Check errno if required.
		 */
		if (isNeg) ires = -ires;
		return IntToBInt(ires);
	}


	/* Allocate enough store for the result */
	retval = bintAlloc(nbits);


	/*
	 * Convert radix by interpreting string as a poly in radix
	 * `rio' and evaluating using Horner's rule. This means that
	 * we convert the number in chunks that will fit into an
	 * immediate integer. Start with a chunk big enough to ensure
	 * that the remaining chunks are all `dio' digits long.
	 */
	l0 = ndigs % dio;
	for (n = 0, l = l0; l > 0; l--)
	{
		IInt dig;

		dig = (*num <= '9') ? (*num - '0') : (*num - 'A') + 10;
		n   = radix*n + dig;
		num++;
	}
	retval = xintCopyInI(retval, n);


	/* Now deal with the other chunks */
	for (ndigs -= l0; ndigs > 0; ndigs -= dio)
	{
		for (n = 0, l = dio; l > 0; l--)
		{
			IInt dig;

			dig = (*num <= '9') ? (*num - '0') : (*num - 'A') + 10;
			n   = radix*n + dig;
			num++;
		}
		iintTimesPlusS(retval, retval, (BIntS) rio, (BIntS) n);
	}


	/* Deal with any negation */
	IsNeg(retval) = isNeg;


	/* Try to make it an immediate integer is possible */
	return xintImmedIfCan(retval);
}


/* Convert a base 10 string representation to a big integer. */
BInt
bintScanFrString(String s, String *end)
{
	String	t;
	BInt	b;
	Bool	isNeg = false;
	Length	ndig;
	IInt	dim, dio;
	ULong	rim, rio;
	IInt	l, l0, n;

	/*
	 * Determine rim: power of input radix for immediate representation.
	 *   This is the largest power of 10 <= INT_MAX_IMMED.
	 * Determine rio: power of input radix for multiplications.
	 *   This is the largest power of 10 < BINT_RADIX.
	 */
	for (rim = 10, dim = 1; 10*rim <= INT_MAX_IMMED; rim *= 10, dim++)
		;
        /*
	 * BUG -------------------------+
         *                              V
	 */
	for (rio = 10, dio = 1; 10*rio <= BINT_RADIX;    rio *= 10, dio++)
		;

	/*
	 * Find beginning and end of number.
	 * Don't use strlen, since there may be trailing text.
	 */
	while (isspace(*s))
		s++;
	if (*s == '-') {
		isNeg = true;
		s++;
	}
	while (*s == '0')
		s++;
	for (t = s; isdigit(*t); t++)
		;
	ndig = t-s;

	if (end) *end = t;
	/*
	 * If safe, compute as a C integer.
	 */
	if (ndig <= dim) {
		for (n = 0; s != t; s++) n = 10*n + *s - '0';
		if (isNeg) n = -n;
		return IntToBInt(n);
	}

	/*
	 * Over-estimate number of bits by multiplying by 4, rather than
	 * log(10)/log(2) < 3.322.
	 */
	b = bintAlloc(4*ndig);

	/*
	 * Convert radix by interpreting string as a poly in radix "rio"
	 * and evaluating using Horner's rule.
	 */
	l0 = ndig % dio;
	for (n = 0, l = l0; l > 0; l--) n = 10*n + *s++ - '0';
	b = xintCopyInI(b, n);

	for (ndig -= l0; ndig > 0; ndig -= dio) {
		for (n = 0, l = dio; l > 0; l--) n = 10*n + *s++ - '0';
		iintTimesPlusS(b, b, (BIntS) rio, (BIntS) n);
	}

	IsNeg(b) = isNeg;

	b = xintImmedIfCan(b);
	
	return b;
}


/****************************************************************************
 *
 * :: Basic Allocation
 *
 ****************************************************************************/

BInt
bintAllocPlaces(Length placea)
{
	BInt 	b;
	b = (BInt) stoAlloc(OB_BInt, fullsizeof(*b, placea, Placev(b)[0]));
	Placea(b) = placea;
	Placec(b) = placea;
	IsNeg(b)  = false;

	return b;
}

BInt
bintAlloc(Length bitc)
{
	return bintAllocPlaces(QUO_ROUND_UP(bitc, BINT_LG_RADIX));
}

void
bintFree(BInt b)
{
	if (IsImmed(b)) return;

	stoFree((Pointer) b);
}

BInt
bintNew(long n)
{
	if (INT_IS_IMMED(n)) return IntToBInt(n);

	return xintStoreI(n);
}

BInt
bintCopy(BInt bo)
{
	BInt	bn;
	Length	i, c;

	if (IsImmed(bo)) return bo;

	c  = Placec(bo);
	bn = bintAllocPlaces(c);

	for (i = 0; i < c; i++) Placev(bn)[i] = Placev(bo)[i];
	IsNeg(bn) = IsNeg(bo);

	return bn;
}

BInt
bintFrPlacev(Bool isNeg, Length placec, BIntS *data)
{
	BInt	bint;
	int	i;
	
	bint = bintAllocPlaces(placec);
	IsNeg(bint) = isNeg;

	for (i = placec - 1; i >= 0 && data[i] == 0; i--) ;

	Placec(bint) = i+1;

	for (; i >= 0; i--)
		Placev(bint)[i] = data[i];
	return xintImmedIfCan(bint);
}

#if (U16sPerUNotAsLong == 1)
BInt
bintFrPlacevS(Bool isNeg, Length placec, U16 *data)
{
	return bintFrPlacev(isNeg, placec, (BIntS*) data);
}

void
bintToPlacevS(BInt b, int *psize, U16 **pdata)
{
	*psize = b->placec;
	*pdata = b->placev;
}

void
bintReleasePlacevS(U16 *pdata)
{
}
#endif

#if (U16sPerUNotAsLong == 2)
BInt
bintFrPlacevS(Bool isNeg, Length placec, U16 *data)
{
	BInt  bint;
	BIntS *bint_data;  
	int   newc, i;

	if (placec & 1)
		bint_data = (BIntS*) data;
	else
		bint_data = (BIntS*) stoAlloc(OB_Other, placec + 1);
	newc = (placec+1)/2;
	for (i = 0; i < newc-1; i++) 
		bint_data[i] = data[2*i] + (data[2*i+1] << bitsizeof(U16));
	bint_data[i] = (placec & 1) ? 
		data[2*i] :
		data[2*i] + (data[2*i+1] << bitsizeof(U16));
	
	bint = bintFrPlacev(isNeg, newc, bint_data);
	
	if (!(placec & 1))
		stoFree(bint_data);
	return bint;
}

void
bintToPlacevS(BInt b, int *psize, U16 **pdata)
{
	BInt   oldb;
	U16    *data;	
	int    placec;
	int    i;
	
	oldb = b;
	b = xintStore(b);
	placec = b->placec;

	data = (U16*) stoAlloc(OB_Other, 2*placec*sizeof(U16));
	
	for (i=0; i < placec; i++) {
		data[2*i]   =  b->placev[i] & 0x0000FFFF;
		data[2*i+1] = (b->placev[i] & 0xFFFF0000) >> 16;
	}
	
	if (data[2*placec - 1] == 0)
		*psize = 2*placec - 1;
	else 
		*psize = 2*placec;
	*pdata = data;

	if (oldb != b) 
		bintFree(b);
}

void
bintReleasePlacevS(U16 *data)
{
	stoFree(data);
}
#endif

/****************************************************************************
 *
 * :: Small integers
 *
 ****************************************************************************/

Bool
bintIsSmall(BInt b)
{
	return IsImmed(b);
}

long
bintSmall(BInt b)
{
	return BIntToInt(b);
}


/****************************************************************************
 *
 * :: General arithmetic
 *
 ****************************************************************************/
local BInt 	bintModi	(BInt, unsigned long);
local ULong 	bintToULong	(BInt);

/*
 * Special values
 */
BInt	bint0 = BIntZero;
BInt	bint1 = BIntOne;

/*
 * b < 0 ?
 */
Bool
bintIsNeg(BInt b)
{
	if (IsImmed(b))	return BIntToInt(b) < 0;

	return IsNeg(b);
}

/*
 * b == 0 ?
 */
Bool
bintIsZero(BInt b)
{
	return IsImmed(b) && BIntToInt(b) == 0;
}

/*
 * b > 0 ?
 */
Bool
bintIsPos(BInt b)
{
	if (IsImmed(b))	return BIntToInt(b) > 0;

	return Placec(b) > 0 && !IsNeg(b);
}

/*
 * a == b ?
 */
Bool
bintEQ(BInt a, BInt b)
{
	Bool	aImmed, bImmed;
	IInt	i, c;

	if (Beq(a,b)) return true;

	aImmed = IsImmed(a);
	bImmed = IsImmed(b);
	
	if (aImmed    && bImmed)    return BIntToInt(a) == BIntToInt(b);
	if (aImmed    != bImmed)    return false;
	if (IsNeg(a)  != IsNeg(b))  return false;
	if (Placec(a) != Placec(b)) return false;

	c = Placec(a);

	for (i = 0; i < c; i++)
		if (Placev(a)[i] != Placev(b)[i]) return false;
	return true;
}

/*
 * a < b ?
 */
Bool
bintLT(BInt a, BInt b)
{
	Bool	aImmed, bImmed;
	IInt	i, c;

	if (Beq(a,b)) return false;

	aImmed = IsImmed(a);
	bImmed = IsImmed(b);
	
	if (aImmed && bImmed)      return BIntToInt(a) < BIntToInt(b);
	if (aImmed)	           return !IsNeg(b);
	if (bImmed)	           return IsNeg(a);
	if (IsNeg(a) != IsNeg(b))  return IsNeg(a) && !IsNeg(b);

	if (IsNeg(a)) {
		if (Placec(a)!= Placec(b)) return Placec(a) > Placec(b);
		c = Placec(a);
		for (i = c-1; i >= 0; i--) {
			if (Placev(a)[i] == Placev(b)[i]) continue;
			return Placev(a)[i] > Placev(b)[i];
		}
		return false;
	}
	else {
		if (Placec(a)!= Placec(b)) return Placec(a) < Placec(b);
		c = Placec(a);
		for (i = c-1; i >= 0; i--) {
			if (Placev(a)[i] == Placev(b)[i]) continue;
			return Placev(a)[i] < Placev(b)[i];
		}
		return false;
	}
}

/*
 * a > b ?
 */
Bool
bintGT(BInt a, BInt b)
{
	Bool	aImmed, bImmed;
	IInt	i, c;

	if (Beq(a,b)) return false;

	aImmed = IsImmed(a);
	bImmed = IsImmed(b);
	
	if (aImmed && bImmed)      return BIntToInt(a) > BIntToInt(b);
	if (aImmed)	           return IsNeg(b);
	if (bImmed)	           return !IsNeg(a);
	if (IsNeg(a) != IsNeg(b))  return !IsNeg(a) && IsNeg(b);

	if (IsNeg(a)) {
		if (Placec(a)!= Placec(b)) return Placec(a) < Placec(b);
		c = Placec(a);
		for (i = c-1; i >= 0; i--) {
			if (Placev(a)[i] == Placev(b)[i]) continue;
			return Placev(a)[i] < Placev(b)[i];
		}
		return false;
	}
	else {
		if (Placec(a)!= Placec(b)) return Placec(a) > Placec(b);
		c = Placec(a);
		for (i = c-1; i >= 0; i--) {
			if (Placev(a)[i] == Placev(b)[i]) continue;
			return Placev(a)[i] > Placev(b)[i];
		}
		return false;
	}
}

/*
 * Form a new value equal to the absolute value of a.
 */
BInt
bintAbs(BInt a)
{
	BInt	r;

	if (IsImmed(a)) {
		r = (BIntToInt(a) < 0) ? bintNew(-BIntToInt(a)) : a;
	}
	else {
		r = bintCopy(a);
		IsNeg(r) = false;
	}
	return r;
}

/*
 * Form a new value equal to -a.
 */
BInt
bintNegate(BInt a)
{
	BInt 	r;

	if (IsImmed(a)) return IntToBInt(-BIntToInt(a));

	r = bintCopy(a);
	IsNeg(r) = !IsNeg(r);
	return r;
}

#define BINT_NEGATE(r) \
  { if (IsImmed(r)) (r) = IntToBInt(-BIntToInt(r)); else IsNeg(r) = !IsNeg(r); }

/*
 * Add a and b to produce a newly allocated result.
 */
BInt
bintPlus(BInt a, BInt b)
{
	BInt	r;
	Bool	aImmed, bImmed, rImmed, aNeg, bNeg;
	IInt	abitc, bbitc, rbitc;

	/* Small integer case. */
	aImmed = IsImmed(a);
	bImmed = IsImmed(b);

	if (aImmed && bImmed) {
		IInt     ai = BIntToInt(a);
		IInt	 bi = BIntToInt(b);
		IInt	 ki, ri;
		PlusStep(ki,ri, ai, bi, int0);
		if (ki == 0 && INT_IS_IMMED(ri)) return IntToBInt(ri);
	}

	if (aImmed) a = xintStore(a);
	if (bImmed) b = xintStore(b);

	aNeg   = bintIsNeg(a);
	bNeg   = bintIsNeg(b);

	if (aNeg && bNeg) {
		BINT_NEGATE(a); if (!Beq(a,b)) BINT_NEGATE(b);
		r = bintPlus(a, b);
		BINT_NEGATE(a); if (!Beq(a,b)) BINT_NEGATE(b);
		BINT_NEGATE(r);
	}
	else if (aNeg) {
		BINT_NEGATE(a);
		r = bintMinus(b, a);
		BINT_NEGATE(a);
	}
	else if (bNeg) {
		BINT_NEGATE(b);
		r = bintMinus(a, b);
		BINT_NEGATE(b);
	}
	else {
		/* General case. */
		abitc = bintLength(a);
		bbitc = bintLength(b);

		if (abitc < bbitc) {
			r = a; rbitc = abitc; rImmed = aImmed;
			a = b; abitc = bbitc; aImmed = bImmed;
			b = r; bbitc = rbitc; bImmed = rImmed;
		}

		rbitc = abitc + 1; /* +1 for potential carry */

		r = bintAlloc(rbitc);

		iintPlus(r, a, b);

		r = xintImmedIfCan(r);
	}

	if (aImmed) bintFree(a);
	if (bImmed) bintFree(b);
	return r;
}

/*
 * Subtract b from a to produce a newly allocated result.
 */
BInt
bintMinus(BInt a, BInt b)
{
	BInt	r;
	Bool	aImmed, bImmed, rImmed, aNeg, bNeg, rNeg;

	/* Small integer case. */
	aImmed = IsImmed(a);
	bImmed = IsImmed(b);

	if (aImmed && bImmed) {
		IInt     ai = BIntToInt(a);
		IInt	 bi = BIntToInt(b);
		IInt	 ri = ai - bi;
		if (INT_IS_IMMED(ri)) return IntToBInt(ri);
	}

	if (aImmed) a = xintStore(a);
	if (bImmed) b = xintStore(b);

	aNeg   = bintIsNeg(a);
	bNeg   = bintIsNeg(b);

	if (aNeg && bNeg) {
		BINT_NEGATE(a); if (!Beq(a,b)) BINT_NEGATE(b);
		r = bintMinus(b, a);
		BINT_NEGATE(a); if (!Beq(a,b)) BINT_NEGATE(b);
	}
	else if (aNeg) {
		BINT_NEGATE(a);
		r = bintPlus(a, b);
		BINT_NEGATE(a);
		BINT_NEGATE(r);
	}
	else if (bNeg) {
		BINT_NEGATE(b);
		r = bintPlus(a, b);
		BINT_NEGATE(b);
	}
	else {
		/* General case. */

		/* If a < b, compute -(b-a). */
		rNeg = bintLT(a, b);
		if (rNeg) {
			r = a; rImmed = aImmed;
			a = b; aImmed = bImmed;
			b = r; bImmed = rImmed;
		}

		r = bintAllocPlaces(Placec(a));

		iintMinus(r, a, b);
		IsNeg(r) = rNeg;

		r = xintImmedIfCan(r);
	}

	if (aImmed) bintFree(a);
	if (bImmed) bintFree(b);

	return r;
}

/*
 * Multiply a and b to produce a newly allocated result.
 */
BInt
bintTimes(BInt a, BInt b)
{
	BInt	r;
	Bool	aImmed, bImmed, aNeg, bNeg;
	IInt	ac, bc, rc;

	/* Small integer case. */
	aImmed = IsImmed(a);
	bImmed = IsImmed(b);

	if (aImmed && bImmed) {
		IInt     ai = BIntToInt(a);
		IInt	 bi = BIntToInt(b);
		if (INT_IS_HALF(ai) && INT_IS_HALF(bi)) return bintNew(ai*bi);
	}
	if (aImmed) {
		IInt      ai = BIntToInt(a);
		if (ai == 0)	return IntToBInt(int0);
		if (ai == 1)	return bintCopy(b);
		if (ai ==-1)	return bintNegate(b);
	}
	if (bImmed) {
		IInt      bi = BIntToInt(b);
		if (bi == 0)	return IntToBInt(int0);
		if (bi == 1)	return bintCopy(a);
		if (bi ==-1)	return bintNegate(a);
	}

	if (aImmed) a = xintStore(a);
	if (bImmed) b = xintStore(b);

	aNeg   = bintIsNeg(a);
	bNeg   = bintIsNeg(b);

	if (aNeg && bNeg) {
		BINT_NEGATE(a); if (!Beq(a,b)) BINT_NEGATE(b);
		r = bintTimes(a, b);
		BINT_NEGATE(a); if (!Beq(a,b)) BINT_NEGATE(b);
	}
	else if (aNeg) {
		BINT_NEGATE(a);
		r = bintTimes(a, b);
		BINT_NEGATE(a);
		BINT_NEGATE(r);
	}
	else if (bNeg) {
		BINT_NEGATE(b);
		r = bintTimes(a, b);
		BINT_NEGATE(b);
		BINT_NEGATE(r);
	}
	else {
		/* General case. */

		ac = Placec(a);
		bc = Placec(b);
		rc = ac + bc;

		r = bintAllocPlaces(rc);

		iintTimes(r, a, b);

		r = xintImmedIfCan(r);
	}

	if (aImmed) bintFree(a);
	if (bImmed) bintFree(b);

	return r;
}


/*
 * Divide a by b to produce a newly allocated quotient as the result.
 * If the pointer pr != 0, then a newly allocated remainder is returned via it.
 */
BInt
bintDivide(BInt *pr, BInt a, BInt b)
{
	BInt	q, r;
	Bool	aImmed, bImmed, aNeg, bNeg;
	IInt	n, m;

	/* Small integer case not yet handled. */
	aImmed = IsImmed(a);
	bImmed = IsImmed(b);

	if (aImmed) a = xintStore(a);
	if (bImmed) b = xintStore(b);

	aNeg   = bintIsNeg(a);
	bNeg   = bintIsNeg(b);

	if (aNeg && bNeg) {
		/* (q,r) = divide(-a,-b);  if (r != 0) {q++; r = b-r;} */
		BINT_NEGATE(a); if (!Beq(a,b)) BINT_NEGATE(b);
		q = bintDivide(&r, a, b);
		BINT_NEGATE(a); if (!Beq(a,b)) BINT_NEGATE(b);

		BINT_NEGATE(r);
	}
	else if (aNeg) {
		/* (q,r) = divide(-a,b); q = -q-1; r = b-r; */
		BINT_NEGATE(a);
		q = bintDivide(&r, a, b);
		BINT_NEGATE(a);

		BINT_NEGATE(q);
		BINT_NEGATE(r);
	}
	else if (bNeg) {
		/* (q,r) = divide(a,-b); q = -q; */
		BINT_NEGATE(b);
		q = bintDivide(&r, a, b);
		BINT_NEGATE(b);

		BINT_NEGATE(q);
	}
	else {
		/* General case. */

		n = Placec(b);
		m = Placec(a) - n; if (m < 0) m = 0;

		q = bintAllocPlaces(m+1);
		r = bintAllocPlaces(n+m+1);

		iintDivide(q, r, a, b);	/*!! Should shrink r here. */

		q = xintImmedIfCan(q);
		r = xintImmedIfCan(r);
	}

	if (aImmed) bintFree(a);
	if (bImmed) bintFree(b);

	if (pr) *pr = r; else bintFree(r);
	return q;
}

BInt
bintMod(BInt a, BInt b)
{
	BInt r, q;
	Bool neg;
	/*
	 * Compute a mod b via horner's rule if b fits into a single word,
	 * use bintDivide o/wise.
	 */
	neg = bintIsNeg(a);
	if (neg)
		a = bintNegate(a);

	if (bintIsNeg(b))
		b = bintNegate(b);
	
	if (IsImmed(b)) {
		r = bintModi(a, (ULong) BIntToInt(b));
	} 
	else if (bintLength(b) < bitsizeof(ULong)) {
		/* Can't handle 32 bit divisors as yet! */
		r = bintModi(a, bintToULong(b));
	}
	else {
		q = bintDivide(&r, (BInt) a, (BInt) b);
		bintFree(q);
	}
	if (neg)
		r = xintNegate(r);

	return r;
}

local BInt
bintModi(BInt a, ULong b)
{
	ULong   d    = 1L << bitsizeof(BIntS);
	ULong  r;
	ULong acc;
	int   i;

	assert(!bintIsNeg(a));

	if (IsImmed(a)) {
		return bintNew(BIntToInt(a) % b);
	}

	if (b < d) {
		d = d % b;
		i = Placec(a) - 1;
		acc = Placev(a)[i] % b;
		i--;
		while (i>=0) {
			long tmp;
			/* acc = acc mod* d mod+ a[i] */
			acc = (acc * d) % b;
			tmp = acc - b + Placev(a)[i] % b;
			if (tmp < 0) tmp += b; 
			acc = tmp;
			i--;
		}
	}
	else {
		/* b larger than 1 halfword */
		i = Placec(a) - 1;
		acc = Placev(a)[i];
		i--;
		while (i>=0) {
			/* acc = acc mod* d mod+ a[i] */
			ULong hi, lo, rem;
			long  tmp;
			/*xxTimesDouble(&hi, &lo, acc, d);*/
			hi = acc >> bitsizeof(BIntS);
			lo = acc << bitsizeof(BIntS);
			rem = xxModDouble(hi, lo, b);
			tmp = (rem - (long) b) + Placev(a)[i];
			if (tmp < 0) 
				tmp += b;
			acc = tmp;
			i--;
		}
	}
	r = acc;
	return bintNew(r);
}

local ULong
bintToULong(BInt b)
{
	ULong res;
	if (bintIsSmall(b))
		return bintSmall(b);

	res = (ULong) Placev(b)[0] + ( ((ULong) Placev(b)[1]) << bitsizeof(BIntS));

	return res;
}

/*
 * The length of b, in bits. I.e. cieling(lg(abs(b))).
 * Define the bit length of zero to be one bit.
 */
Length
bintLength(BInt b)
{
	Length res;
	if (IsImmed(b)) return intLength(BIntToInt(b));
	res = BINT_LG_RADIX*(Placec(b)-1) + uintLength(Placev(b)[Placec(b)-1]);
	return res;
}


/*
 * Extract the ix-th bit of b.  I.e. the coef of 2**ix.
 * !! This should handle negative numbers.
 */
Bool
bintBit(BInt b, Length ix)
{
	Length	cq, cr;

	if (IsImmed(b)) return intBit(BIntToInt(b), ix);

	cq = ix / BINT_LG_RADIX;
	cr = ix % BINT_LG_RADIX;

	return (cq < Placec(b)) && (Placev(b)[cq] & (1<<cr)) != 0;
}

/*
 * Shift b by n bits.  I.e. compute b*2**n.
 */
BInt
bintShift(BInt b, int n)
{
	BInt	r;
	IInt	bbitc, rbitc;
	Bool	bImmed;

	bImmed = IsImmed(b);
	bbitc  = bintLength(b);
	rbitc  = bbitc + n;

	if (b == bint0) return bint0;
	if (bbitc == 0 || rbitc <= 0)
		return IntToBInt(int0);
	if (bImmed && rbitc <= INT_LG_IMMED) {
		IInt      i = BIntToInt(b);
		UIInt     u = (i > 0) ? i : -i;
		u = (n > 0) ? u << n : u >> -n;
		return IntToBInt((i > 0) ? u : -(IInt)u);
	}
	if (bImmed) b = xintStore(b);
	r = bintAlloc(rbitc);

	iintShift(r, b, n);

	if (bImmed) bintFree(b);
	if (rbitc <= INT_LG_IMMED) r = xintImmedIfCan(r);

	return r;
}

BInt
bintShiftRem(BInt b, int n)
{
	BInt r;
	int  i, top;
	/* Returns lowest `n' bits from b. */
	
	if (IsImmed(b)) {
		IInt x = BIntToInt(b);
		return IntToBInt(x & ((1 << n) - 1));
	}

	r = bintAlloc(n);
	
	for (i=0; i<Placea(r) - 1; i++) Placev(r)[i] = Placev(b)[i];
	top = n - BINT_LG_RADIX*(Placec(r) - 1);
	Placev(r)[i] = Placev(b)[i] & ((1<< top) - 1);

	return xintImmedIfCan(r);
}

/****************************************************************************
 *
 * :: Low-level, fine control allocation
 *
 ****************************************************************************/

/*
 * Allocate a new, stored integer value.
 */
BInt
xintStoreI(long n)
{
	BInt	b;
	ULong	u;
	IInt	a;

	u = (n < 0) ? -n : n;
	for (a = 1; ; a += 1, u >>= BINT_LG_RADIX)
		if( u < BINT_RADIX ) break;
	b = bintAllocPlaces(a);
	return xintCopyInI(b, n);
}

/*
 * Convert b to a stored representation.
 */
BInt
xintStore(BInt b)
{
	if (IsImmed(b)) return xintStoreI(BIntToInt(b));

	return b;
}

/*
 * Convert b to an immeditate (non-stored) representation, if possible.
 */
BInt
xintImmedIfCan(BInt b)
{
	Length	pb;
	ULong	u = 0;

	if (IsImmed(b)) return b;

	pb = Placec(b);

	/* Special case all 32-bit cases */
	if (pb == 0)
		return IntToBInt(int0);
	if (pb == 1 || pb == 2) {
		if (pb == 1)
			u = Placev(b)[0];
		else
			u = ((BIntD)(Placev(b)[1]) << BINT_LG_RADIX)
			  + Placev(b)[0];
		if (IsNeg(b))
			return u > -(long)INT_MIN_IMMED
			       ? b : (BInt) ptrFrLong(MkImmed(-(IInt)u));
		else
			return u > INT_MAX_IMMED
			       ? b : (BInt) ptrFrLong(MkImmed(u));
	}


	/* For cases where 2*sizeof(BIntS) < sizeof(BInt) 	*/
	if (pb*BINT_LG_RADIX <= bitsizeof(IInt)) {
		int 	i;
		for (i = pb - 1; i>= 0; i--)
			u = (u << BINT_LG_RADIX) + Placev(b)[i];
		if (IsNeg(b))
		        return (u > -INT_MIN_IMMED) ? b 
				: (BInt) ptrFrLong(MkImmed(-u));
		else
		        return (u > INT_MAX_IMMED) ? b 
				: (BInt) ptrFrLong(MkImmed(u));
	}

	return b;
}

/*
 * Place integer value n into stored BInt b.
 * If b is not big enough, then it is reallocated.
 */
BInt
xintCopyInI(BInt b, long n)
{
	ULong u, ut;
	IInt	i, c, a;
	assert(!IsImmed(b));

	u = (n<0)? -n : n;
	c = Placea(b);

	/* Quick test for usual case. */
	if (u < BINT_RADIX && c > 0) {
		Placev(b)[0] = u;
		Placec(b)    = 1;
		IsNeg(b) = (n < 0);
		return b;
	}

	/* Multi-word case */
	ut = u; for (a = 0; ut != 0; a++) ut /= BINT_RADIX;

	if (a > c) {
		bintFree(b);
		b = bintAllocPlaces(a);
		c = a;
	}
	for (i = 0; u != 0 && i < c; i++) {
		Placev(b)[i] = u % BINT_RADIX;
		u            = u / BINT_RADIX;
	}
	Placec(b)     = i;

	IsNeg(b) = (n < 0);
	return b;
}


/*
 * Ensure b can contain at least bitc bits.
 */
BInt
xintNeeds(BInt b, Length bitc)
{
	BInt	bn;
	Length	i, c;

	if (IsImmed(b)) {
		 if (bitc <= INT_LG_IMMED)
			return b;
		else
			return xintCopyInI(bintAlloc(bitc), BIntToInt(b));
	}

	if (Placea(b)*BINT_LG_RADIX >= bitc) return b;

	bn = bintAlloc(bitc);
	c  = Placec(b);
	for (i = 0; i < c; i++) Placev(bn)[i] = Placev(b)[i];
	Placec(bn) = Placec(b);
	IsNeg(bn)  = IsNeg(b);
	bintFree(b);

	return bn;
}


/*
 * Operations which free their operands.
 */
int
xintPrint(FILE *fout, BInt a)
{
	int cc = bintPrint(fout, a);
	bintFree(a);
	return cc;
}

int
xintPrint2(FILE *fout, BInt a)
{
	int cc = bintPrint2(fout, a);
	bintFree(a);
	return cc;
}

int
xintPrint16(FILE *fout, BInt a)
{
	int cc = bintPrint16(fout, a);
	bintFree(a);
	return cc;
}

BInt
xintNegate(BInt a)
{
	BInt r = bintNegate(a);
	bintFree(a);
	return r;
}

BInt
xintPlus(BInt a, BInt b)
{
	BInt r = bintPlus(a, b);
	bintFree(a);
	bintFree(b);
	return r;
}

BInt
xintMinus(BInt a, BInt b)
{
	BInt r = bintMinus(a, b);
	bintFree(a);
	bintFree(b);
	return r;
}

BInt
xintTimes(BInt a, BInt b)
{
	BInt r = bintTimes(a, b);
	bintFree(a);
	bintFree(b);
	return r;
}

BInt
xintDivide(BInt *pr, BInt a, BInt b)
{
	BInt q = bintDivide(pr, a, b);
	bintFree(a);
	bintFree(b);
	return q;
}

BInt
xintShift(BInt a, int n)
{
	BInt r = bintShift(a, n);
	bintFree(a);
	return r;
}

/*****************************************************************************
 *
 * :: Low-level, non-allocating operations
 *
 * Arithmetic is done with the macros PlusStep, TimesDouble, etc.
 *
 ****************************************************************************/

/*
 * Place the value abs(a) in r.
 */
void
iintAbs(BInt r, BInt a)
{
	IInt	i, c;

	assert(!IsImmed(r) && !IsImmed(a) && Placea(r)>=Placec(a));

	IsNeg(r) = false;

	if (Beq(r,a)) return;

	c = Placec(a);

	for (i = 0; i < c; i++) Placev(r)[i] = Placev(a)[i];
	Placec(r) = Placec(a);
}

/*
 * Place the value -a in r.
 */
void
iintNegate(BInt r, BInt a)
{
	IInt	i, c;

	assert(!IsImmed(r) && !IsImmed(a) && Placea(r)>=Placec(a));

	IsNeg(r) = !IsNeg(a);

	if (Beq(r,a)) return;
		
	c = Placec(a);

	for (i = 0; i < c; i++) Placev(r)[i] = Placev(a)[i];
	Placec(r) = Placec(a);
}

/*
 * Add non-negative integers a and b and place result in preallocated r.
 * It is assumed that a is greater than b.
 * The result r, can be an alias for a or b.
 */
void
iintPlus(BInt r, BInt a, BInt b)
{
	BIntS	s, k;		/* k = 0 or 1 */
	IInt	i, ac, bc;

	assert(!IsImmed(a) && !IsImmed(b));
	assert(!IsNeg(a) && !IsNeg(b));
	assert(Placec(a) >= Placec(b));
	assert(!IsImmed(r) && Placea(r) >= Placec(a));

	ac = Placec(a);
	bc = Placec(b);

	k = 0;

	for (i = 0; i < bc; i++) {
		PlusStep(k, s, Placev(a)[i], Placev(b)[i], k);
		Placev(r)[i] = s;
	}
	
	for ( ; i < ac && k; i++) {
		PlusStep(k, s, Placev(a)[i], int0, k);
		Placev(r)[i] = s;
	}

	if (!k && Beq(r,a)) return;

	for ( ; i < ac; i++) {
		Placev(r)[i] = Placev(a)[i];
	}

	if (k)
		Placev(r)[i++] = k;

	Placec(r) = i;
}

/*
 * Subtract non-negative integers a and b and place result in preallocated r.
 * It is assumed that a >= b >= 0.
 * The result r, can be an alias for a or b.
 *
 * This is Algorithm S of Knuth Volume 2, Section 4.3.1.
 */
void
iintMinus(BInt r, BInt a, BInt b)
{
	BIntS	s, kp1;		/* kp1 = k+1. k = 0 or -1 */
	IInt	i, ac, bc;

	assert(!IsImmed(a) && !IsImmed(b));
	assert(!IsNeg(a) && !IsNeg(b));
	assert(Placec(a) >= Placec(b));
	assert(!IsImmed(r) && Placea(r) >= Placec(a));

	ac = Placec(a);
	bc = Placec(b);

	kp1 = 1;

	for (i = 0; i < bc; i++) {
		MinusStep(kp1, s, Placev(a)[i], Placev(b)[i], kp1);
		Placev(r)[i] = s;
	}
	
	for ( ; i < ac && kp1 == 0; i++) {
		MinusStep(kp1, s, Placev(a)[i], int0, kp1);
		Placev(r)[i] = s;
	}

	if (kp1 == 1 && Beq(r,a)) goto normalize;

	for ( ; i < ac; i++) {
		Placev(r)[i] = Placev(a)[i];
	}

	assert(kp1 == 1);

normalize:
	for (i = Placec(a) - 1; i >= 0; i--)
		if (Placev(r)[i] != 0) break;
	Placec(r) = i+1;
}

/*
 * Multiply non-negative integers a and b and place result in preallocated r.
 * The result r, cannot be an alias for a or b.
 */
void
iintTimes(BInt r, BInt a, BInt b)
{
	IInt	i, j, ac, bc;
	BIntS	k, ai, bj;

	assert(!IsImmed(a) && !IsImmed(b));
	assert(!IsNeg(a) && !IsNeg(b));
	assert(!IsImmed(r) && Placea(r) >= Placec(a)+Placec(b));

	ac = Placec(a);
	bc = Placec(b);

	/* Make a be the longer number to minimize loop cost. */
	if (ac < bc) {
		BInt x;
		IInt  xc;
		x = a;	xc = ac;
		a = b; 	ac = bc;
		b = x;	bc = xc;
	}

	for (i = 0; i < ac; i++) Placev(r)[i] = 0;

	for (j = 0; j < bc; j++) {
		bj  = Placev(b)[j];
		k   = 0;
		if (bj != 0) {
			for (i = 0; i < ac; i++) {
				ai = Placev(a)[i];
				TimesStep(k, Placev(r)[i+j],
					  ai, bj, Placev(r)[i+j], k);
			}
		}
		Placev(r)[ac+j] = k;
	}
	while (Placec(r) > 0 && Placev(r)[Placec(r)-1] == 0) Placec(r)--;
}


/*
 * Divide the non-negative numbers a and b and place the quotient and
 * remainder in the preallocated structures q and r.
 *
 * The output structure, q, must have space for m+1 digits.
 * The output structure, r, must have space for n+m+1 digits
 * (unless n = 1 or a < b), even though at most n will be used for output.
 *
 * The output q may not be an alias for an input.
 * The output r can be an alias for the input a.
 *
 * This is Algorithm D of Knuth Vol 2, Section 4.3.1.
 * Note: we index the digits n-1..0 rather than 1..n.
 */

/*
 * Convert to Knuth's notation.
 */
#define KtoI(ki)	(n   - (ki))
#define KtoJu(kj)	(nm  - (kj))
#define KtoJq(kj)	(m   - (kj))
#define DB		/*Debug*/

void
iintDivide(BInt q, BInt r, BInt u, BInt v)
{
	IInt	n, m, nm;
	IInt	i;
	IInt	ki, kj, kjj;		/* Knuths i + j. */
	BIntS	d, qhat, rhat;
	BIntS	v1, v2, uj0, uj1, uj2; 	/* Knuth's v1,v2, uj,uj+1,uj+2 */
	BIntS	k, kk, vi, ujj, uh, ul;

	assert(!IsImmed(u) && !IsImmed(v));
	assert(!IsImmed(q) && !IsImmed(r));
	assert(Placea(r) > Placec(u));
	assert(Placev(v)[Placec(v)-1] != 0);
	assert(q != r && q != u && q != v && r != v);

	n = Placec(v);
	m = Placec(u) - n;
	nm = n + m;

	/* Copy u into r as an in-place work area. */
	if (r != u) {
		for (i = 0; i < nm; i++) Placev(r)[i] = Placev(u)[i];
		Placec(r) = nm;
		u = r;
	}

	/* Trivial case: v has 1 digit. */
	if (n == 1) {
		iintDivideS(q, &rhat, u, Placev(v)[0]);
		Placev(u)[0] = rhat;
		Placec(u) = 1;
		return;
	}
		
	/* Trivial case: u < v => q = 0, r = u. */
	if (bintLT(u,v)) {
		Placec(q) = 0;
		return;
	}

	/*
	 * D1. Normalize: choose d which results in v[n-1] >= BINT_RADIX/2.
	 */
	v1= Placev(v)[KtoI(1)];
	if (v1 >= BINT_RADIX/2) {
		bintDEBUG(dbOut, "**** d = 1 ****\n");
		d = 1;
	}
	else {
		d = BINT_RADIX/(v1 + 1);
		iintTimesS(u, u, d);
		iintTimesS(v, v, d);	/* Guaranteed ptr u != v */
	}
	v1= Placev(v)[KtoI(1)];
	v2= Placev(v)[KtoI(2)];
	if (Placec(u) == nm) {
		Placev(u)[Placec(u)++] = 0;
	}
	assert(Placec(u) == nm+1);
	assert(Placec(v) == n);

	/*
	 * D2-D7. Each iter, divide u[kj..kj+n] by v[1..n] to get a digit of q.
	 */
	for (kj = 0; kj <= m; kj++) {
		/*
		 * D3. Compute qhat.
		 */
		uj0 = Placev(u)[KtoJu(kj)];
		uj1 = Placev(u)[KtoJu(kj+1)];
		uj2 = Placev(u)[KtoJu(kj+2)];

		if (uj0 == v1) {
			bintDEBUG(dbOut, "**** uj0 == v1 ****\n");
			qhat = BINT_RADIX_MINUS_1;
			rhat = uj1;
			PlusStep(k, rhat, rhat, v1, int0);
			
		}
		else {
			DivideDouble(qhat, rhat, uj0, uj1, v1);
			k = 0;
		}
		if (!k) {
			BIntS	v2qhh, v2qhl;
			Bool	isGT;
			for (i = 1; ; i++) {
				/* Loop shd be evaluated no more than twice. */
				if (DEBUG(bint)) {
					if (i == 2)
						fprintf(dbOut, "**** 2 ****\n");
				}
				assert(i <= 2);
				/*
				 * This test could be speeded up by doing
				 * unsigned long arithmetic and a comparison
				 * when BINT_LG_RADIX <= bitsizeof(long).
				 * However by writing it this way, this code
				 * could handle a long-sized radix.
				 */
				TimesDouble (v2qhh,v2qhl, v2, qhat);
				TestGTDouble(isGT, v2qhh,v2qhl, rhat,uj2);

				if (!isGT) break;

				qhat--;
				PlusStep(k, rhat, rhat, v1, int0);
				if (k) break;
			}
		}
		if (DEBUG(bint)) {
			if (k)
				fprintf(dbOut, "**** rhat ov ****\n");
		}

		/*
		 * D4. Multiply and subtract: u[kj..kj+n] -= qhat * (0,v[1..n])
	 	 */

		k = 0;

		for (ki = n, kjj = kj+n; ki >= 0; ki--, kjj--) {
			vi  = (ki == 0) ? 0 : Placev(v)[KtoI(ki)];
			ujj = Placev(u)[KtoJu(kjj)];

			TimesDouble(uh, ul, qhat, vi);

			MinusStep(kk, ujj, ujj, ul, 1);
			uh += !kk;

			MinusStep(kk, ujj, ujj,  k, 1);
			uh += !kk;

			Placev(u)[KtoJu(kjj)] = ujj;
			k = uh;
		}

		/*
		 * D5. Set quotient digit and test remainder.
		 */
		Placev(q)[KtoJq(kj)] = qhat;	/* I.e. Knuth's q[j] */

		if (k != 0) {
			/*
			 * D6. Add back: u[kj..kj+n] += (0,v[1..n])
			 */
			bintDEBUG(dbOut, "**** Add Back %d ****\n", k);
			Placev(q)[KtoJq(kj)]--;
			k = 0;

			for (ki = n, kjj = kj+n; ki >= 0; ki--, kjj--) {
				vi  = (ki == 0) ? 0 : Placev(v)[KtoI(ki)];

				PlusStep(k,  Placev(u)[KtoJu(kjj)],
					 vi, Placev(u)[KtoJu(kjj)], k);
			}
		}
	}

	/*
	 * D8. Unnormalize: u /= d
	 */
	Placec(u) = n;
	iintDivideS(u, NULL, u, d);
	iintDivideS(v, NULL, v, d);	/* Return v to original state. */

	/* Set place counts to not see leading zeros. */
	for (i = Placec(q)-1; i >= 0; i--)
		if (Placev(q)[i] != 0) break;
	Placec(q) = i+1;

	for (i = Placec(u)-1; i >= 0; i--)
		if (Placev(u)[i] != 0) break;
	Placec(u) = i+1;
}

/*
 * iintShift(r, b, n)
 *    Shift b by n bits into r;
 *    The result, r, can be an alias for the argument b.
 *	
 * Examples:
 *    BINT_LG_RADIX = 4
 *    n    -- number of bits shifted by
 *    bitc -- bit count of result
 *    c    -- number of places
 *    z    -- number of zero bits in leading place
 *    up   -- whether position of bits in first place is further up
 *    q    -- upward rel offset of first place (not including chopped zeros)
 *    q0   -- upward rel offset of first place (including phantom chopped zeros)
 *    h    -- upward  rel offset of bits from previous place
 *    k    -- downard rel offset of bits within place
 *
 *                             n bitc  c  z  up q q0  h  k
 *      .ooaa.bbbb.0000.0000.  8   14  4  2  n  2  2  4  0
 *      .0ooa.abbb.b000.0000.  7   13  4  3  n  2  2  3  1
 *           .aabb.bb00.0000.  6   12  3  0  y  1  2  2  2
 *           .oaab.bbb0.0000.  5   11  3  1  y  1  2  1  3
 *           .ooaa.bbbb.0000.  4   10  3  2  n  1  1  4  0
 *           .0ooa.abbb.b000.  3    9  3  3  n  1  1  3  1
 *                .aabb.bb00.  2    8  2  0  y  0  1  2  2
 *                .oaab.bbb0.  1    7  2  1  y  0  1  1  3
 *                .ooaa.bbbb.  0    6  2  2  n  0  0  4  0
 *                .0ooa.abbb. -1    5  2  3  n  0  0  3  1
 *                     .aabb. -2    4  1  0  y -1  0  1  2
 *                     .oaab. -3    3  1  1  y -1  0  3  1
 *                     .ooaa. -4    2  1  2  n -1 -1  4  0
 *                     .0ooa. -5    1  1  3  n -1 -1  3  1
 *                          . -6    0  0  0  y -2 -1  2  2
 */

void
iintShift(BInt r, BInt b, int n)
{
	IInt	q, q0, h, k;
	IInt	rbitc, rc, rz;
	IInt	bbitc, bc, bz;
	Bool	up;
	IInt	i, j, x0, x1;
	BIntS	*rp, *bp;

	assert(!IsImmed(r) && !IsImmed(b));

	bbitc = bintLength(b);
	bc    = Placec(b);
	bz    = bc * BINT_LG_RADIX - bbitc;
	bp    = Placev(b);

	rbitc = bbitc + n;
	rc    = QUO_ROUND_UP(rbitc, BINT_LG_RADIX);
	rz    = rc * BINT_LG_RADIX - rbitc;
	rp    = Placev(r);

	assert(Placea(r) >= rc);
	Placec(r) = rc;
	IsNeg(r)  = IsNeg(b);

	up = (rz <= bz);
	q  = rc - bc;
	q0 = q + up;
	h  = q0 * BINT_LG_RADIX - n;
	k  = BINT_LG_RADIX - h;
	x0 = 0;			/* for lint */

	/*
	 * Copy in the direction which allows in-place operation.
	 *
	 * Each place's value is temporarily held in x0 until
	 * it is certain that the original value of that place
	 * will not be needed.
	 */

	/*
	 * Would you believe that some compiler have trouble with x<<32 
	 * MIPS cc on IRIX64 with -64 
	 */
	if (h == BINT_LG_RADIX) h=0;
	if (n < 0) {
		/* Copy from lo to hi. */
		bp -= q0;
		i   = 0;
		j   = 0;

		if (rc > 1) {
			x0  = h ? bp[i] >> h : 0;
			x0 |= bp[++i]   << k;
		}
		while (i < rc-1) {
			x1  = x0;
			x0  = h ? bp[i] >> h : 0 ;
			x0 |= bp[++i]   << k;

			rp[j++] = x1 & BINT_RADIX_MASK;
		}
		if (rc > 1)
			rp[j++] = x0 & BINT_RADIX_MASK;
		if (up) {
			x0  = h ? bp[i] >> h : 0;
			x0 |= bp[++i]   << k;
		}
		else {
			x0  = h ? bp[i]   >> h : 0;
		}
		rp[j++] = x0 & BINT_RADIX_MASK;
	}
	else if (n > 0) {
		/* Copy from hi to lo. */
		i = bc - 1;
		j = rc - 1;

		if (up) {
			x0  = bp[i--] << k;
			x0 |= h ? bp[i]   >> h : 0;
		}
		else {
			x0  = h ? bp[i]   >> h : 0;
		}
		while (i > 0) {
			x1  = x0;
			x0  = bp[i--] << k;
			x0 |= h ? bp[i]  >> h : 0;

			rp[j--] = x1 & BINT_RADIX_MASK;
		}
		if (i == 0) {
			x1 = x0;
			x0 = bp[0] << k;
			rp[j--] = x1 & BINT_RADIX_MASK;
		}
		rp[j--] = x0 & BINT_RADIX_MASK;

		while (j >= 0) rp[j--] = 0;
	}
	else /* n == 0 */ {
		for (i = 0; i < rc; i++) rp[i] = bp[i];
	}
}

/*
 * iintTimesS(r, a, b)
 *
 * Place the value a*b into r.
 * The result, r, can be an alias for the input a.
 */
void
iintTimesS(BInt r, BInt a, BIntS b)
{
	IInt	n, j;
	BIntS	c;

	assert(!IsImmed(r) && !IsImmed(a));
	assert(Placea(r) >= Placea(a));
	
	/* !!! xintCopyInI may reallocate */
	if (b == 0) { xintCopyInI(r, int0); return; }

	n = Placec(a);
	c = 0;

	for (j = 0; j < n; j++)
		TimesStep(c, Placev(r)[j], Placev(a)[j], b, c, int0);

	if (c) Placev(r)[j++] = c;
	Placec(r) = j;
}

/*
 * iintTimesPlusS(r, a, b, c)
 *
 * Place the value a*b + c into r.
 * The result, r, can be an alias for the input a.
 */
void
iintTimesPlusS(BInt r, BInt a, BIntS b, BIntS c)
{
	IInt	n, j;

	assert(!IsImmed(r) && !IsImmed(a));
	assert(Placea(r) >= Placea(a));

	/* !!! xintCopyInI may reallocate */
	if (b == 0) { xintCopyInI(r, c); return; }

	n = Placec(a);

	for (j = 0; j < n; j++)
		TimesStep(c, Placev(r)[j], Placev(a)[j], b, c, int0);

	if (c) Placev(r)[j++] = c;
	Placec(r) = j;
}

/*
 * iintDivideS(q, &r, a, b)
 *
 * This divides the positive multiprecision number a by the half precision
 * number b, 0 < b < BINT_RADIX.
 *
 * The quotient is placed in q and the remainder in r.
 * If q == u, then the operation is done in-place.
 *
 * The result, q, can be an alias for a.
 */
void
iintDivideS(BInt q, BIntS *pr, BInt a, BIntS b)
{
	IInt	n, j;
	BIntS	r;

	assert(!IsImmed(q) && !IsImmed(a));
#ifndef BIGINT_SHHHH
	if (Placea(q) < Placec(a))
		printf("This is it guys.\n");
#endif
	assert(Placea(q) >= Placec(a));

	n = Placec(a);
	r = 0;
	for (j = n-1; j >= 0; j--)
		DivideDouble(Placev(q)[j], r,  r, Placev(a)[j],  b);

	Placec(q) = (Placev(q)[n-1] == 0) ? n-1 : n;
	if (pr) *pr = r;
}


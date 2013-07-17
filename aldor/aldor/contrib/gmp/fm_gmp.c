/****************************************************************************
 *
 * fm_gmp.c:  Run time support for C version of abstract machine.
 *	      Big Integer implementation using GMP as basis.
 *
 * Copyright (c) 1990-2007 Aldor Software Organization Ltd (Aldor.org).
 *
 ****************************************************************************/

/*
 * Fi is the acronym for Foam Implementation
 *
 * The operations in this file provide an implementation of FOAM for
 * use with C code.  The functions must use the old-style headers to
 * match the declarations in the foam_c.h file. 
 *
 * I.e. Use     FiPtr fiEnvMake(size) FiSInt size; { ... }
 * rather than  FiPtr fiEnvMake(FiSInt size) { ... }
 */

/*
 * The include of "foam_c.h" must come second.
 */
# include "axlgen.h"
# include "foam_c.h"
# include <gmp.h>

/*****************************************************************************
 *
 * :: BInt operations
 *
 *****************************************************************************/

typedef mpz_t *FGmpBInt;
typedef long IInt;

#define F2MPZ(x) (*(x))
#define B2MPZ(x) F2MPZ( (FGmpBInt) x)

#define IsImmed(b)	((long)(b) &  1)
#define MkImmed(n)	((((long)(n)) << 1) | 1)
#define UnImmed(n)	((n) >> 1)

#define FGmpBIntToInt(b) 	((IInt) UnImmed(ptrToLong(b)))
#define IntToFGmpBInt(n)	((FGmpBInt) ptrFrLong(MkImmed(n)))
#define FGmpBIntZero		((FGmpBInt) MkImmed(0))
#define FGmpBIntOne		((FGmpBInt) MkImmed(1))


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

static int fgmpLimbsPerWord;

#ifndef NDEBUG
#define TraceSmallOps
#endif

#ifdef TraceSmallOps
int fgmpSmallCounter[30];
int fgmpBigCounter[30];
int fgmpMixedCounter[30];
int fgmpImmed;
int fgmpBig;
#define SmallOp(i,x,y) ((Abut(x,Immed) && Abut(y,Immed))  \
			? fgmpSmallCounter[i]++ 	  \
			: (Abut(x,Immed) != Abut(y,Immed) \
			   ? fgmpMixedCounter[i]++ : fgmpBigCounter[i]++))
#define NewImmed() (fgmpImmed++)
#define NewBig()	 (fgmpBig++)
#else
#define SmallOp(i,x,y) (0)
#define NewBig()       (0)
#define NewImmed()     (0)
#endif

static FGmpBInt
fgmpBIntNew()
{
	FGmpBInt i;
	i = (FGmpBInt) stoAlloc(0, sizeof(*i));
	mpz_init(F2MPZ(i));
	return i;
}

static FGmpBInt
fgmpBIntNewFrInt(long n)
{
	FGmpBInt i;
	i = (FGmpBInt) stoAlloc(0, sizeof(*i));
	mpz_init_set_si(F2MPZ(i), n);
	return i;
}

static void
fgmpBIntFree(FGmpBInt x)
{
	/*mpz_clear(F2MPZ(x));*/
	/* mpz_clear does a multiply (and an indirection),
	 * and this appears to be _way_ expensive on a sparc.
	 * We just wing it */
	stoFree(F2MPZ(x)->_mp_d);
	stoFree(x);
}

/* NB: This function may destroy its argument */
static FGmpBInt
fgmpImmedIfCan(FGmpBInt b)
{
	long sz;
	int isNeg = 0;

	if (IsImmed(b)) return b;
	sz = F2MPZ(b)->_mp_size;
	if (sz < 0) {
		sz = -sz; isNeg = 1;
	}
	NewBig();
	if (sz <= fgmpLimbsPerWord) {
		unsigned long u;
		u = mpz_get_ui(F2MPZ(b));
		if (isNeg && u > -(long) INT_MIN_IMMED)
			return b;
		else if (u > INT_MAX_IMMED) 
			return b;
		fgmpBIntFree(b);
		NewImmed();
		return IntToFGmpBInt(isNeg ? -(long) u : (long) u);
	}
	else
		return b;
}

FGmpBInt
fgmpFrInt(long n)
{
	FGmpBInt i;
	i = fgmpBIntNewFrInt(n);
	return i;
}


static void*
fgmpAlloc(size_t sz)
{
	return stoAlloc(OB_BInt, sz);
}

static void *
fgmpReAlloc(void *p, size_t old, size_t new)
{
	return stoResize(p, new);
}

static void
fgmpFree(void *p, size_t sz)
{
	stoFree(p);
}

void
fiBIntInit()
{
	mp_set_memory_functions(fgmpAlloc, fgmpReAlloc, fgmpFree);
	fgmpLimbsPerWord = bitsizeof(FGmpBInt)/mp_bits_per_limb;
	if (fgmpLimbsPerWord == 0) fgmpLimbsPerWord=1;
}


/* !!Easy-but-none-too-fast */

FiBInt
fiBIntFrPlacev(int isNeg, unsigned long placec, FiBIntS *placev)
{
	FiBInt x = fiBInt0();
	int i;
	for (i=placec-1 ; i >= 0; i--) {
		/* 8 bits per byte... */
		x = fiBIntPlus(fiBIntShiftUp(x, 8*sizeof(FiBIntS)),
			       fiBIntFrInt(placev[i]));
	}
	if (isNeg)
		x = fiBIntNegate(x);
	return x;
}


FiBInt
fiBIntFrInt(long n)
{
	if (INT_IS_IMMED(n))
		return (FiBInt) IntToFGmpBInt(n);
	else
		return (FiBInt) fgmpFrInt(n);
}

FiBInt
fiBIntMake(FiPtr b)
{
	return (FiBInt) b;
}

FiBInt
fiBInt0(void)
{
	return (FiBInt) FGmpBIntZero;
}

FiBInt
fiBInt1(void)
{
	return (FiBInt) FGmpBIntOne;
}

FiBInt
fiBIntNew(long i)
{
	return fiBIntFrInt(i);
}

FiBool
fiBIntIsZero(FiBInt b)
{

	if (!IsImmed(b) && 
	    (mpz_cmp_si(B2MPZ(b), int0) == 0))
		fiHalt(666);

	return (FiBool) ( (FGmpBInt) b == FGmpBIntZero);
}

FiBool
fiBIntIsPos(FiBInt b)
{
	if (IsImmed(b))
		return (FiBool) (FGmpBIntToInt( (FGmpBInt)b) > 0);
	else
		return (FiBool) (B2MPZ(b)->_mp_size > 0);
}

FiBool
fiBIntIsNeg(FiBInt b)
{
	if (IsImmed(b))
		return (FiBool) (FGmpBIntToInt((FGmpBInt)b) < 0);
	else
		return (FiBool) (B2MPZ(b)->_mp_size < 0);
}

FiBool
fiBIntIsSingle(FiBInt b)
{
	int sz;
	if (IsImmed(b)) return (FiBool) true;
	sz = mpz_sizeinbase(F2MPZ((FGmpBInt) b), 2);

	/* One bit for the sign */
	if (sz >= bitsizeof(FGmpBInt)) 
		return (FiBool) false;
	return true;
}

FiBool
fiBIntLE(FiBInt a, FiBInt b)
{
	Bool aImmed, bImmed;

	aImmed = IsImmed(a);
	bImmed = IsImmed(b);

	if (aImmed && bImmed)
		return (FiBool) (FGmpBIntToInt((FGmpBInt)a) <= FGmpBIntToInt((FGmpBInt)b));
	
	if (bImmed)
		return (FiBool) (mpz_cmp_si(B2MPZ(a), 
					    FGmpBIntToInt((FGmpBInt)b)) <= 0);
	if (aImmed)
		return (FiBool) (mpz_cmp_si(B2MPZ(b), 
					    FGmpBIntToInt( (FGmpBInt)a)) >= 0);
	
	return (FiBool) (mpz_cmp(F2MPZ((FGmpBInt) a), F2MPZ((FGmpBInt) b)) <= 0);
}

FiBool
fiBIntEQ(FiBInt a, FiBInt b)
{
	if ((FiPtr) a == (FiPtr) b) return (FiBool) true;
	if(IsImmed(a) || IsImmed(b)) return (FiBool) false;
	return (FiBool) (mpz_cmp(B2MPZ(a), B2MPZ(b)) == 0);
}

FiBool
fiBIntLT(FiBInt a, FiBInt b)
{
	Bool aImmed, bImmed;

	aImmed = IsImmed(a);
	bImmed = IsImmed(b);

	SmallOp(0, a, b);

	if (aImmed && bImmed)
		return (FiBool) (FGmpBIntToInt( (FGmpBInt)a) < FGmpBIntToInt( (FGmpBInt)b));
	
	if (bImmed)
		return (FiBool) (mpz_cmp_si(B2MPZ(a), FGmpBIntToInt( (FGmpBInt)b)) < 0);
	if (aImmed)
		return (FiBool) (mpz_cmp_si(B2MPZ(b), FGmpBIntToInt( (FGmpBInt)a)) > 0);
	
	return (FiBool) (mpz_cmp(B2MPZ(a), B2MPZ(b)) < 0);
}

FiBool
fiBIntNE(FiBInt a, FiBInt b)
{
	return !fiBIntEQ(a, b);
}

FiBInt
fiBIntNegate(FiBInt b)
{
	FGmpBInt i;
	if (IsImmed(b))
		return (FiBInt) MkImmed(-FGmpBIntToInt( (FGmpBInt)b));
	i = fgmpBIntNew();
	mpz_neg(F2MPZ(i), B2MPZ(b));

	return (FiBInt) i;
}

FiBInt	
fiBIntPlus(FiBInt a, FiBInt b)
{
	FGmpBInt res;
	Bool aImmed, bImmed;
	
	aImmed = IsImmed(a);
	bImmed = IsImmed(b);

	SmallOp(1, a, b);
	if (aImmed && bImmed) {
		long val = FGmpBIntToInt((FGmpBInt)a) + FGmpBIntToInt((FGmpBInt)b);
		if (INT_IS_IMMED(val)) 
			return (FiBInt) IntToFGmpBInt(val);
		else
			return (FiBInt) fgmpFrInt(val);
	}

	if (aImmed) a = (FiBInt) fgmpFrInt(FGmpBIntToInt((FGmpBInt)a));
	if (bImmed) b = (FiBInt) fgmpFrInt(FGmpBIntToInt((FGmpBInt)b));
		
	res = fgmpBIntNew();
	mpz_add(F2MPZ(res), B2MPZ(a), B2MPZ(b));

	if (aImmed) fiBIntFree(a);
	if (bImmed) fiBIntFree(b);

	return (FiBInt) fgmpImmedIfCan(res);
}
	
FiBInt	
fiBIntMinus(FiBInt a, FiBInt b)
{
	FGmpBInt res;
	Bool aImmed, bImmed;
	
	aImmed = IsImmed(a);
	bImmed = IsImmed(b);

	SmallOp(2, a, b);
	if (aImmed && bImmed) {
		long val = FGmpBIntToInt((FGmpBInt)a) - FGmpBIntToInt((FGmpBInt)b);
		if (INT_IS_IMMED(val)) 
			return (FiBInt) IntToFGmpBInt(val);
		else
			return (FiBInt) fgmpFrInt(val);
	}

	if (aImmed) a = (FiBInt) fgmpFrInt(FGmpBIntToInt( (FGmpBInt)a));
	if (bImmed) b = (FiBInt) fgmpFrInt(FGmpBIntToInt( (FGmpBInt)b));
		
	res = fgmpBIntNew();
	mpz_sub(F2MPZ(res), B2MPZ(a), B2MPZ(b));

	if (aImmed) fiBIntFree(a);
	if (bImmed) fiBIntFree(b);

	return (FiBInt) fgmpImmedIfCan(res);
}
	
FiBInt	
fiBIntTimes(FiBInt a, FiBInt b)
{
	FGmpBInt res;
	Bool aImmed, bImmed;
	
	aImmed = IsImmed(a);
	bImmed = IsImmed(b);

	SmallOp(3, a, b);
	if (aImmed) {
		FiBInt tmp = a;
		a = b;
		b = tmp;
		aImmed = bImmed;
		bImmed = true;
	}
	res = fgmpBIntNew();
	if (aImmed) a = (FiBInt) fgmpFrInt(FGmpBIntToInt( (FGmpBInt)a));
	if (bImmed) {
		long bb = FGmpBIntToInt( (FGmpBInt)b);
		mpz_mul_ui(F2MPZ(res), B2MPZ(a), labs(bb));
		if (bb < 0)
			mpz_neg(F2MPZ(res), F2MPZ(res));
	}
	else {
		mpz_mul(F2MPZ(res), B2MPZ(a), B2MPZ(b));
		if (bImmed) fiBIntFree(b);
	}
	if (aImmed) fiBIntFree(a);

	return (FiBInt) fgmpImmedIfCan(res);
}
	
FiBInt
fiBIntTimesPlus(FiBInt a, FiBInt b, FiBInt c)
{
	FiBInt tmp, res;
	tmp = fiBIntTimes(a, b);
	res = fiBIntPlus(tmp, c);
	fiBIntFree(tmp);
	return res;
}

FiSInt	
fiBIntLength(FiBInt b)
{
	if (IsImmed(b))	return intLength(FGmpBIntToInt( (FGmpBInt)b));

	return mpz_sizeinbase(B2MPZ(b), 2);
}
	
FiBInt	
fiBIntShiftUp(FiBInt a, FiSInt n)
{
	FGmpBInt res;
	Bool aImmed;

	aImmed = IsImmed(a);
	if (aImmed) a = (FiBInt) fgmpFrInt(FGmpBIntToInt( (FGmpBInt)a));
	res = fgmpBIntNew();
	mpz_mul_2exp(F2MPZ(res), B2MPZ(a), (unsigned long) n);
	
	if (aImmed) fiBIntFree(a);
	return (FiBInt) fgmpImmedIfCan(res);
}
	
	
FiBInt	
fiBIntShiftDn(FiBInt a, FiSInt n)
{
	FGmpBInt res;
	Bool aImmed;

	aImmed = IsImmed(a);
	if (aImmed) a = (FiBInt) fgmpFrInt(FGmpBIntToInt( (FGmpBInt)a));
	res = fgmpBIntNew();
	mpz_tdiv_q_2exp(F2MPZ(res), B2MPZ(a), (unsigned long) n);
	
	if (aImmed) fiBIntFree(a);
	return (FiBInt) fgmpImmedIfCan(res);
}
	

FiBool	
fiBIntBit(FiBInt b, FiSInt n)
{	
	FGmpBInt tmp;
	Bool res;

	if (IsImmed(b)) return (FiBool) intBit(FGmpBIntToInt( (FGmpBInt)(FGmpBInt) b), n);

	/* XXX: Should not allocate memory */
	tmp = fgmpBIntNew();
	mpz_tdiv_r_2exp(F2MPZ(tmp), B2MPZ(b), n+1);
	mpz_tdiv_q_2exp(F2MPZ(tmp), F2MPZ(tmp), n);

	res = (mpz_cmp_si(F2MPZ(tmp), 0) != 0);
	fiBIntFree((FiBInt) tmp);
	return res;
}


FiBInt	
fiBIntMod(FiBInt a, FiBInt b)
{
	FGmpBInt res;
	Bool aImmed, bImmed;
	
	aImmed = IsImmed(a);
	bImmed = IsImmed(b);

	SmallOp(4, a, b);

	if (aImmed) a = (FiBInt) fgmpFrInt(FGmpBIntToInt( (FGmpBInt)a));
	if (bImmed) b = (FiBInt) fgmpFrInt(FGmpBIntToInt( (FGmpBInt)b));
		
	res = fgmpBIntNew();
	mpz_mod(F2MPZ(res), B2MPZ(a), B2MPZ(b));

	if (aImmed) fiBIntFree(a);
	if (bImmed) fiBIntFree(b);

	return (FiBInt) fgmpImmedIfCan(res);
}

FiBInt	
fiBIntQuo(FiBInt a, FiBInt b)
{
	FGmpBInt res;
	Bool aImmed, bImmed;
	
	aImmed = IsImmed(a);
	bImmed = IsImmed(b);

	SmallOp(5, a, b);
	if (aImmed && bImmed) {
		long val = FGmpBIntToInt((FGmpBInt)a) / FGmpBIntToInt((FGmpBInt)b);
		return (FiBInt) IntToFGmpBInt(val);
	}
	if (aImmed) a = (FiBInt) fgmpFrInt(FGmpBIntToInt( (FGmpBInt)a));
	if (bImmed) b = (FiBInt) fgmpFrInt(FGmpBIntToInt( (FGmpBInt)b));
		
	res = fgmpBIntNew();
	mpz_tdiv_q(F2MPZ(res), B2MPZ(a), B2MPZ(b));

	if (aImmed) fiBIntFree(a);
	if (bImmed) fiBIntFree(b);

	return (FiBInt) fgmpImmedIfCan(res);
}

FiBInt
fiBIntShiftRem(FiBInt a, FiSInt b)
{
	FGmpBInt res;
	
	if (IsImmed(a)) a = (FiBInt) fgmpFrInt(FGmpBIntToInt( (FGmpBInt) a));
		
	res = fgmpBIntNew();
	mpz_fdiv_r_2exp(F2MPZ(res), B2MPZ(a), b);

	return (FiBInt) fgmpImmedIfCan(res);
}


FiBInt	
fiBIntRem(FiBInt a, FiBInt b)
{
	FGmpBInt res;
	Bool aImmed, bImmed;
	
	aImmed = IsImmed(a);
	bImmed = IsImmed(b);

	SmallOp(6, a, b);
	if (aImmed && bImmed) {
		long val = FGmpBIntToInt((FGmpBInt)a) % FGmpBIntToInt((FGmpBInt)b);
		return (FiBInt) IntToFGmpBInt(val);
	}

	res = fgmpBIntNew();

	if (!aImmed && bImmed) {
		mpz_tdiv_r_ui(F2MPZ(res), B2MPZ(a),
			      FGmpBIntToInt((FGmpBInt) b));
		return (FiBInt) fgmpImmedIfCan(res);
	}

	if (aImmed) a = (FiBInt) fgmpFrInt(FGmpBIntToInt( (FGmpBInt)a));
	
	mpz_tdiv_r(F2MPZ(res), B2MPZ(a), B2MPZ(b));

	if (aImmed) fiBIntFree(a);
	if (bImmed) fiBIntFree(b);

	return (FiBInt) fgmpImmedIfCan(res);
}

void
fiBIntDivide(FiBInt a, FiBInt b, FiBInt *pr0, FiBInt *pr1)
{
	FGmpBInt r0, r1;
	Bool aImmed, bImmed;
	
	aImmed = IsImmed(a);
	bImmed = IsImmed(b);

	SmallOp(7, a, b);
	if (IsImmed(a) && IsImmed(b)) {
		long aa, bb, r0, r1;
		aa = FGmpBIntToInt((FGmpBInt) a);
		bb = FGmpBIntToInt((FGmpBInt) b);
		/* Some compilers can optimise this */
		r0 = aa / bb;
		r1 = aa - r0 * bb ;
		*pr0 = (FiBInt) IntToFGmpBInt(r0);
		*pr1 = (FiBInt) IntToFGmpBInt(r1);
		return;
	}
	if (aImmed) a = (FiBInt) fgmpFrInt(FGmpBIntToInt( (FGmpBInt)a));
	if (bImmed) b = (FiBInt) fgmpFrInt(FGmpBIntToInt( (FGmpBInt)b));
		
	r0 = fgmpBIntNew();
	r1 = fgmpBIntNew();
	mpz_tdiv_qr(F2MPZ(r0), F2MPZ(r1), B2MPZ(a), B2MPZ(b));

	if (aImmed) fiBIntFree(a);
	if (bImmed) fiBIntFree(b);
	
	*pr0 = (FiBInt) fgmpImmedIfCan(r0);
	*pr1 = (FiBInt) fgmpImmedIfCan(r1);
}

FiBInt	
fiBIntGcd(FiBInt a, FiBInt b)
{
	FGmpBInt res;
	Bool aImmed, bImmed;
	
	aImmed = IsImmed(a);
	bImmed = IsImmed(b);

	SmallOp(8, a, b);

	if (aImmed) a = (FiBInt) fgmpFrInt(FGmpBIntToInt( (FGmpBInt)a));
	if (bImmed) b = (FiBInt) fgmpFrInt(FGmpBIntToInt( (FGmpBInt)b));
		
	res = fgmpBIntNew();
	mpz_gcd(F2MPZ(res), B2MPZ(a), B2MPZ(b));

	if (aImmed) fiBIntFree(a);
	if (bImmed) fiBIntFree(b);

	return (FiBInt) fgmpImmedIfCan(res);
}

FiBInt	
fiBIntSIPower(FiBInt a, FiSInt n)
{
	FGmpBInt res;
	Bool aImmed;

	aImmed = IsImmed(a);
	if (aImmed) a = (FiBInt) fgmpFrInt(FGmpBIntToInt( (FGmpBInt)a));

	res = fgmpBIntNew();
	mpz_pow_ui(F2MPZ(res), B2MPZ(a), (unsigned long) n);
	
	if (aImmed) fiBIntFree(a);
	return (FiBInt) fgmpImmedIfCan(res);
}

/*
 *!! Should use iint{Times,Plus}.
 */
FiBInt	
fiBIntBIPower(FiBInt a0, FiBInt b0)
{
	FiSInt i;
	
	i = fiBIntToSInt(b0);
	return fiBIntSIPower(a0, i);
}

FiBInt
fiBIntPowerMod(FiBInt a, FiBInt b, FiBInt c)
{
	FGmpBInt res;
	Bool aImmed, bImmed, cImmed;
	unsigned long bn,cn;

	aImmed = IsImmed(a);
	bImmed = IsImmed(b);
	cImmed = IsImmed(c);
	if (aImmed) a = (FiBInt) fgmpFrInt(FGmpBIntToInt( (FGmpBInt)a));
	if (bImmed) bn = FGmpBIntToInt( (FGmpBInt)b);
	if (cImmed) c = (FiBInt) fgmpFrInt(FGmpBIntToInt( (FGmpBInt)c));

	res = fgmpBIntNew();
	if (bImmed) 
	  mpz_powm_ui(F2MPZ(res), B2MPZ(a), bn ,B2MPZ(c));
	else 
	  mpz_powm(F2MPZ(res), B2MPZ(a), B2MPZ(b),B2MPZ(c));
	
	if (aImmed) fiBIntFree(a);
	if (cImmed) fiBIntFree(c);
	
	return (FiBInt) fgmpImmedIfCan(res);

}

void
fiScanBInt(FiArr s, FiSInt i, FiBInt *r0, FiSInt *r1)
{
	FGmpBInt x;
	char sbuf[20], *buf, *str;
	FiSInt j;
	j = 0;
	str = (char*)s;
	/* NB: Won't work for radix integers... */
	while (isdigit(str[i+j])) j++;

	*r1 = j;

	if (j < 20) 
		buf = sbuf;
	else 
		buf = (char*) fiAlloc(j+1);

	buf[j] = 0;
	for (j=j-1; j>=0; j--) 
		buf[j] = str[i+j];

	x = fgmpBIntNew();
	mpz_set_str(F2MPZ(x), buf, 10);
	if (buf != sbuf) fiFree(buf);

	*r0 = (FiBInt) fgmpImmedIfCan(x);
}

FiSInt
fiFormatBInt(FiBInt x, FiArr s, FiSInt i)
{
	String	buf;
	int	l;

	buf = fiBIntToString((FiBInt) x);
	l   = strlen(buf);
	if (l+i > strlen((String) s)) {
		fprintf(stderr, "Error -- not enough space!\n");
		exit(1);
	}
	memmove((String) s + i, buf, l+1);
	return (FiSInt) l + i;
}

char *
fiBIntToString(FiBInt x)
{
	char *str;
	Bool xImmed;
	xImmed = IsImmed(x);

	SmallOp(9,x,x);

	if (xImmed) x = (FiBInt) fgmpFrInt(FGmpBIntToInt( (FGmpBInt)x));
	str = mpz_get_str(NULL, 10, B2MPZ(x));

	if (xImmed) fiBIntFree(x);
	return str;
}

FiBInt
fiSIntToBInt(FiSInt i)
{
	return fiBIntFrInt((long) i);
}

FiSInt
fiBIntToSInt(FiBInt b)
{
	if (IsImmed(b))
		return FGmpBIntToInt( (FGmpBInt)b);
	return mpz_get_si(B2MPZ(b));
}

FiSFlo
fiBIntToSFlo(FiBInt b)
{
	double foo;
	if (IsImmed(b)) return (double) FGmpBIntToInt((FGmpBInt)b);

	foo = mpz_get_d(B2MPZ(b));
	return (FiSFlo) foo;
}

FiDFlo
fiBIntToDFlo(FiBInt b)
{
	double foo;
	if (IsImmed(b)) return (double) FGmpBIntToInt((FGmpBInt)b);

	foo = mpz_get_d(B2MPZ(b));
	return (FiDFlo) foo;
}


#define fiRadixChar	('r')

/* Scan an UNSIGNED radix integer */
FiBInt
fiArrToRadixBInt(FiArr s)
{
	FGmpBInt retval;
	String	rpos = (String)NULL;
	String	num, end;
	FiWord	rlen, radix = 10;


	/* Note the start position */
	num = (String)s;


	/* Skip over leading whitespace */
	while (isspace(*num))
		num++;


	/*
	 * Locate the end of the string: start by skipping over
	 * the leading part (which must be in base 10). This
	 * might be the radix or it might be the whole part.
	 */
	for (end = num; isdigit(*end); end++);


	/* Is there a radix marker? */
	if (*end == fiRadixChar)
		rpos = end;


	/* There must be something before the radix marker */
	if (rpos == num)
		return fiSIntToBInt(0); /* Bad number */


	/* Extract the radix (if any) */
	if (rpos)
	{
		/* Extract the radix as a (small) decimal integer */
		rlen  = rpos - num;
		radix = fiScanSmallIntFrString(num, rlen, 10);


		/* Valid radix: 2-36 inclusive */
		if (errno || (radix < 2) || (radix > 36))
			return fiSIntToBInt(0);


		/* Move to the start of the whole part */
		num = rpos + 1;
	}


	/* Scan the radix integer */
	retval = fgmpBIntNew();
	mpz_set_str(F2MPZ(retval), num, radix);
	return (FiBInt)fgmpImmedIfCan(retval);
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
FiBInt
fiArrToBInt(FiArr s)
{
	return fiArrToRadixBInt(s);
}

void
fiBIntFree(FiBInt b)
{
	if (IsImmed(b)) return;

	fgmpBIntFree((FGmpBInt) b);
}

/****************************************************************************
 *
 * foam_c.c:  Run time support for C version of abstract machine.
 *
 * Copyright (c) 1990-2007 Aldor Software Organization Ltd (Aldor.org).
 *
 ****************************************************************************/

/*
 * Fi is the acronym for Foam Implementation
 */

/*
 * The include of "foam_c.h" must come last.
 */
#include "axlgen.h"
#include "dword.h"
#include "foam_c.h"
#include "opsys.h"
#include "store.h"
#include "table.h"
#include "util.h"
#include "xfloat.h"

local Hash	localStrHash	(String); 
/*****************************************************************************
 *
 * :: Things only used for the FOAM run-time system.
 *    These should not be used when foam_c.c is linked into the Aldor compiler.
 *
 *****************************************************************************/

#ifdef FOAM_RTS
int
strLength(s)
	char	*s;
{
	return strlen(s);
}
#endif /* FOAM_RTS */

/******************************************************************************
 *
 * :: Generic Macros
 *
 *****************************************************************************/

#define FiSIntBits	(int)bitsizeof(FiSInt)
#define FiSFloBits	(int)bitsizeof(FiSFlo)
#define FiDFloBits	(int)bitsizeof(FiDFlo)


/*****************************************************************************
 *
 * :: Machine-specific optimisations
 *
 *****************************************************************************/

/*#include "foamopt.c"*/

/*****************************************************************************
 *
 * :: Command line support
 *
 *****************************************************************************/

char *x[] = {"-", NULL};
char **mainArgv = x;
FiWord mainArgc = 1;

/*****************************************************************************
 *
 * :: Storage management
 *
 *****************************************************************************/

#ifdef STO_CENSUS
# define  FI_ALLOC(n,t)		stoAlloc(t, (unsigned long) (n))
#else
# define  FI_ALLOC(n,t)		stoAlloc(OB_Other, (unsigned long) (n))
#endif
# define  FI_FREE(d)		stoFree(d)

double *
fiAlloc(unsigned long n)
{
	return (double *) FI_ALLOC(n, CENSUS_Unknown);
}

double *
fi0Alloc(unsigned long n, int t)
{
	return (double *) FI_ALLOC(n, t);
}

/*
 * This function exists to allow the source of record
 * allocations to be determined from gprof output. With
 * the exception of big integers, FiRec values are one
 * of the most common method of store allocation.
 */
double *
fi0RecAlloc(unsigned long n, int t)
{
	return (double *) FI_ALLOC(n, t);
}

void
fiFree(Ptr p)
{
	FI_FREE(p);
}

/*****************************************************************************
 *
 * :: Closures and functions
 *
 *****************************************************************************/

FiPtr
fiEnvMake(FiSInt size)
{
	/* Never used */
	return (FiPtr) FI_ALLOC(size, CENSUS_EnvLevel);
}

FiEnv
fiEnvPushFun(FiPtr a, FiEnv b)
{
	FiEnv	e;

	e = (FiEnv) FI_ALLOC(sizeof(*e), CENSUS_EnvInfo);
	e->level = a;
	e->next = b;
	e->info = (FiWord) NULL;
	return e;
}

FiClos	
fiClosMakeFun(FiEnv e, FiProg p)
{
	FiClos	c;

	c = (FiClos) FI_ALLOC(sizeof(*c), CENSUS_Clos);
	c->prog = p;
	c->env = e;
	return c;
}

void
fiEnvEnsureFun(FiEnv e)
{ 
	FiWord tmp = (e)->info; 
	if ((FiClos) tmp != NULL) 
		fiCCall0(FiWord, ((FiClos) tmp));
}

/*****************************************************************************
 *
 * :: Char operations
 *
 *****************************************************************************/

FiChar
fiCharMin(void)
{
	return (FiChar) CHAR_MIN;
}

FiChar
fiCharMax(void)
{
	return (FiChar) CHAR_MAX;
}

FiBool
fiCharIsDigit(FiChar c)
{
 	return (isdigit(c) != 0);
}

FiBool
fiCharIsLetter(FiChar c)
{
	return (isalpha(c) != 0);
}

FiChar
fiCharLower(FiChar c)
{
 	return (FiChar) tolower(c);
}

FiChar
fiCharUpper(FiChar c)
{
 	return (FiChar) toupper(c);
}

/*****************************************************************************
 *
 * :: SFlo operations
 *
 *****************************************************************************/

FiSFlo
fiSFloMin(void)
{
	return (FiSFlo) FLT_MIN;
}

FiSFlo
fiSFloMax(void)
{
	return (FiSFlo) FLT_MAX;
}

FiSFlo
fiSFloEpsilon(void)
{
	return (FiSFlo) FLT_EPSILON;
}

/* -- Next / Prev --*/

FiSFlo
fiSFloNext(FiSFlo sf)
{
	int	i, expon;
	Bool	sign;
	UByte	pb[sizeof(FiSFlo)];

	if (sf < 0.0)
		return - fiSFloPrev(-sf);

	sfDissemble(&sf, &sign, &expon, pb,NULL);

#ifdef SF_NoDenorms
	if (sf == 0.0) {
		sfAssemble(&sf, int0, 1, pb);
		return sf;
	}
#endif	

	expon += SF_FracOff;

	bfShiftDn(sizeof(FiSFlo), pb, SF_FracOff, pb, int0, SF_HasNorm1);

	for (i = sizeof(FiSFlo)-1;; i--) {
		pb[i] += 1;
		if (pb[i]) break;
	}

	fracNormalize(&expon, sizeof(FiSFlo), pb);

	if (expon < -SF_Excess) return 0.0;

	sfAssemble(&sf, int0, expon, pb);

	return sf;
}

FiSFlo
fiSFloPrev(FiSFlo sf)
{
	int	i, expon;
	Bool	sign;
	UByte	pb[sizeof(FiSFlo)];

	if (sf <= 0.0)
		return - fiSFloNext(-sf);

	sfDissemble(&sf, &sign, &expon, pb,NULL);

	expon += SF_FracOff;

	bfShiftDn(sizeof(FiSFlo), pb, SF_FracOff, pb, int0, SF_HasNorm1);

	for (i = sizeof(FiSFlo)-1;; i--) {
		pb[i] -= 1;
		if (pb[i] != 0xff) break;
	}

	fracNormalize(&expon, sizeof(FiSFlo), pb);

#ifdef SF_NoDenorms
	if (expon == -SF_Excess) return 0.0;
#endif	
	if (expon < -SF_Excess) return 0.0;

	sfAssemble(&sf, int0, expon, pb);

	return sf;
}

/* -- Rounded operations. "d" is the rounding direction (see foam_c.h) --*/

local FiSFlo
fiSFloRId(FiSFlo sf, FiSInt direction)
{
	FiSFlo res;
	switch ((int)direction) {
	case fiRoundZero():
		res = (sf > 0? fiSFloPrev(sf) : fiSFloNext(sf));
		break;
	case fiRoundNearest():
		res =  sf;
		break;
	case fiRoundUp():
		res = fiSFloNext(sf);
		break;
	case fiRoundDown():
		res = fiSFloPrev(sf);
		break;
	default:
	        res =  sf;
		break;
	}
	return res;
}

FiSFlo
fiSFloRPlus(FiSFlo x,FiSFlo y,FiSInt d)
{
	FiSFlo res = x + y;

	return fiSFloRId(res, d);
}

FiSFlo
fiSFloRMinus(FiSFlo x,FiSFlo y,FiSInt d)
{
	return fiSFloRId(x - y, d);
}

FiSFlo
fiSFloRTimes(FiSFlo x,FiSFlo y,FiSInt d)
{
	return fiSFloRId(x * y, d);
}

FiSFlo
fiSFloRTimesPlus(FiSFlo x,FiSFlo y,FiSFlo z,FiSInt d)
{
	return fiSFloRId(x*y + z, d);
}

FiSFlo
fiSFloRDivide(FiSFlo x,FiSFlo y,FiSInt d)
{
	
	return fiSFloRId(x / y, d);
}

/* --- SFloTruncate, SFloFraction, SFloNearest --- */
FiBInt
fiSFloTruncate(FiSFlo sf)
{
	int	i, j, expon;
	Bool	sign, sfHasNorm1 = SF_HasNorm1;
	long	res;
	FiBInt	result = NULL;
	long	tmp = 0;
	UByte	pb[sizeof(FiSFlo)];

	sfDissemble(&sf, &sign, &expon, pb,NULL);


	/* NOTE: the mantissa in `pb' is the same on little/big endian hw. */
	if (expon == 0) {
		if (sfHasNorm1)
			result = (FiBInt) (sign ? fiBIntNew(-1) : fiBInt1());
		else
			result = (FiBInt) fiBInt0();
	}
	else if (expon < 0)
		result = (FiBInt) fiBInt0();


	/* Does the value fit in an immediate bigint? (`-1' is for the sign) */
	else if (expon < bitsizeof(FiSFlo) - 1) {

		bfShiftDn(sizeof(FiSFlo), pb, (FiSFloBits - expon),
		          pb, int0, SF_HasNorm1);
		
		if (sizeof(long) > sizeof(FiSFlo)) {
			for (i = sizeof(long) - sizeof(FiSFlo),
			     j = 0;
			     j < sizeof(FiSFlo);
			     i++, j++)
				FiWord_UByte(&tmp, i) = pb[j];
		}
		else {
			for (i = 0, j = (int)sizeof(FiSFlo) - (int)sizeof(long);
			     i < sizeof(FiSFlo); i++, j++)
				FiWord_UByte(&tmp, i) = pb[j];
		}

		res = (sign ? -tmp : tmp);

		result = (FiBInt) fiBIntNew(res);
	}
	else /* expon => FiSIntBits -1 */ {

	        /* PI: Don't use fiBInt0()! "bres" is assigned to "btmp2"
		 * and then released.
		 */
		FiBInt	bres = fiBIntNew(int0);
		FiBInt	btmp0, btmp1, btmp2;

		for (i = 0, j = (sizeof(FiSFlo)-1) * CHAR_BIT;
		     i < sizeof(FiSFlo); i++,j -= CHAR_BIT) {

			btmp0 = fiBIntNew(pb[i]);
			btmp1 = fiBIntShiftUp(btmp0, j);
			btmp2 = bres;
			bres  = fiBIntPlus(btmp2, btmp1);

			fiBIntFree(btmp0);
			fiBIntFree(btmp1);
			fiBIntFree(btmp2);
		}

		if (sfHasNorm1) {
			btmp0 = fiBIntShiftUp(fiBInt1(), FiSFloBits);
			btmp2 = bres;
			bres = fiBIntPlus(btmp2, btmp0);
			fiBIntFree(btmp0);
			fiBIntFree(btmp2);
		}
		
		btmp0 = bres;
		if (expon > FiSFloBits)
			bres = fiBIntShiftUp(btmp0, expon - FiSFloBits);
		else
			bres = fiBIntShiftDn(btmp0, FiSFloBits - expon);
			
		fiBIntFree(btmp0);

		if (sign)   {
			btmp0 = bres;
			bres = fiBIntNegate(btmp0);
			fiBIntFree(btmp0);
		}

		result = (FiBInt) bres;
	}

	return result;
}

FiSFlo
fiSFloFraction(FiSFlo sf)
{
	int	i, j, expon;
	Bool	sign;
	UByte	pb[sizeof(FiSFlo)];

	sfDissemble(&sf, &sign, &expon, pb,NULL);

	if (expon >= (int) (FiSFloBits - SF_FracOff))
		return 0.0;

	else if (expon == -SF_Excess)
		return 0.0;

	else if (expon < 0)
		return sf;

	else /* 0 <= expon <= sizeof(Mantissa) */ {

		UByte 	mask = 0x7f;
		int 	nbytes = expon / CHAR_BIT;
		int	nbits  = expon % CHAR_BIT;
		int	ix1;

		for (i = 0; i < nbytes; i++)
			pb[i] = 0;

		for (j = 0; j < nbits; j++) {
			pb[i] &= mask;
			mask >>= 1;
		}

		ix1 = bfFirst1(sizeof(FiSFlo), pb);

		if (ix1 == -1) return 0.0;

		/* Shift first explicit 1 out the most significant end. */
		bfShiftUp(sizeof(FiSFlo), pb, ix1 + 1, pb, int0);
		expon -= ix1 + 1;

		sfAssemble(&sf, sign, expon, pb);

		return sf;
	}
	return sf;
}

local FiBInt
fiSFloNearest(FiSFlo sf)
{
	FiSFlo	frac = fiSFloFraction(sf);
	FiSFlo	delta;

	delta = (frac < -0.5) ? (FiSFlo)-1.0 : (frac > 0.5) ? (FiSFlo)1.0 : (FiSFlo)0.0;
	return fiSFloTruncate(sf + delta);
}


FiBInt
fiSFloRound(FiSFlo sf, FiSInt direction)
{
        FiBInt res;
	switch ((int)direction) {
	case fiRoundZero():
		res = fiSFLO_ROUND_ZERO(sf);
		break;
	case fiRoundNearest():
		res = fiSFLO_ROUND_NEAREST(sf);
		break;
	case fiRoundUp():
		res = fiSFLO_ROUND_UP(sf);
		break;
	case fiRoundDown():
		res = fiSFLO_ROUND_DOWN(sf);
		break;
	default:
		res = fiSFLO_ROUND(sf);
		break;
	}
	return res;
}

/* -- Dissemble, Assemble: box and unbox a FiSFlo -- */

void
fiSFloDissemble(FiSFlo sf, FiBool *psign, FiSInt *pexpon, FiWord *psig0)
{
	Bool	sign;
	int	expon;

	sfDissemble(&sf, &sign, &expon, (UByte *)psig0, NULL);

	/* FiBool and Bool are not equivalent, nor are FiSInt and int. */
	*psign  = (FiBool)sign;
	*pexpon = (FiSInt)expon;
}

FiSFlo
fiSFloAssemble(FiBool sign,FiSInt  exponent,FiWord sig0)
{
	FiSFlo res;

	sfAssemble(&res, sign,(int) exponent, (UByte *) &sig0);

	return res;
}

/*****************************************************************************
 *
 * :: DFlo operations
 *
 *****************************************************************************/

FiDFlo
fiDFloMin(void)
{
	return (FiDFlo) DBL_MIN;
}

FiDFlo
fiDFloMax(void)
{
	return (FiDFlo) DBL_MAX;
}

FiDFlo
fiDFloEpsilon(void)
{
	return (FiDFlo) DBL_EPSILON;
}

/* -- Next / Prev --*/

FiDFlo
fiDFloNext(FiDFlo df)
{
	int	i, expon;
	Bool	sign;
	UByte	pb[sizeof(FiDFlo)];

	if (df < 0.0)
		return - fiDFloPrev(-df);

	dfDissemble(&df, &sign, &expon, pb,NULL);

	expon += DF_FracOff;

	bfShiftDn(sizeof(FiDFlo), pb, DF_FracOff, pb, int0, DF_HasNorm1);

	for (i = sizeof(FiDFlo)-1;; i--) {
		pb[i] += 1;
		if (pb[i]) break;
	}

	fracNormalize(&expon, sizeof(FiDFlo), pb);

	if (expon < -DF_Excess) return 0.0;

	dfAssemble(&df, int0, expon, pb);

	return df;
}

FiDFlo
fiDFloPrev(FiDFlo df)
{
	int	i, expon;
	Bool	sign;
	UByte	pb[sizeof(FiDFlo)];

	if (df <= 0.0)
		return - fiDFloNext(-df);

	dfDissemble(&df, &sign, &expon, pb,NULL);

	expon += DF_FracOff;

	bfShiftDn(sizeof(FiDFlo), pb, DF_FracOff, pb, int0, DF_HasNorm1);

	for (i = sizeof(FiDFlo)-1;; i--) {
		pb[i] -= 1;
		if (pb[i] != 0xff) break;
	}

	fracNormalize(&expon, sizeof(FiDFlo), pb);

	if (expon < -DF_Excess) return 0.0;

	dfAssemble(&df, int0, expon, pb);

	return df;
}

/* -- Rounded operations. "d" is the rounding direction (see foam_c.h) --*/

local FiDFlo
fiDFloRId(FiDFlo df,FiSInt  direction)
{
        FiDFlo res;
	switch ((int)direction) {
	case fiRoundZero():
		res = (df > 0? fiDFloPrev(df) : fiDFloNext(df));
		break;
	case fiRoundNearest():
		res = df;
		break;
	case fiRoundUp():
		res = fiDFloNext(df);
		break;
	case fiRoundDown():
		res = fiDFloPrev(df);
		break;
	default:
	        res = df;
		break;
	}
	return res;
}

FiDFlo
fiDFloRPlus(FiDFlo x,FiDFlo y,FiSInt d)
{
	return fiDFloRId(x + y, d);
}

FiDFlo
fiDFloRMinus(FiDFlo x,FiDFlo y,FiSInt d)
{
	return fiDFloRId(x - y, d);
}

FiDFlo
fiDFloRTimes(FiDFlo x,FiDFlo y,FiSInt d)
{
	return fiDFloRId(x * y, d);
}

FiDFlo
fiDFloRTimesPlus(FiDFlo x,FiDFlo y,FiDFlo z,FiSInt d)
{
	return fiDFloRId(x*y + z, d);
}

FiDFlo
fiDFloRDivide(FiDFlo x,FiDFlo y,FiSInt d)
{
	return fiDFloRId(x / y, d);
}

/* --- DFloTruncate, DFloFraction, DFloNearest --- */

FiBInt
fiDFloTruncate(FiDFlo df)
{
	int	i, j, expon;
	Bool	sign, dfHasNorm1 = DF_HasNorm1;
	FiSInt	res;
	FiBInt	result = NULL;
	FiSInt	tmp = 0;
	UByte	pb[sizeof(FiDFlo)];

	dfDissemble(&df, &sign, &expon, pb,NULL);


	/* NOTE: the mantissa in `pb' is the same on little/big endian hw. */
	if (expon == 0) {
		if (dfHasNorm1)
			result = (FiBInt) (sign ? fiBIntNew(-1) : fiBInt1());
		else
			result = (FiBInt) fiBInt0();
	}
	else if (expon < 0)
		result = (FiBInt) fiBInt0();

	/* Does the value fit in a FiSInt? (`-1' is for the sign) */
	else if (expon < FiSIntBits - 1) {

		bfShiftDn(sizeof(FiDFlo), pb, (FiDFloBits - expon),
		          pb, int0, DF_HasNorm1);

		for (i = 0, j = sizeof(FiDFlo) - sizeof(FiSInt);
		     i < sizeof(FiSInt); i++, j++)
			FiWord_UByte(&tmp, i) = pb[j];

		res = (sign ? -tmp : tmp);

		result = (FiBInt) fiBIntNew(res);
	}
	else /* expon => FiSIntBits -1 */ {

	        /* PI: Don't use fiBInt0()! "bres" is assigned to "btmp2"
		 * and then released.
		 */
		FiBInt	bres = fiBIntNew(int0);
		FiBInt	btmp0, btmp1, btmp2;

		for (i = 0, j = (sizeof(FiDFlo)-1) * CHAR_BIT;
		     i < sizeof(FiDFlo); i++,j -= CHAR_BIT) {

			btmp0 = fiBIntNew(pb[i]);
			btmp1 = fiBIntShiftUp(btmp0, j);
			btmp2 = bres;
			bres  = fiBIntPlus(btmp2, btmp1);

			fiBIntFree(btmp0);
			fiBIntFree(btmp1);
			fiBIntFree(btmp2);
		}

		if (dfHasNorm1) {
			btmp0 = fiBIntShiftUp(fiBInt1(), FiDFloBits);
			btmp2 = bres;
			bres = fiBIntPlus(btmp2, btmp0);
			fiBIntFree(btmp0);
			fiBIntFree(btmp2);
		}
		
		btmp0 = bres;
		if (expon > FiDFloBits)
			bres = fiBIntShiftUp(btmp0, expon - FiDFloBits);
		else
			bres = fiBIntShiftDn(btmp0, FiDFloBits - expon);
		fiBIntFree(btmp0);

		if (sign)   {
			btmp0 = bres;
			bres = fiBIntNegate(btmp0);
			fiBIntFree(btmp0);
		}

		result = (FiBInt) bres;
	}

	return result;
}


FiDFlo
fiDFloFraction(FiDFlo df)
{
	int i, j, expon;
	Bool sign;
	UByte  pb[sizeof(FiDFlo)];
	dfDissemble(&df, &sign, &expon, pb,NULL);

	if (expon >= (int) (FiDFloBits - DF_FracOff))
	        return 0.0;

	else if (expon == -DF_Excess)
		return 0.0;

	else if (expon < 0)
		return df;

	else /* 0 <= expon <= sizeof(Mantissa) */ {

		UByte 	mask = 0x7f;
		int 	nbytes = expon / CHAR_BIT;
		int	nbits  = expon % CHAR_BIT;
		int	ix1;

		for (i = 0; i < nbytes; i++)
			pb[i] = 0;

		for (j = 0; j < nbits; j++) {
			pb[i] &= mask;
			mask >>= 1;
		}

		ix1 = bfFirst1(sizeof(FiDFlo), pb);

		if (ix1 == -1) return 0.0;

		/* Shift first explicit 1 out the most significant end. */
		bfShiftUp(sizeof(FiDFlo), pb, ix1 + 1, pb, int0);
		expon -= ix1 + 1;

		dfAssemble(&df, sign, expon, pb);

		return df;
	}
	return df;
}

local FiBInt
fiDFloNearest(FiDFlo df)
{
	FiDFlo	frac = fiDFloFraction(df);
	FiDFlo	delta;

	delta = (frac < -0.5) ? -1.0 : (frac > 0.5) ? 1.0 : 0.0;
	return fiDFloTruncate(df + delta);
}


FiBInt
fiDFloRound(FiDFlo df,FiSInt direction)
{
        FiBInt res;
	switch ((int)direction) {
	case fiRoundZero():
		res= fiDFLO_ROUND_ZERO(df);
		break;
	case fiRoundNearest():
		res= fiDFLO_ROUND_NEAREST(df);
		break;
	case fiRoundUp():
		res= fiDFLO_ROUND_UP(df);
		break;
	case fiRoundDown():
		res= fiDFLO_ROUND_DOWN(df);
		break;
	default:
		res= fiDFLO_ROUND(df);
		break;
	}
	return res;
}

/* -- Dissemble, Assemble: box and unbox a FiDFlo -- */

void
fiDFloDissemble(FiDFlo df, FiBool *psign, FiSInt *pexpon, FiWord *psig0, FiWord *psig1)
{
	Bool	sign;
	int	expon;
	FiWord	fracb[2];

 	dfDissemble(&df, &sign, &expon, (UByte *)fracb, NULL);

	/* FiBool and Bool are not equivalent, nor are FiSInt and int. */
	*psign  = (FiBool)sign;
	*pexpon = (FiSInt)expon;
	*psig0  = fracb[0]; 
	*psig1  = fracb[1];
}

FiDFlo
fiDFloAssemble(FiBool sign, FiSInt expon,FiWord  sig0,FiWord  sig1)
{
	FiDFlo	res;
	FiWord  fracb[2];

	fracb[0] = sig0;
	fracb[1] = sig1;

	dfAssemble(&res, sign,(int) expon, (UByte *) fracb);

	return res;
}

FiBInt
fiDFloExponent(FiDFlo df)
{
	int exp;
	Bool iszero;
	dfDissemble(&df, NULL, &exp, NULL,&iszero);

	if (iszero) return fiBIntNew(exp);      
	return fiBIntNew(exp - 64);
}

FiBInt
fiDFloMantissa(FiDFlo df)
{
	Bool   sign,iszero;
	int    exp;
	UByte  mantissa[sizeof(FiDFlo)];
	FiBInt bint;
	int    i;

	dfDissemble(&df, &sign, &exp, mantissa,&iszero);

        if(iszero) {
                bint = fiBIntNew((long) 0);
                return bint;
                }
	
	bint = fiBIntNew(1);
	for (i=0; i<sizeof(FiDFlo); i++)
		bint = fiBIntPlus(fiBIntTimes(fiBIntNew(256), bint), fiBIntNew(mantissa[i]));

	return sign?fiBIntNegate(bint):bint;
}

FiBInt
fiSFloExponent(FiSFlo sf)
{
	int exp;
	Bool iszero;
	sfDissemble(&sf, NULL, &exp, NULL,&iszero);

	if (iszero) return fiBIntNew(exp);	
	return fiBIntNew(exp - 8*sizeof(FiSFlo));
}

FiBInt
fiSFloMantissa(FiSFlo sf)
{
	Bool   sign,iszero;
	int    exp;
	UByte  mantissa[sizeof(FiSFlo)];
	FiBInt bint;
	int    i;

	sfDissemble(&sf, &sign, &exp, mantissa,&iszero);

	if(iszero) {
		bint = fiBIntNew((long) 0);
		return bint;
		}
	bint = fiBIntNew(1);
	for (i=0; i<sizeof(FiSFlo); i++)
		bint = fiBIntPlus(fiBIntTimes(fiBIntNew(256), bint), fiBIntNew(mantissa[i]));

	return sign?fiBIntNegate(bint):bint;
}

/*****************************************************************************
 *
 * :: (Unsigned) Byte operations
 *
 *****************************************************************************/

#if AXL_EDIT_1_1_12p6_19
FiByte
fiByteMin(void)
{
	return (FiByte)0; /*SCHAR_MIN;*/
}

FiByte
fiByteMax(void)
{
	return (FiByte)UCHAR_MAX; /*SCHAR_MAX;*/
}
#else
FiByte
fiByteMin(void)
{
	return (FiByte) SCHAR_MIN;
}

FiByte
fiByteMax(void)
{
	return (FiByte) SCHAR_MAX;
}
#endif

/*****************************************************************************
 *
 * :: HInt operations
 *
 *****************************************************************************/

FiHInt
fiHIntMin(void)
{
	return (FiHInt) SHRT_MIN;
}

FiHInt
fiHIntMax(void)
{
	return (FiHInt) SHRT_MAX;
}

/*****************************************************************************
 *
 * :: SInt operations
 *
 *****************************************************************************/

FiSInt
fiSIntMin(void)
{
	return (FiSInt) LONG_MIN;
}

FiSInt
fiSIntMax(void)
{
	return (FiSInt) LONG_MAX;
}

void
fiSIntDivide(FiSInt a,FiSInt b,FiSInt *  r0,FiSInt * r1)
{
	*r0 = (FiSInt) a / b;
	*r1 = (FiSInt) a % b;
	return;
}

FiSInt
fiSIntGcd(FiSInt a,FiSInt b)
{
	if (a < 0) a = -a;
	if (b < 0) b = -b;

	while (b != 0) {
		FiSInt t = a;
		a = b;
		b = t % b;
	}
	return a;
}

FiSInt	
fiSIntLength(FiSInt i)
{
	long		b;
	unsigned long	x;

	x = (i < 0) ? (-i) : i;

	for (b = 0; x != 0; x >>= 1) b++;
	return (FiSInt) b;
}

FiSInt
fiSIntTimesMod(FiSInt a,FiSInt  b,FiSInt m)
{
	/*!! Not yet implemented */
	return 0;
}

FiSInt
fiSIntTimesModInv(FiSInt a,FiSInt  b,FiSInt  m,FiDFlo  mi)
{
	/*!! Not yet implemented */
	return 0;
}

/*****************************************************************************
 *
 * :: Word operations
 *
 *****************************************************************************/

#ifndef OPT_NoDoubleOps
void
fiWordTimesDouble(FiWord a,FiWord  b,FiWord * pnhi,FiWord * pnlo)
{
	ULong p0, p1;

	xxTimesDouble(&p0, &p1, a, b);

	*pnhi = (FiWord) p0;
	*pnlo = (FiWord) p1;

	return;
}

void
fiWordDivideDouble(FiWord nhi,FiWord nlo,FiWord d,FiWord * pqhi,FiWord * pqlo,FiWord * pr)
{
	ULong p0, p1, p2;

	xxDivideDouble(&p0, &p1, &p2, nhi, nlo, d);

	*pqhi = (FiWord) p0;
	*pqlo = (FiWord) p1;
	*pr   = (FiWord) p2;

	return;
}
#endif

void
fiWordPlusStep(FiWord a,FiWord  b,FiWord kin,FiWord * pkout,FiWord * pr)
{
	ULong	p0, p1;

	xxPlusStep(&p0, &p1, (ULong) a, (ULong) b, (ULong) kin);
	
	*pkout = (FiWord) p0;
	*pr    = (FiWord) p1;

	return;
}

void
fiWordTimesStep(FiWord a,FiWord  b,FiWord  c,FiWord kin,FiWord * pkout,FiWord * pr)
{
	ULong	p0, p1;

	xxTimesStep(&p0, &p1, (ULong) a, (ULong) b, (ULong) c, (ULong) kin);

	*pkout = (FiWord) p0;
	*pr    = (FiWord) p1;

	return;
}


/*****************************************************************************
 *
 * :: Arr operations
 *
 *****************************************************************************/

FiChar *
fiArrNew_Char(unsigned long arrSize)
{
	int	i;
	FiChar	*array;

	array = (FiChar *) FI_ALLOC(sizeof(FiChar)*arrSize, CENSUS_CharArray);
	for (i = 0; i < arrSize; i++)
		array[i] = (FiChar)0;
	return array;
}

FiBool *
fiArrNew_Bool(unsigned long  arrSize)
{
	int	i;
	FiBool	*array;

	array = (FiBool *) FI_ALLOC(sizeof(FiBool)*arrSize, CENSUS_BoolArray);
	for (i = 0; i < arrSize; i++)
		array[i] = (FiBool) 0;
	return array;
}

FiByte *
fiArrNew_Byte(unsigned long arrSize)
{
	int	i;
	FiByte	*array;

	array = (FiByte *) FI_ALLOC(sizeof(FiByte)*arrSize, CENSUS_ByteArray);
	for (i = 0; i < arrSize; i++)
		array[i] = (FiByte) 0;
	return array;
}

FiHInt *
fiArrNew_HInt(unsigned long arrSize)
{
	int	i;
	FiHInt	*array;

	array = (FiHInt *) FI_ALLOC(sizeof(FiHInt)*arrSize, CENSUS_HIntArray);
	for (i = 0; i < arrSize; i++)
		array[i] = (FiHInt) 0;
	return array;
}

FiSInt *
fiArrNew_SInt(unsigned long arrSize)
{
	int	i;
	FiSInt	*array;

	array = (FiSInt *) FI_ALLOC(sizeof(FiSInt)*arrSize, CENSUS_SIntArray);
	for (i = 0; i < arrSize; i++)
		array[i] = (FiSInt) 0;
	return array;
}

FiSFlo *
fiArrNew_SFlo(unsigned long arrSize)
{
	int	i;
	FiSFlo	*array;

	array = (FiSFlo *) FI_ALLOC(sizeof(FiSFlo)*arrSize, CENSUS_SFloArray);
	for (i = 0; i < arrSize; i++)
		array[i] = (FiSFlo) 0.0;
	return array;
}

FiDFlo *
fiArrNew_DFlo(unsigned long arrSize)
{
	int	i;
	FiDFlo	*array;

	array = (FiDFlo *) FI_ALLOC(sizeof(FiDFlo)*arrSize, CENSUS_DFloArray);
	for (i = 0; i < arrSize; i++)
		array[i] = (FiDFlo) 0.0;
	return array;
}

FiWord *
fiArrNew_Word(unsigned long arrSize)
{
	int	i;
	FiWord	*array;

	array = (FiWord *) FI_ALLOC(sizeof(FiWord)*arrSize, CENSUS_WordArray);
	for (i = 0; i < arrSize; i++)
		array[i] = (FiWord) 0;
	return array;
}

FiPtr *
fiArrNew_Ptr(unsigned long arrSize)
{
	int	i;
	FiPtr	*array;

	array = (FiPtr *) FI_ALLOC(sizeof(FiPtr)*arrSize, CENSUS_PtrArray);
	for (i = 0; i < arrSize; i++)
		array[i] = (FiPtr) 0;
	return array;
}

/*
 * A number of these fiArrNew_*() functions can never be invoked. However,
 * bug fixes and future extensions to the compiler may need them. Thus we
 * define them now to avoid yet more trivial bug reports later. Please do
 * not create more CENSUS_*Array types for them: use CENSUS_PtrArray.
 */
FiNil *
fiArrNew_Nil(unsigned long arrSize)
{
	int	i;
	FiNil	*array;

	array = (FiNil *) FI_ALLOC(sizeof(FiNil)*arrSize, CENSUS_PtrArray);
	for (i = 0; i < arrSize; i++)
		array[i] = (FiNil) 0;
	return array;
}

FiRec *
fiArrNew_Rec(unsigned long arrSize)
{
	int	i;
	FiRec	*array;

	array = (FiRec *) FI_ALLOC(sizeof(FiRec)*arrSize, CENSUS_PtrArray);
	for (i = 0; i < arrSize; i++)
		array[i] = (FiRec) 0;
	return array;
}

FiArr *
fiArrNew_Arr(unsigned long arrSize)
{
	int	i;
	FiArr	*array;

	array = (FiArr *) FI_ALLOC(sizeof(FiArr)*arrSize, CENSUS_PtrArray);
	for (i = 0; i < arrSize; i++)
		array[i] = (FiArr) 0;
	return array;
}

FiArb *
fiArrNew_Arb(unsigned long arrSize)
{
	int	i;
	FiArb	*array;

	array = (FiArb *) FI_ALLOC(sizeof(FiArb)*arrSize, CENSUS_PtrArray);
	for (i = 0; i < arrSize; i++)
		array[i] = (FiArb) 0;
	return array;
}

FiRRec *
fiArrNew_RRec(unsigned long arrSize)
{
	int	i;
	FiRRec	*array;

	array = (FiRRec *) FI_ALLOC(sizeof(FiRRec)*arrSize, CENSUS_PtrArray);
	for (i = 0; i < arrSize; i++)
		array[i] = (FiRRec) 0;
	return array;
}

FiTR *
fiArrNew_TR(unsigned long arrSize)
{
	int	i;
	FiTR	*array;

	array = (FiTR *) FI_ALLOC(sizeof(FiTR)*arrSize, CENSUS_PtrArray);
	for (i = 0; i < arrSize; i++)
		array[i] = (FiTR) 0;
	return array;
}

FiBInt *
fiArrNew_BInt(unsigned long arrSize)
{
	int	i;
	FiBInt	*array;

	array = (FiBInt *) FI_ALLOC(sizeof(FiBInt)*arrSize, CENSUS_PtrArray);
	for (i = 0; i < arrSize; i++)
		array[i] = (FiBInt) 0;
	return array;
}

FiClos *
fiArrNew_Clos(unsigned long arrSize)
{
	int	i;
	FiClos	*array;

	array = (FiClos *) FI_ALLOC(sizeof(FiClos)*arrSize, CENSUS_PtrArray);
	for (i = 0; i < arrSize; i++)
		array[i] = (FiClos) 0;
	return array;
}

/*****************************************************************************
 *
 * :: Format operations
 *
 *****************************************************************************/

# define  MAX_SINT_TEXT	200
# define  MAX_SFLO_TEXT	200
# define  MAX_DFLO_TEXT	200

/*fixes the differences in the exponent output between windows and linux*/
local void
fixFloat(char *buf)
{
	int i, j;
	int len = strlen(buf);
	for (i = j = 0; i < len+1; i++, j++) {
		if ((buf[i] == 'e' || buf[i] == 'E')
		    && buf[i+2] == '0' 
		    && buf[i+3] != '\0'
		    && buf[i+4] != '\0'
		    && buf[i+5] == '\0') {
		        i = i + 2;
			j = j + 3;
		}
		if (i != j) buf[i] = buf[j];
	}
}

FiSInt
fiFormatSFlo(FiSFlo sf,FiArr s,FiSInt i)
{
	char	buf[MAX_SFLO_TEXT];
	unsigned long	l;

	(void)sprintf(buf, "%.9g", sf);
	l   = strlen(buf);
	if (l+i > strlen((String) s)) {
		fiRaiseException((FiWord)"FormatSFlo: the array given is not big enough");
		exit(1);
	}
	fixFloat(buf);
	(void)memmove((String) s + i, buf, l+1);
	return (FiSInt) l + i;
}

FiSInt
fiFormatDFlo(FiDFlo x,FiArr  s,FiSInt i)
{
	char	buf[MAX_DFLO_TEXT];
	unsigned long	l;

	(void)sprintf(buf, "%.17g", x);
	l   = strlen(buf);
	if (l+i > strlen((String) s)) {
		fiRaiseException((FiWord)"FormatDFlo: the array given is not big enough");
		exit(1);
	}
	fixFloat(buf);
	(void)memmove((String) s + i, buf, l+1);
	return (FiSInt) l + i;
}

FiSInt
fiFormatSInt(FiSInt x,FiArr s,FiSInt  i)
{
	char	buf[MAX_SINT_TEXT];
	unsigned long	l;

	(void)sprintf(buf, "%ld", x);
	l   = strlen(buf);
	if (l+i > strlen((String) s)) {
		fiRaiseException((FiWord)"FormatSInt: the array given is not big enough");
		exit(1);
	}
	(void) memmove((String) s + i, buf, l+1);
	return (FiSInt) l + i;
}

void
fiScanSFlo(FiArr s,FiSInt  i,FiSFlo * r0,FiSInt * r1)
{
	/*!!
	int	l;
	double	r;
	l = sscanf(((char *) s) + i, "%f", &r);
	*r0 = r;
	*r1  = l + i;
	*/
	/*!! The following is wrong, but sscanf seems to be broken */
	/* MND: use the %n conversion to get scan position */
	*r0 = (FiSFlo) atof(((char *) s) + i);
	*r1 = strlen(((char *) s) + i) + i;
	return;
}


/*****************************************************************************
 *
 * :: Scan operations
 *
 *****************************************************************************/

void
fiScanDFlo(FiArr s,FiSInt i,FiDFlo * r0,FiSInt * r1)
{
	/*!!
	int	l;
	
	l = sscanf(((char *) s) + i, "%f", r0);
	*r1  = l + i;
	*/
	/*!! The following is wrong, but sscanf seems to be broken */
	/* MND: use the %n conversion to get scan position */
	*r0 = atof(((char *) s) + i);
	*r1 = strlen(((char *) s) + i) + i;
	return;
}

void
fiScanSInt(FiArr s,FiSInt i,FiSInt * r0,FiSInt * r1)
{
	String rest;
	long int l;
	l = strtol((String) s + i, &rest, 10);
	*r0 = l;
	*r1  = rest - (String) s;
	return;
}


/*****************************************************************************
 *
 * :: Conversion operations
 *
 * They must be able to accept numbers using the Aldor number format since
 * the compiler doesn't perform any conversions. The standard number domains
 * simply pass the literal on to convert()$Machine which drops them here.
 *
 * General Aldor number format is RRrWW.FFeSXX where
 *
 *    RR  is the radix part.
 *    'r' is the radix character.
 *    WW  is the whole part.
 *    '.' is the point character.
 *    FF  is the fractional part.
 *    'e' is the exponent character.
 *    S   is the sign character.
 *    XX  is the exponent part.
 *
 *****************************************************************************/

#define fiRadixChar	('r')
#define fiRadixBits	(sizeof(FiWord) << 3)
/* Note: << 3 assumes 8 bits per byte */


/*
 * Extract a small unsigned integer of specified radix from a
 * string. The first `ndigs' of the string represent a valid
 * number. On failure we return 0 and may update errno. Note
 * that this function is also used by the bigint scanners.
 */
FiWord
fiScanSmallIntFrString(char * start,FiWord  ndigs,FiWord  radix)
{
	char 	*junk;
	FiWord	ires;
	FiWord	slen;
	char	num[fiRadixBits + 1];

	/*
	 * Copy out the number to be scanned. Since we know
	 * that the number is small, we know the maximum bit
	 * length. This means that we can allocate temporary
	 * storage statically which is good. However, we can
	 * never trust anyone to get things right so we have
	 * a quick test to prevent buffer overflow.
	 */
	assert(ndigs <= fiRadixBits);
	slen = (ndigs <= fiRadixBits) ? ndigs : fiRadixBits;
	(void) strncpy(num, start, slen);
	num[slen] = '\0';


	/* Convert the string into an integer */
	errno = 0;
	ires  = strtol(num, &junk, (int)radix);


	/* Were all the characters valid? */
	if (!errno && *junk) errno = EDOM;


	/* Return the result */
	return ires;
}


FiSFlo
fiArrToSFlo(FiArr s)
{
	return (FiSFlo) atof((String) s);
}


FiDFlo
fiArrToDFlo(FiArr s)
{
	return (FiDFlo) atof((String) s);
}


/*
 * Convert an integer literal of the form RRrWW into an integer. If
 * the literal didn't conform to our expectations then 0 is returned
 * and errno set to EDOM or a value determined by strtol. If the
 * result is too large then the largest possible number is returned
 * and errno will probably be set to ERANGE.
 */
FiSInt
fiArrToSInt(FiArr s)
{
	String num, rpos, rest;
	FiSInt retval;
	FiSInt rlen;
	int radix = 10;


	/* We want to work with String without coercions */
	num = (String)s;


	/* Reset errno */
	errno = 0;


	/* Does this number have an explicit radix? */
	rpos = strchr(num, (int)fiRadixChar);


	/* There must be something before the radix marker */
	if (rpos == num)
	{
		/* Bad number */
		errno = EDOM;
		return (FiSInt)0;
	}


	/* Extract the radix (if any) */
	if (rpos)
	{
		/* Are there any other radix markers? */
		if (strrchr(num, (int)fiRadixChar) != rpos)
		{
			/* Yes - bad number */
			errno = EDOM;
			return (FiSInt)0;
		}


		/* Scan for the radix (a decimal number) */
		rlen = rpos - num;
		radix = (int)fiScanSmallIntFrString(num, rlen, 10);


		/* All okay so far? Valid radix: 2-36 inclusive */
		if (errno || (radix < 2) || (radix > 36))
			return (FiSInt)0;


		/* Move to the start of the whole part */
		num = rpos + 1;
	}


	/* Convert the whole part using the specified base */
	retval = strtol(num, &rest, radix);


	/* Check for badly formatted input */
	if (retval == 0 && (errno != 0 || num == rest))
	{
		/* Some error - give 0 */
		if (!errno) errno = EDOM;
		retval = (FiSInt) 0;
	}


	/*
	 * If an overflow has occurred then retval == LONG_MAX or
	 * retval == LONG_MIN. We don't really care either way.
	 */
	return (FiSInt) retval;
}

/******************************************************************************
 *
 * :: Conversion of floating point machine types
 *
 *****************************************************************************/

FiArb
fiBoxDFlo(FiDFlo d)
{
	FiDFlo	*ptr;

	ptr = (FiDFlo *) FI_ALLOC(sizeof(FiDFlo), CENSUS_DFlo);
	*ptr = d;
	return (FiArb) ptr;
}

FiDFlo
fiUnBoxDFlo(FiArb p)
{
	FiDFlo	d;

	d = *((FiDFlo *) p);
     /*	FI_FREE((FiDFlo *) p); */
	return d;
}

FiWord
fiSFloToWord(FiSFlo sf)
{
	/*
	 *  2006/Dec/20
	 *
	 *  BDS: On ia64 SMP machine silky.sharcnet.ca, the original cast
	 *  gives incorrect results in some instances.  It isn't clear
	 *  why a different result occurs, but returning the value as a
	 *  FiWord in this manner appears to work correctly.
         */
	union {
		FiWord w;
		FiSFlo f;
	} temp;

	temp.f = sf;
	return temp.w;

	/*
	 *  Original code before the modification:
	 *
	 *	return *((FiWord *) &sf);
         */
}

FiSFlo
fiWordToSFlo(FiWord i)
{
	/*
	 *  2006/Dec/20
	 *
	 *  BDS: On ia64 SMP machine silky.sharcnet.ca, the original cast
	 *  gives incorrect results in some instances.  It isn't clear
	 *  why a different result occurs, but returning the value as a
	 *  FiWord in this manner appears to work correctly.
         */
	union {
		FiWord w;
		FiSFlo f;
	} temp;

	temp.w = i;
	return temp.f;


	/*
	 *  Original code before the modification:
	 *
	 *	return *((FiSFlo *) &i);
         */
}

/******************************************************************************
 *
 * :: Platform operations
 *
 *****************************************************************************/

FiWord
fiHalt(FiSInt i)
{
	/* These numbers must be the same as the foamHaltCode enum. */
	switch ((int)i) {
	case 101:
		fiRaiseException((FiWord)"(Aldor error) Bad use of a dependent type");
		exit((int)i);		
	case 102:
		fiRaiseException((FiWord)"(Aldor error) Reached a \"never\"");
		exit((int)i);
	case 103:
		fiRaiseException((FiWord)"(Aldor error) Bad union branch.");
		exit((int)i);
	case 104:
		fiRaiseException((FiWord)"(Aldor error) Assertion failed.");
		exit((int)i);
	case 105:
		fiRaiseException((FiWord)"(Aldor error) Bad Fortran recursion.");
		exit((int)i);
	case 106:
		fiRaiseException((FiWord)"(Aldor error) Write to invalid pointer (read-only?).");
		exit((int)i);
	default:
		fiRaiseException((FiWord)"(Aldor error) Halt");
		exit((int)i);
	case -1:
		break;	/* To quiet compilers which think "exit" returns. */
	}
	return 0;
}

FiWord
fiPlatformRTE(void)
{
	return 1;
}

FiWord
fiPlatformOS(void)
{
	int	platno = 0;
#ifdef OS_UNIX
	platno = 1000;
#endif
#ifdef OS_CMS
	platno = 2000;
#endif
#ifdef OS_IBM_OS2
	platno = 3000;
#endif
#ifdef OS_MS_DOS
	platno = 4000;
#endif
	return (FiWord) platno;
}

/*****************************************************************************
 *
 * :: Old-style debugger support.
 *
 *****************************************************************************/

static FiWord fiDebugVar;

void
fiSetDebugVar(FiWord v)
{
	fiDebugVar = v;
}

FiWord
fiGetDebugVar(void)
{
	return fiDebugVar;
}

/*****************************************************************************
 *
 * :: New-style debugger support.
 *
 *****************************************************************************/

static FiWord fiDebugger[FI_DBG_LIMIT];

void
fiSetDebugger(FiDbgTag event, FiWord v)
{
	if ((event >= FI_DBG_START) && (event < FI_DBG_LIMIT))
		fiDebugger[event] = v;
}

FiWord
fiGetDebugger(FiDbgTag event)
{
	if ((event >= FI_DBG_START) && (event < FI_DBG_LIMIT))
		return fiDebugger[event];
	else
		return (FiWord)0;
}

/******************************************************************************
 *
 * :: Useful functions
 *
 *****************************************************************************/

local Hash	localStrHash(register String s);

FiWord
fiStrHash(FiWord s)
{
	return localStrHash((char*) s);
}

/******************************************************************************
 *
 * :: Fluids
 *
 *****************************************************************************/

local FiFluid	fiPushFluid _of_((void));
FiFluidStack fiGlobalFluidStack;

FiFluid 
fiAddFluid(char * name)
{
	FiFluid new;

	new = fiPushFluid();
	new->tag = name;
	new->value = (FiWord) 0xdeadaabb;
	return new;
}

/* Avoiding macros for debugging porpoises */
local FiFluid
fiPushFluid(void) 
{
	FiFluidStack new, tmp;

	tmp = fiGlobalFluidStack;
	new = (FiFluidStack)FI_ALLOC(sizeof(struct fiFluidStack), CENSUS_Fluid);
	fiGlobalFluidStack = new;

	new->prev = tmp;

	return (FiFluid) new;
}

void
fiSetFluid(FiFluid obj,FiWord  val)
{
	obj->value = val;
}

void
fiKillFluid(FiFluid obj)
{
	FI_FREE(obj);
}

FiWord
fiFluidValue(FiFluid obj)
{
	return obj->value;
}

FiFluid
fiGetFluid(char * name)
{
	FiFluidStack s = fiGlobalFluidStack;

	while (s) {
		if (!strcmp(name, s->tag))
			return (FiFluid) s;
		s = s->prev;
	}
	return NULL;
}

/******************************************************************************
 *
 * :: Fluid State Mangler...
 *
 *****************************************************************************/

/* 
 * Dynamic "State" is something that can be extended by external
 * functions, so we make this nice and generic.
 * we need some way of unregistering state functions, but I guess
 * that isn't too important at the moment.
 * [NB: what about fluid.c? ]
 */

void *	fiSaveFIState	(void);
void	fiRestoreFIState(void *);

typedef struct {
	/* should be trying to avoid an alloc/free for each fn */
	int  nbytes;
	void *(*save)(void);
	void (*restore)(void *);
} stateFns;

FiStateChain    fiGlobStates;

static stateFns fiStateFns[10];
int    fiNStates;

#define firstState(x) x


void
fiRegisterStateFns(void *(*save)(void), void (*restore)(void *))
{
	if (fiNStates >= 10) 
		bug("fiRegisterStateFns: no way!");
	fiStateFns[fiNStates].save    = save;
	fiStateFns[fiNStates].restore = restore;
	fiNStates++;
}

void 
fiSaveState0(FiState state)
{
	static int initted = 0;
	int i;

	if (!initted) {
		fiRegisterStateFns(fiSaveFIState, fiRestoreFIState);
		initted = 1;
	}
	state->next = fiGlobStates;
	state->nStates = fiNStates;
#if OLDWAY
	state->states = (void **)stoAlloc(OB_Other, fiNStates * sizeof(void*));
#else
	state->states = (void **)FI_ALLOC(fiNStates*sizeof(void*), CENSUS_SaveState);
#endif
	for (i=0; i<fiNStates; i++) {
		state->states[i] = (*fiStateFns[i].save)();
	}
	fiGlobStates = state;
}

void
fiRestoreState0(FiState state)
{
	int i;
	fiGlobStates = state->next;

	for (i=0; i<state->nStates; i++) 	
		(*fiStateFns[i].restore)(state->states[i]);
}

void
fiUnwind(FiWord target, FiWord value)
{
	FiState state = firstState(fiGlobStates);
	if (target == 0) 
	  	target=(FiWord) state;
	state->target = target;
	state->value  = value;
	longjmp(state->machineState, 1);
}

typedef struct {
	FiFluidStack chain;
} fintState;

void *
fiSaveFIState(void)
{
#if OLDWAY
	fintState *state = (fintState *) stoAlloc(OB_Other, sizeof(*state));
#else
	fintState *state = (fintState *)FI_ALLOC(sizeof(*state), CENSUS_SaveState);
#endif
	state->chain = fiGlobalFluidStack;

	return (void*) state;
}

void
fiRestoreFIState(void *state)
{
	fiGlobalFluidStack = ((fintState *) state)->chain;
#if OLDWAY
	stoFree(state);
#else
	FI_FREE(state);
#endif
}

#ifdef FOAM_RTS
static FiClos exnHandler;
static FiClos unhHandler;
#endif

void
fiUnhandledException(FiWord exn)
{
#ifdef FOAM_RTS
	extern FiClos G_LKR1B_aldorUnhandledExceptio;
	/* Call user code */
	Ptr foo = NULL;
	Ptr *pfoo = &foo;
	if (unhHandler == NULL) {
		fiImportGlobal("G_LKR1B_aldorUnhandledExceptio", pfoo);
		if (pfoo) unhHandler = (FiClos) *pfoo;
	}
	if (unhHandler)
		fiCCall1(FiWord, unhHandler, (FiWord) exn);
	else {
		printf("Aldor runtime: there is no exception handler installed.\nNote: There seems to be no aldorUnhandledException function defined\nso it is not possible to continue.\n");
		exit(2);
	}
#else
	/* Shouldn't happen */
#endif
}

void (*fiExceptionHandler)(char *,void *);

void
fiRaiseException(FiWord problem)
{
#ifdef FOAM_RTS
	Ptr foo = NULL;
	Ptr *pfoo;
	pfoo = &foo;
	if (exnHandler == NULL) {
		fiImportGlobal("G_JSAWY_aldorRuntimeException", pfoo);
		if (pfoo) exnHandler = (FiClos) *pfoo;
	}
	if (exnHandler)
		fiCCall2(FiWord, exnHandler, (FiWord) problem, NULL);
	else {
		printf("Aldor runtime: An Aldor runtime error occurred : %s\n\
Note: there seems to be no aldorRuntimeException function defined\n\
so it is not possible to throw an exception.\n", (char *) problem);
		exit(2);
	}
#else
	/*fiExceptionHandler must have been set to fintRaiseException in fint.c */
	if ((void *)*fiExceptionHandler) (*fiExceptionHandler)((char *)problem, NULL);
	else {
		(void) printf("Aldor runtime (interpreter): should never happen : fiExceptionHandler corrupted\nExiting...");
		exit(2);
	}
#endif
}

/******************************************************************************
 *
 * :: Interface to the store management
 *
 *****************************************************************************/

/* Force a garbage collection */
void
fiStoForceGC(void)
{
	stoGc();
}


/* Is the object in the FOAM heap? */
FiBool
fiStoInHeap(FiPtr p)
{
	return (FiBool)stoIsPointer((Pointer *)p);
}


/* Is the object writable? */
FiSInt
fiStoIsWritable(FiPtr p)
{
	/* FIXME: takes (Pointer)p ... */
	return stoWritablePointer((Pointer *)p);
}


/* Mark from a pointer: may only be called during a GC */
FiSInt
fiStoMarkObject(FiPtr p)
{
	return stoMarkObject((Pointer)p);
}


/* Change the storage type of an object */
FiWord
fiStoRecode(FiPtr p, FiSInt t)
{
	/* FIXME: takes (Pointer)p ... */
	return (FiWord)stoRecode((Pointer *)p, t);
}


/* Introduce a new object type */
void
fiStoNewObject(FiSInt code, FiBool hasPtrs)
{
	StoInfoObj info;

	info.code    = code;
	info.hasPtrs = hasPtrs;
	stoRegister((StoInfo)(&info));
}


/*
 * This function adds a user-defined tracer function for the
 * specified object type. The tracer is an Aldor function.
 */
void
fiStoATracer(FiSInt code, FiClos clos)
{
	stoSetAldorTracer(code, clos);
}

/*
 * This function adds a user-defined tracer function for the
 * specified object type. The tracer is a C function.
 */
void
fiStoCTracer(FiSInt code, FiWord fun)
{
	stoSetTracer(code, (FiPtr)fun);
}


/* Detailed information about the store */
void
fiStoShow(FiSInt flags)
{
	stoShowDetail(flags);
}


/* Create suitable argument for fiStoShow (try "show") */
int
fiStoShowArgs(FiPtr args)
{
	return stoShowArgs(args);
}


/******************************************************************************
 *
 * :: fiListNil, fiListCons, fiListEmptyP, fiListHead, fiListTail
 *
 * This is an experiment to see if an intrinsic list type is useful
 *
 *****************************************************************************/

FiPtr	fiListNil(void)		{ return (FiPtr)NULL; }
FiBool	fiListEmptyP(FiPtr lis)	{ return !lis; }
FiWord	fiListHead(FiPtr lis)	{ return ((FiList *)lis)->data; }
FiPtr	fiListTail(FiPtr lis)	{ return (FiPtr)(((FiList *)lis)->next); }


FiPtr
fiListCons(FiWord dat, FiPtr lis)
{
	FiList	*result = (FiList *)FI_ALLOC(sizeof(FiList), CENSUS_Unknown);

	result->data = dat;
	result->next = (FiList *)lis;

	return (FiPtr)result;
}


/******************************************************************************
 *
 * :: strEqual, strHash
 *
 *****************************************************************************/

local Bool
localStrEqual(String s1, String s2)
{
	return !strcmp(s1, s2);
}

local Hash
localStrHash(register String s)
{
	register Hash   h = 0;
	register int    c;

	while ((c = *s++) != 0) {
		h ^= (h << 8);
		h += (charToAscii(c) + 200041);
		h &= 0x3FFFFFFF;
	}
	return h;
}

/******************************************************************************
 *
 * :: Globals linking...
 *
 * Globals linking is performed in this way:
 * 1) Globals are unique by name, so there is a table from names to
 *    GlobalLinkInfo objects (see below)
 * 2) Each module (typically an object file) has a set of globals exported and
 *    a set of globals imported. In the module initialization there is:
 *      - for each global EXPORTED: fiExportGlobal("globName", globName);
 *	- for each global IMPORTED: fiImportGlobal("globName", pGlobName);
 * 3) Follows from (2) that: a) exported globals are static or extern value
 *    provided by the file that exports them; b) imported globals are
 *    represented as pointers to the data in the file that exports them.
 * 4) If a global is imported before exported, then it is added to the list
 *    of unresolved references to that name; the first element of this list
 *    is pointed by the slot unresolved in the structure globalLinkInfo.
 * 5) When a global is exported, then:
 *	- globalLinkInfo.data is set to point to the address of the static 
 *	   location containing his value,
 *	- globalLinkInfo.size records his size
 *	- all the pointers in the list of unresolved globals are set pointing
 *	   to the right location.
 *
 *****************************************************************************/

#define linkDEBUG(x)	 			/* x */


/* FiLinkList is a local lists implementation. We need some basic operations on
 * a list of unresolved imports, but we don't want to add list.c to the runtime
 */
typedef struct _fiConsCell * FiLinkList;
typedef struct _fiConsCell {
	Ptr		* import;
	FiLinkList	next;

} fiConsCell;


typedef struct {
	Ptr		data; /* points to a glob once it has been exported */
	int		size;	/* -1 means that it hasn't been exported yet */
	FiLinkList	unresolved;  /* see above */

} * GlobalLinkInfo;


Table tblGlobals;

static void
tblGlobalsInit(void)
{
	static Bool initialized = false;
	StoInfoObj  obj;

	if (initialized) 
		return;
	else
		initialized = true;
	/* Hack !! */
	osInit();
	fiBIntInit();
	obj.code    = OB_BInt;
	obj.hasPtrs = false;
	stoRegister(&obj);

	tblGlobals = tblNew((TblHashFun)localStrHash, (TblEqFun)localStrEqual);
}

void
fiExportGlobalFun(String name, Ptr p, int size)
{
	GlobalLinkInfo		glInfo;

	linkDEBUG(printf("Exporting %s %d %d\n", name, p, size);)

	tblGlobalsInit();

	glInfo = (GlobalLinkInfo) tblElt(tblGlobals, (TblKey)name, (TblElt)0);
	if (glInfo == 0) {
		glInfo = (GlobalLinkInfo) FI_ALLOC(sizeof(*glInfo), CENSUS_GlobalInfo);
		(void)tblSetElt(tblGlobals, (TblKey) name, (TblElt) glInfo);
	}
	else if (glInfo->size == -1) {
		FiLinkList p0, next;

		for (p0 = glInfo->unresolved; p0; p0 = next) {
			*(p0->import) = p;
			next = p0->next;
			FI_FREE(p0);
		}
	}
	else {
		linkDEBUG(printf("WARNING: %s already exported!\n", name);)
		assert(p == glInfo->data);
		return;
	}

	glInfo->data = p;
	glInfo->size = size;
	glInfo->unresolved = NULL;
}

void
fiImportGlobalFun(String name, Ptr * p)
{
	GlobalLinkInfo		glInfo;

	linkDEBUG(printf("Importing %s: ", name);)

	tblGlobalsInit();

	glInfo = (GlobalLinkInfo) tblElt(tblGlobals, (TblKey)name, (TblElt)0);
	if (glInfo == 0) {
		glInfo = (GlobalLinkInfo) FI_ALLOC(sizeof(*glInfo), CENSUS_GlobalInfo);
		(void) tblSetElt(tblGlobals, (TblKey) name, (TblElt) glInfo);

		linkDEBUG(printf("unresolved (first time)\n");)

		glInfo->data = NULL;
		glInfo->size = -1;
		glInfo->unresolved = (FiLinkList) FI_ALLOC(sizeof(fiConsCell), CENSUS_GlobalInfo);
		glInfo->unresolved->next = NULL;
		glInfo->unresolved->import = p;
		*p = NULL;
	}
	else if (glInfo->size > -1)   {  /* already exported */ 
		*p = glInfo->data;
		linkDEBUG(printf("resolved with (%d) %d\n", glInfo, glInfo->data);)
	}
	else {
		FiLinkList	l = (FiLinkList) FI_ALLOC(sizeof(fiConsCell), CENSUS_GlobalInfo);
		linkDEBUG(printf("unresolved (NOT first time)\n");)
		
		l->next = glInfo->unresolved;
		l->import = p;
		*p = NULL;
		glInfo->unresolved = l;
	}
}

void	(* fiFileInitializer)(char *) = (void (*)(char *)) NULL;


/*****************************************************************************
 *
 * :: Raw record operations
 *
 *****************************************************************************/


/*
 * Create a raw record of the specified format.
 */
FiRRec
fiRawRecordNewFn(FiSInt fmtc, FiSInt *fmtv)
{
	return (FiRRec)fi0Alloc(fmtv[fmtc], CENSUS_RRec);
}


/*
 * Return a pointer to a field in a raw record. This pointer
 * can be viewed as an Aldor record by the caller. We assume
 * that `elt' is a valid field number (counting from 0).
 */
FiWord
fiRawRecordEltFn(FiPtr ptr, FiSInt elt, FiSInt *fmtv)
{
	return (FiWord)((FiSInt)ptr + fmtv[elt]);
}


/*
 * Convert a multi of field sizes into an array of field offsets.
 */
FiPtr
fiRawRecordValues(FiSInt num, ...)
{
	va_list	argp;
	FiSInt	i, rsize;
	FiWord	*result;


	/* Allocate enough storage for the result */
	result = (FiWord *)fi0Alloc((num + 1)*sizeof(FiSInt), CENSUS_RRFmt);


	/* Compute record indices from field sizes */
	va_start(argp, num);
	for (i = rsize = 0; i < num; i++)
	{
		result[i] = rsize;
		rsize += va_arg(argp, FiSInt);
	}
	va_end(argp);


	/* Record the total size for the RRNew */
	result[num] = rsize;


	/* Return the array of values */
	return (FiPtr)result;
}


/*
 * Compute the size of a field whose format entry is `fmt'.
 * We assume that values are word-aligned.
 */
FiSInt
fiRawRepSize(FiSInt fmt) {
	FiSInt	extra, size = sizeof(FiWord);

	switch (fmt) {
#if 0
		case fiTypeInt8()   :	size = sizeof(FiInt8); break;
		case fiTypeInt16()  :	size = sizeof(FiInt16); break;
		case fiTypeInt32()  :	size = sizeof(FiInt32); break;
		case fiTypeInt64()  :	size = sizeof(FiInt64); break;
		case fiTypeInt128() :	size = sizeof(FiInt128); break;
#endif

		case fiTypeNil()    :	size = sizeof(FiNil); break;
		case fiTypeChar()   :	size = sizeof(FiChar); break;
		case fiTypeBool()   :	size = sizeof(FiBool); break;
		case fiTypeByte()   :	size = sizeof(FiByte); break;
		case fiTypeHInt()   :	size = sizeof(FiHInt); break;
		case fiTypeSInt()   :	size = sizeof(FiSInt); break;
		case fiTypeBInt()   :	size = sizeof(FiBInt); break;
		case fiTypeSFlo()   :	size = sizeof(FiSFlo); break;
		case fiTypeDFlo()   :	size = sizeof(FiDFlo); break;
		case fiTypeWord()   :	size = sizeof(FiWord); break;
		case fiTypeClos()   :	size = sizeof(FiClos); break;
		case fiTypePtr()    :	size = sizeof(FiPtr); break;
		case fiTypeRec()    :	size = sizeof(FiRec); break;
		case fiTypeArr()    :	size = sizeof(FiArr); break;
		case fiTypeTR()     :	size = sizeof(FiTR); break;

		/* Bad news if we reach this one */
		default           :	break;
	}


	/* Round to word boundary (assuming sizeof(word) = 2^N) */
	extra = size & (sizeof(FiWord) - 1);
	if (extra) size += sizeof(FiWord) - extra;


	/* Return the aligned size */
	return size;
}


/*****************************************************************************
 *
 * :: Runtime export hash code clash checking
 *
 *****************************************************************************/

#if EDIT_1_0_n1_06

/*
 * This code is invoked during domain initialisation and is used to detect
 * name and type code clashes. This check cannot be done a compile time as
 * we do not have concrete type codes for parameterised domains (we have
 * expressions which compute them).
 *
 * There are three kinds of clashes detected here: the first is where there
 * are two exports with precisely the same name and type. For example, two
 * foo exports with type %->%. We assume that the programmer is aware of
 * this and that the exports have the same implementation. We do not show
 * such clashes as they can be very frequent. The second kind of clash is
 * similar: a domain Foo(T) might have exports foo: T->() and foo: X->().
 * When instantiated as Foo(X) we get a clash. Again we assume that the
 * programmer is aware of this but since we cannot match parameters very
 * easily we show them. Future work is to remove these from the output.
 *
 * The third type of clash is due to limitations of the hash code system
 * or bugs in its implementation. Two exports with different names or with
 * different types share the same name and type hash code. We show these.
 */
typedef struct {
	char *dom;
	long got;
	FiSInt *nameh;	/* Name hash codes */
	FiSInt *typeh;	/* Type hash codes */
	char **name;	/* Names */
	char **type;	/* Types */
} ExpTable;

FiWord
fiNewExportTable(char *exporter, long nelts)
{
	ExpTable *result = (ExpTable *)fiAlloc(sizeof(*result));
	result->nameh = (long *)fiAlloc(nelts*sizeof(FiSInt));
	result->typeh = (long *)fiAlloc(nelts*sizeof(FiSInt));
	result->name = (char **)fiAlloc(nelts*sizeof(char *));
	result->type = (char **)fiAlloc(nelts*sizeof(char *));
	result->dom = exporter;
	result->got = 0;
	return (FiWord)result;
}

void
fiAddToExportTable(ExpTable *t, FiSInt namh, FiSInt typh, char *nam, char *typ)
{
	/* Linear search to see if there is a clash */
	long i;
	for (i = 0; i < t->got; i++) {
		/* Check for a clash */
		if (t->nameh[i] != namh) continue;
		if (t->typeh[i] != typh) continue;

		/* Clash! Check for textual equality */
		if (!strcmp(nam, t->name[i]) && !strcmp(typ, t->type[i]))
			continue; /* We assume they have the same impl */

		/* Display this clash */
		(void)fprintf(stderr, "\n*** Clash in %s:\n", t->dom);
		(void)fprintf(stderr, "   %s: %s [hash %ld, slot %ld]\n",
			      nam, typ, typh, t->got);
		(void)fprintf(stderr, "   %s: %s [hash %ld, slot %ld]\n",
			      t->name[i], t->type[i], t->typeh[i], i);
	}

	/* Add to the next free slot in the table */
	t->nameh[t->got] = namh;
	t->typeh[t->got] = typh;
	t->name[t->got] = nam;
	t->type[t->got] = typ;
	t->got++;
}

void
fiFreeExportTable(ExpTable *t)
{
	fiFree((FiPtr)(t->nameh));
	fiFree((FiPtr)(t->typeh));
	fiFree((FiPtr)(t->name));
	fiFree((FiPtr)(t->type));
	fiFree((FiPtr)t);
}
#endif


FiSInt fiCounter()
{
	static int counter = 0;
	return counter++;
}

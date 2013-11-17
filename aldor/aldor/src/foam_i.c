/****************************************************************************
 *
 * foam_i.c:  Run time support for C version of abstract machine.
 *	      Big Integer implementation.
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
#include "axlgen.h"
#include "bigint.h"
#include "foam_c.h"

/*****************************************************************************
 *
 * :: BInt operations
 *
 *****************************************************************************/

void fiBIntInit()
{
	/* Random check that integers are where we expect them to be */
	if (sizeof(int) != sizeof(long))
		assert(sizeof(int) == sizeof(BIntS));
	if (sizeof(int) == sizeof(long))
		assert(sizeof(short) == sizeof(BIntS));
}


FiBInt
fiBIntFrPlacev(int isNeg,unsigned long  placec,unsigned short * data)
{
	return (FiBInt) bintFrPlacevS((Bool) isNeg, 
				      (Length) placec, 
				      data);
}

FiBInt
fiBIntFrInt(long n)
{
	return (FiBInt) bintNew(n);
}

FiBInt
fiBIntMake(FiPtr b)
{
	return (FiBInt) b;
}

FiBInt
fiBInt0(void)
{
	return (FiBInt) bint0;
}

FiBInt
fiBInt1(void)
{
	return (FiBInt) bint1;
}

FiBInt
fiBIntNew(long i)
{
	return (FiBInt) bintNew(i);
}

FiBool
fiBIntIsZero(FiBInt b)
{
	return (FiBool) bintIsZero((BInt) b);
}

FiBool
fiBIntIsPos(FiBInt b)
{
	return (FiBool) bintIsPos((BInt) b);
}

FiBool
fiBIntIsNeg(FiBInt b)
{
	return (FiBool) bintIsNeg((BInt) b);
}

FiBool
fiBIntIsSingle(FiBInt b)
{
	/* '<' to allow space for sign bit */
	return (FiBool) (bintLength((BInt) b) < bitsizeof(FiSInt));
}

FiBool
fiBIntLE(FiBInt a, FiBInt b)
{
	return (FiBool) bintLE((BInt) a, (BInt) b);
}

FiBool
fiBIntEQ(FiBInt a, FiBInt b)
{
	return (FiBool) bintEQ((BInt) a, (BInt) b);
}

FiBool
fiBIntLT(FiBInt a, FiBInt b)
{
	return (FiBool) bintLT((BInt) a, (BInt) b);
}

FiBool
fiBIntNE(FiBInt a, FiBInt b)
{
	return (FiBool) bintNE((BInt) a, (BInt) b);
}

FiBInt
fiBIntNegate(FiBInt b)
{
	return (FiBInt) bintNegate((BInt) b);
}

FiBInt
fiBIntPlus(FiBInt a, FiBInt b)
{
	return (FiBInt) bintPlus((BInt) a, (BInt) b);
}
	
FiBInt
fiBIntMinus(FiBInt a, FiBInt b)
{
	return (FiBInt) bintMinus((BInt) a, (BInt) b);
}
	
FiBInt
fiBIntTimes(FiBInt a, FiBInt b)
{
	return (FiBInt) bintTimes((BInt) a, (BInt) b);
}
	
FiBInt
fiBIntTimesPlus(FiBInt a, FiBInt b, FiBInt c)
{
	return (FiBInt) bintPlus(bintTimes((BInt) a, (BInt) b), (BInt) c);
}

FiSInt
fiBIntLength(FiBInt b)
{
	return (FiSInt) bintLength((BInt) b);
}
	
FiBInt
fiBIntShiftUp(FiBInt b, FiSInt n)
{
	return (FiBInt) bintShift((BInt) b, n);
}
	
	
FiBInt
fiBIntShiftDn(FiBInt b, FiSInt n)
{
	return (FiBInt) bintShift((BInt) b, -n);
}

FiBInt
fiBIntShiftRem(FiBInt b, FiSInt n)
{
	return (FiBInt) bintShiftRem((BInt) b, n);
}
	
FiBool
fiBIntBit(FiBInt b, FiSInt n)
{
	return (FiBool) bintBit((BInt) b, (Length) n);
}
	
FiBInt
fiBIntMod(FiBInt a, FiBInt b)
{
	BInt	r;
	if (bintIsZero((BInt) b)) {
		fiRaiseException((FiWord)"BIntMod: division by zero");
		exit(1);
	}
	r = bintMod((BInt) a, (BInt) b);
	return (FiBInt) r;
}

FiBInt
fiBIntQuo(FiBInt a, FiBInt b)
{
	BInt	r;

	if (bintIsZero((BInt) b)) {
		fiRaiseException((FiWord)"BIntQuo: division by zero");
		exit(1);
	}
	return (FiBInt) bintDivide(&r, (BInt) a, (BInt) b);
}

FiBInt
fiBIntRem(FiBInt a, FiBInt b)
{
	BInt	r;
	if (bintIsZero((BInt) b)) {
		fiRaiseException((FiWord)"BIntRem: division by zero");
		exit(1);
	}
	r = bintMod((BInt) a, (BInt) b);
	return (FiBInt) r;
}

void
fiBIntDivide(FiBInt a, FiBInt b, FiBInt *r0, FiBInt *r1)
{
	BInt	rem;
	if (bintIsZero((BInt) b)) {
		fiRaiseException((FiWord)"BIntDivide: division by zero");
		exit(1);
	}
	*r0 = (FiBInt) bintDivide(&rem, (BInt) a, (BInt) b);
	*r1 = (FiBInt) rem;
	return;
}

/*
 *!! Should get DHL to rewrite this.
 *!! Or at least use iintDivide.
 */
FiBInt
fiBIntGcd(FiBInt a, FiBInt b)
{
	BInt	c, d, c0, d0, t, q;

	c = c0 = bintCopy((BInt) a);
	d = d0 = bintCopy((BInt) b);

	if (bintLT(c, bint0)) { c = bintNegate(c); bintFree(c0); }
	if (bintLT(d, bint0)) { d = bintNegate(d); bintFree(d0); }

	while (bintNE(d, bint0)) {
		t = c;
		c = d;
		q = bintDivide(&d, t, c);
		bintFree(q);
		bintFree(t);
	}
	return (FiBInt) c;
}

/*
 *!! Should use iint{Times,Plus}.
 */
FiBInt
fiBIntSIPower(FiBInt a0, FiSInt b)
{
	BInt	a, p, t;
	int	i, l;

	if (b < 0) {
		fiRaiseException((FiWord)"BIntSIPower: negative power");
		exit(1);
	}
	if (b == 0)
		return (FiBInt) bint1;

	p = bint1;
	a = bintCopy((BInt) a0);
	l = fiSIntLength(b);

	for (i = 0; ; i++) {
		if (fiSIntBit(b, i)) {
			t = p;
			p = bintTimes(t, a);
			bintFree(t);
		}
		if (i >= l) break;

		t = a;
		a = bintTimes(t, t);
		bintFree(t);
	}
	bintFree(a);
	return (FiBInt) p;
}

/*
 *!! Should use iint{Times,Plus}.
 */
FiBInt
fiBIntBIPower(FiBInt a0, FiBInt b0)
{
	BInt	a, b, p, t;
	int	i, l;

	b = (BInt) b0;

	if (bintIsNeg(b)) {
		fiRaiseException((FiWord)"BIntBIPower: negative power");
		exit(1);
	}
	if (bintIsZero(b))
		return (FiBInt) bint1;

	p = bint1;
	a = bintCopy((BInt) a0);
	l = bintLength(b);

	for (i = 0; ; i++) {
		if (bintBit(b, i)) {
			t = p;
			p = bintTimes(t, a);
			bintFree(t);
		}
		if (i >= l) break;

		t = a;
		a = bintTimes(t, t);
		bintFree(t);
	}
	bintFree(a);
	return (FiBInt) p;
}

FiBInt
fiBIntPowerMod(FiBInt a0, FiBInt b0, FiBInt c0)
{
	BInt    reda;
	BInt	a, b, c, p, t;
	int	i, l;

	b = (BInt) b0;
	c = (BInt) c0;
	if (bintIsZero((BInt) c)) {
		fiRaiseException((FiWord)"BIntPowerMod: modulo is zero");
		exit(1);
	}
	if (bintIsZero(b)) return (FiBInt)bint1;
	reda = bintMod((BInt)a0,c);
	if (bintIsZero(reda)) return (FiBInt)bint0;
	if (bintIsNeg(b)) {
		fiRaiseException((FiWord)"BIntPowerMod: negative power");
		exit(1);
	}

	p = bint1;
	a = bintCopy((BInt) reda);
	l = bintLength(b);

	for (i = 0; ; i++) {
		if (bintBit(b, i)) {
			t = p;
			p = bintMod(bintTimes(t, a),c);
			bintFree(t);
		}
		if (i >= l) break;

		t = a;
		a = bintMod(bintTimes(t, t),c);
		bintFree(t);
	}
	bintFree(a);
	return (FiBInt) p;

}


void
fiScanBInt(FiArr s, FiSInt i, FiBInt *r0, FiSInt *r1)
{
	String end;
	*r0 = (FiBInt) bintScanFrString((String) s + i, &end);
	*r1  = end - (String) s;
	return;
}

FiSInt
fiFormatBInt(FiBInt x, FiArr s, FiSInt i)
{
	String	buf;
	int	l;

	buf = bintToString((BInt) x);
	l   = strlen(buf);
	if (l+i > strlen((String) s)) {
		fiRaiseException((FiWord)"FormatBInt: the array given is not big enough");
		exit(1);
	}
	memmove((String) s + i, buf, l+1);
	return (FiSInt) l + i;
}

char *
fiBIntToString(FiBInt x)
{
	return bintToString((BInt)x);
}

FiBInt
fiSIntToBInt(FiSInt i)
{
	return (FiBInt) bintNew(i);
}

FiSInt
fiBIntToSInt(FiBInt b)
{
	if (bintIsSmall((BInt) b))
		return (FiSInt) bintSmall((BInt) b);
	else {
		int    i;
		FiSInt n;
		BInt   bb = (BInt) b;
		for (i=bitsizeof(FiSInt) - 1, n=0; i >= 0 ; i--) {
			n = n << 1;
			if (bintBit(bb, i)) 
				n++;
		}
		if (bintIsNeg(bb))
			n = -n;
		return n;
	}
}

FiSFlo
fiBIntToSFlo(FiBInt b)
{
	if (bintIsZero((BInt) b)) return 0.0;
	return (FiSFlo) atof(bintToString((BInt) b));
}

FiDFlo
fiBIntToDFlo(FiBInt b)
{
	if (bintIsZero((BInt) b)) return 0.0;
	return (FiDFlo) atof(bintToString((BInt) b));
}


FiBInt
fiArrToBInt(FiArr s)
{
	return (FiBInt) bintFrString((String) s);
}

void
fiBIntFree(FiBInt b)
{
	bintFree((BInt) b);
}

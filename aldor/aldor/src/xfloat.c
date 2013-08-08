/*****************************************************************************
 *
 * xfloat.c: External floating point format
 *
 * Copyright (c) 1990-2007 Aldor Software Organization Ltd (Aldor.org).
 *
 ****************************************************************************/

#include "axlgen.h"
#include "debug.h"
#include "util.h"
#include "xfloat.h"

/*
 *  2006/Dec/20
 *
 *  Including cconfig.h to gain access to CC_SF_is_double
 */
#include "cconfig.h"
/*
 *  Define a local macro for the C data type being used to represent
 *  a single float.  We cannot just go and include the normal definition
 *  because that becomes a tangle of conflicting definitions from all the
 *  other files that are included as a result.
 */
#ifdef CC_SF_is_double
#  define ALDOR_SF_TYPE double
#else
#  define ALDOR_SF_TYPE float
#endif

#ifdef XFLOAT_DO_DEBUG
# define xfloatDEBUG		if (true)
#else
# define xfloatDEBUG		if (false)
#endif

#define USHORT_BIT	((int) bitsizeof(UShort))

#define SF_FracShift    (USHORT_BIT - SF_FracOff) 
#define SF_FracIx0	(SF_FracOff / CHAR_BIT)
#define SF_FracSh0	(SF_FracOff % CHAR_BIT)
#define SF_SignMask	(1<<(USHORT_BIT-1))
#define SF_FracMask	((1<<SF_FracShift) - 1)
#define SF_ExponMask	(((1<<USHORT_BIT)-1) &~SF_SignMask &~SF_FracMask)
#define SF_ExponMin	(-SF_Excess)
#define SF_ExponNAN	((SF_ExponMask >> SF_FracShift) - SF_Excess)
#define SF_LgBase	(1 << SF_LgLgBase)

#define DF_FracShift 	(USHORT_BIT - DF_FracOff)
#define DF_FracIx0	(DF_FracOff / CHAR_BIT)
#define DF_FracSh0	(DF_FracOff % CHAR_BIT)
#define DF_SignMask	(1<<(USHORT_BIT-1))
#define DF_FracMask	((1<<DF_FracShift) - 1)
#define DF_ExponMask	(((1<<USHORT_BIT)-1) &~DF_SignMask &~DF_FracMask)
#define DF_ExponMin	(-DF_Excess)
#define DF_ExponNAN	((DF_ExponMask >> DF_FracShift) - DF_Excess)
#define DF_LgBase	(1 << DF_LgLgBase)


#define XSF_FracShift 	(USHORT_BIT - XSF_FracOff)
#define XSF_FracIx0	(XSF_FracOff / BYTE_BITS)
#define XSF_FracSh0	(XSF_FracOff % BYTE_BITS)
#define XSF_SignMask	(1<<(USHORT_BIT-1))
#define XSF_FracMask	((1<<XSF_FracShift) - 1)
#define XSF_ExponMask	(((1<<USHORT_BIT)-1) &~XSF_SignMask &~XSF_FracMask)
#define XSF_ExponMin	(-XSF_Excess)
#define XSF_ExponNAN	((XSF_ExponMask >> XSF_FracShift) - XSF_Excess)
#define XSF_LgBase	(1 << XSF_LgLgBase)


#define XDF_FracShift 	(USHORT_BIT - XDF_FracOff)
#define XDF_FracIx0	(XDF_FracOff / BYTE_BITS)
#define XDF_FracSh0	(XDF_FracOff % BYTE_BITS)
#define XDF_SignMask	(1<<(USHORT_BIT-1))
#define XDF_FracMask	((1<<XDF_FracShift) - 1)
#define XDF_ExponMask	(((1<<USHORT_BIT)-1) &~XDF_SignMask &~XDF_FracMask)
#define XDF_ExponMin	(-XDF_Excess)
#define XDF_ExponNAN	((XDF_ExponMask >> XDF_FracShift) - XDF_Excess)
#define XDF_LgBase	(1 << XDF_LgLgBase)


#define ROUND_UP0(exp, lgB)   ((exp) > 0 ? ROUND_UP((exp), (lgB)) : \
   				          -(ROUND_UP(-(exp), (lgB)) - lgB))

#ifndef FOAM_RTS

/*****************************************************************************
 *
 * :: xxExponMin
 * :: xxExponNAN
 *
 ****************************************************************************/

int sfExponMin (void) { return SF_ExponMin;  }
int dfExponMin (void) { return DF_ExponMin;  }
int xsfExponMin(void) { return XSF_ExponMin; }
int xdfExponMin(void) { return XDF_ExponMin; }

int sfExponNAN (void) { return SF_ExponNAN;  }
int dfExponNAN (void) { return DF_ExponNAN;  }
int xsfExponNAN(void) { return XSF_ExponNAN; }
int xdfExponNAN(void) { return XDF_ExponNAN; }


/*****************************************************************************
 *
 * :: xxClassify
 *
 ****************************************************************************/

FloatCase
sfClassify(ALDOR_SF_TYPE *psf)
{
	int	expbits, i;
	Bool	hasFrac, hasnans = SF_HasNANs;

	expbits = SF_UShort(psf,int0) & SF_ExponMask;

	hasFrac = false;
	for (i = 0; i < sizeof(ALDOR_SF_TYPE) - SF_FracIx0; i++)
		if (SF_UByte(psf, SF_FracIx0 + i) & (i ? ~0 : SF_FracMask)) {
			hasFrac = true;
			break;
		}

	if      (expbits == 0)
		return hasFrac ? FLOAT_DENORM : FLOAT_ZERO;
	else if (expbits == SF_ExponMask && hasnans)
		return hasFrac ? FLOAT_NAN : FLOAT_INF;
	else
		return FLOAT_NORM;
}

FloatCase
dfClassify(double *pdf)
{
	int	expbits, i;
	Bool	hasFrac, hasnans = DF_HasNANs;

	expbits = DF_UShort(pdf,int0) & DF_ExponMask;

	hasFrac = false;
	for (i = 0; i < sizeof(double) - DF_FracIx0; i++)
		if (DF_UByte(pdf, DF_FracIx0 + i) & (i ? ~0 : DF_FracMask)) {
			hasFrac = true;
			break;
		}

	if      (expbits == 0)
		return hasFrac ? FLOAT_DENORM : FLOAT_ZERO;
	else if (expbits == DF_ExponMask && hasnans)
		return hasFrac ? FLOAT_NAN : FLOAT_INF;
	else
		return FLOAT_NORM;
}

FloatCase
xsfClassify(XSFloat *pxsf)
{
	ULong	expbits;
	Bool	hasFrac, hasnans = XSF_HasNANs;
	int	i;

	expbits = UNBYTE2(XSF_UByte(pxsf, 1), XSF_UByte(pxsf, int0))
	        & XSF_ExponMask;

	hasFrac = false;
	for (i = 0; i < sizeof(XSFloat) - XSF_FracIx0; i++)
		if (XSF_UByte(pxsf, XSF_FracIx0 + i)) {
			hasFrac = true;
			break;
		}

	if      (expbits == 0)
		return hasFrac ? FLOAT_DENORM : FLOAT_ZERO;
	else if (expbits == XSF_ExponMask && hasnans)
		return hasFrac ? FLOAT_NAN : FLOAT_INF;
	else
		return FLOAT_NORM;
}

FloatCase
xdfClassify(XDFloat *pxdf)
{
	ULong	expbits;
	Bool	hasFrac, hasnans = XDF_HasNANs;
	int	i;

	expbits = UNBYTE2(XDF_UByte(pxdf, 1), XDF_UByte(pxdf, int0))
	        & XDF_ExponMask;

	hasFrac = false;
	for (i = 0; i < sizeof(XDFloat) - XDF_FracIx0; i++)
		if (XDF_UByte(pxdf, XDF_FracIx0 + i)) {
			hasFrac = true;
			break;
		}

	if      (expbits == 0)
		return hasFrac ? FLOAT_DENORM : FLOAT_ZERO;
	else if (expbits == XDF_ExponMask && hasnans)
		return hasFrac ? FLOAT_NAN : FLOAT_INF;
	else
		return FLOAT_NORM;
}

#endif /* FOAM_RTS */
/*****************************************************************************
 *
 * :: xxDissemble
 *
 ****************************************************************************/

void
sfDissemble(ALDOR_SF_TYPE *psf, Bool *psign, int *pexponent, UByte *pfracbytes, Bool *iszero)
{
	int	i;

	if (iszero)
		*iszero = *psf == 0.0;
	if (psign)
		*psign     = (SF_UShort(psf, int0)&SF_SignMask) != 0;
	if (pexponent)
		*pexponent = ((SF_UShort(psf,int0)&SF_ExponMask)>>SF_FracShift)
		           - SF_Excess; 

	if (pfracbytes) {
		for (i = 0; i < sizeof(ALDOR_SF_TYPE) - SF_FracIx0; i++)
			pfracbytes[i] =	SF_UByte(psf, SF_FracIx0 + i); 
		for (     ; i < sizeof(ALDOR_SF_TYPE); i++)
			pfracbytes[i] = 0; 

		bfShiftUp(sizeof(ALDOR_SF_TYPE), pfracbytes,
		          SF_FracSh0, pfracbytes, int0);
	}
}

void
dfDissemble(double *pdf, Bool *psign, int *pexponent, UByte *pfracbytes, Bool *iszero)
{
	int	i;

	if (iszero)
		*iszero = *pdf == 0.0;
	if (psign)
		*psign     = (DF_UShort(pdf, int0)&DF_SignMask) != 0;
	if (pexponent)
		*pexponent = ((DF_UShort(pdf,int0)&DF_ExponMask)>>DF_FracShift)
		           - DF_Excess;

	if (pfracbytes) {
		for (i = 0; i < sizeof(double) - DF_FracIx0; i++)
			pfracbytes[i] = DF_UByte(pdf, DF_FracIx0 + i); 
		for (     ; i < sizeof(double); i++)
			pfracbytes[i] = 0; 

		bfShiftUp(sizeof(double), pfracbytes,
		          DF_FracSh0, pfracbytes, int0);
	}
}

#ifndef FOAM_RTS

void
xsfDissemble(XSFloat *pxsf, Bool *psign, int *pexponent, UByte *pfracbytes)
{
	ULong	w0;
	int	i;

	w0 = UNBYTE2(XSF_UByte(pxsf, 1), XSF_UByte(pxsf, int0));

	if (psign)
		*psign     = (w0 & XSF_SignMask) != 0;
	if (pexponent)
		*pexponent = (w0 & XSF_ExponMask) - XSF_Excess;

	if (pfracbytes)
		for (i = 0; i < sizeof(XSFloat) - XSF_FracIx0; i++)
			pfracbytes[i] = XSF_UByte(pxsf, XSF_FracIx0 + i); 
}


void
xdfDissemble(XDFloat *pxdf, Bool *psign, int *pexponent, UByte *pfracbytes)
{
	ULong	w0;
	int	i;

	w0 = UNBYTE2(XDF_UByte(pxdf, 1), XDF_UByte(pxdf, int0));

	if (psign)
		*psign     = (w0 & XDF_SignMask) != 0;
	if (pexponent)
		*pexponent = (w0 & XDF_ExponMask) - XDF_Excess;

	if (pfracbytes)
		for (i = 0; i < sizeof(XDFloat) - XDF_FracIx0; i++)
			pfracbytes[i] = XDF_UByte(pxdf, XDF_FracIx0 + i); 
}

#endif /* FOAM_RTS */
/*****************************************************************************
 *
 * :: xxAssemble
 *
 ****************************************************************************/

void
sfAssemble(ALDOR_SF_TYPE *psf, Bool sign, int exponent, UByte *pfracbytes)
{
	int	i;
	UByte	pb[sizeof(ALDOR_SF_TYPE)];
	UShort	us;

	us  = sign ? SF_SignMask : 0;
	us  |= ((exponent + SF_Excess) << SF_FracShift)
			     & SF_ExponMask;

	SF_UByte(psf, int0) = (us >> CHAR_BIT) & 0xff;
	SF_UByte(psf, 1) = us & 0xff;

	bfShiftDn(sizeof(ALDOR_SF_TYPE), pb,
		  SF_FracSh0, pfracbytes, int0, int0);

	for (i = 0; i < 1; i++)
		SF_UByte(psf, SF_FracIx0 + i) |= pb[i];
	for (     ; i < sizeof(ALDOR_SF_TYPE) - SF_FracIx0; i++)
		SF_UByte(psf, SF_FracIx0 + i)  = pb[i];
}

void
dfAssemble(double *pdf, Bool sign, int exponent, UByte *pfracbytes)
{
	int	i;
	UByte	pb[sizeof(double)];
	UShort  us;

	us = sign ? DF_SignMask : 0;
	us |= ((exponent + DF_Excess) << DF_FracShift)
			     & DF_ExponMask;

	DF_UByte(pdf, int0) = (us >> CHAR_BIT) & 0xff;
	DF_UByte(pdf, 1) = us & 0xff;

	bfShiftDn(sizeof(double), pb,
		  DF_FracSh0, pfracbytes, int0, int0);

	for (i = 0; i < 1; i++)
		DF_UByte(pdf, DF_FracIx0 + i) |= pb[i]; 
	for (     ; i < sizeof(double) - DF_FracIx0; i++)
		DF_UByte(pdf, DF_FracIx0 + i)  = pb[i]; 
}

#ifndef FOAM_RTS

void
xsfAssemble(XSFloat *pxsf, Bool sign, int exponent, UByte *pfracbytes)
{
	ULong	w0;
	int	i;

	w0  = sign ? XSF_SignMask : 0;
	w0 |= (exponent + XSF_Excess) & XSF_ExponMask;

	XSF_UByte(pxsf, int0) = BYTE1(w0);
	XSF_UByte(pxsf, 1   ) = BYTE0(w0);

	for (i = 0; i < sizeof(XSFloat) - XSF_FracIx0; i++)
		XSF_UByte(pxsf, XSF_FracIx0 + i) = pfracbytes[i]; 
}

void
xdfAssemble(XDFloat *pxdf, Bool sign, int exponent, UByte *pfracbytes)
{
	ULong	w0;
	int	i;

	w0  = sign ? XDF_SignMask : 0;
	w0 |= (exponent + XDF_Excess) & XDF_ExponMask;

	XDF_UByte(pxdf, int0) = BYTE1(w0);
	XDF_UByte(pxdf, 1   ) = BYTE0(w0);

	for (i = 0; i < sizeof(XDFloat) - XDF_FracIx0; i++)
		XDF_UByte(pxdf, XDF_FracIx0 + i) = pfracbytes[i]; 
}


/*****************************************************************************
 *
 * :: xxToNative
 *
 ****************************************************************************/

void
xsfToNative(XSFloat *pxsf, ALDOR_SF_TYPE *psf)
{
	int	pbTot = sizeof(XSFloat) - XSF_FracIx0;
	UByte	pb[sizeof(XSFloat) - XSF_FracIx0];
	int	expon, i, exponMod;
	Bool	sign, hasFrac, hasNANs = SF_HasNANs, hasNorm1 = SF_HasNorm1;

	xsfDissemble(pxsf, &sign, &expon, pb);

	hasFrac = 0;
	for (i = 0; i < pbTot; i++) if (pb[i]) hasFrac = 1;

	/* Was NAN or INF. */
	if (expon == XSF_ExponNAN) {
		if (!hasNANs)
			for (i = 0; i < sizeof(SFloat); i++) pb[i] = 0xff;
		sfAssemble(psf, sign, SF_ExponNAN, pb);
		xfloatDEBUG{fprintf(dbOut, "<A>");}
		return;
	}
	/* Was normal, becomes INF. */
	if ((expon >> SF_LgLgBase) >= (int) SF_ExponNAN) {
		UByte fillMask = (hasNANs ? 0x00 : 0xff);
		int	i;

		for (i = 0; i < pbTot; i++)  pb[i] = fillMask;

		sfAssemble(psf, sign, SF_ExponNAN, pb);
		xfloatDEBUG{fprintf(dbOut, "<B>");}
		return;
	}

	/* Was zero, becomes zero. */
	if (expon == XSF_ExponMin && !hasFrac) {
		sfAssemble(psf, sign, SF_ExponMin, pb);
		xfloatDEBUG{fprintf(dbOut, "<E>");}
		return;
	}

	xfloatDEBUG{fprintf(dbOut, "Origin exp: %d\n", expon);}

	if (XSF_HasNorm1 && !hasNorm1) {
		bfShiftDn(pbTot, pb, 1, pb, int0, 1);
		expon++;
		xfloatDEBUG{fprintf(dbOut, "<I1:%x>", pb[0]);}
	}

	exponMod = expon % SF_LgBase;

	if (exponMod) {   /* Must be rounded */
		int p = ROUND_UP0(expon, SF_LgBase);
		bfShiftDn(pbTot, pb, -expon + p, pb, int0, int0);
		
		xfloatDEBUG{fprintf(dbOut, "<R%d:%d>", expon-p, pb[0]);}
		expon = p;
	}

	expon = expon >> SF_LgLgBase;

	/* Was normal or subnormal, becomes subnormal or zero. */
	if (expon <= SF_ExponMin) {
		Bool b = false; 

		fracDenormalize(&expon, SF_ExponMin, sizeof(float), pb,
				   SF_LgLgBase, SF_HasNorm1);

		for (i = 0; i < pbTot && !b; i++)
			if (pb[i])   b = true;
		
		if (hasFrac && !b)  expon = SF_ExponMin;

		sfAssemble(psf, sign, expon, pb);
		xfloatDEBUG{fprintf(dbOut, "<C>");}
		return;
	}

	/* Was normal, becomes normal. */
	sfAssemble(psf, sign, expon, pb);
	xfloatDEBUG{fprintf(dbOut, "<D>");}
}


void
xdfToNative(XDFloat *pxdf, double *pdf)
{
	int	pbTot = sizeof(XDFloat) - XDF_FracIx0;
	UByte	pb[sizeof(XDFloat) - XDF_FracIx0];
	int	expon, i, exponMod;
	Bool	sign, hasFrac, hasNANs = DF_HasNANs, hasNorm1 = DF_HasNorm1;

	xdfDissemble(pxdf, &sign, &expon, pb);

	hasFrac = 0;
	for (i = 0; i < pbTot; i++) if (pb[i]) hasFrac = 1;

	/* Was NAN or INF. */
	if (expon == XDF_ExponNAN) {
		if (!hasNANs)
			for (i = 0; i < sizeof(DFloat); i++) pb[i] = 0xff;
		dfAssemble(pdf, sign, DF_ExponNAN, pb);
		xfloatDEBUG{fprintf(dbOut, "<A>");}
		return;
	}
	/* Was normal, becomes INF. */
	if ((expon >> DF_LgLgBase) >= (int) DF_ExponNAN) {
		UByte fillMask = (hasNANs ? 0x00 : 0xff);
		int	i;

		for (i = 0; i < pbTot; i++)  pb[i] = fillMask;

		dfAssemble(pdf, sign, DF_ExponNAN, pb);
		xfloatDEBUG{fprintf(dbOut, "<B>");}
		return;
	}

	/* Was zero, becomes zero. */
	if (expon == XDF_ExponMin && !hasFrac) {
		dfAssemble(pdf, sign, DF_ExponMin, pb);
		xfloatDEBUG{fprintf(dbOut, "<E>");}
		return;
	}

	xfloatDEBUG{fprintf(dbOut, "Origin exp: %d\n", expon);}

	if (XDF_HasNorm1 && !hasNorm1) {
		bfShiftDn(pbTot, pb, 1, pb, int0, 1);
		expon++;
		xfloatDEBUG{fprintf(dbOut, "<I1:%x>", pb[0]);}
	}

	exponMod = expon % DF_LgBase;

	if (exponMod) {   /* Must be rounded */
		int p = ROUND_UP0(expon, DF_LgBase);
		bfShiftDn(pbTot, pb, -expon + p, pb, int0, int0);
		
		xfloatDEBUG{fprintf(dbOut, "<R%d:%d>", expon-p, pb[0]);}
		expon = p;
	}

	expon = expon >> DF_LgLgBase;

	/* Was normal or subnormal, becomes subnormal or zero. */
	if (expon <= DF_ExponMin) {
		Bool b = false; 

		fracDenormalize(&expon, DF_ExponMin, sizeof(double), pb,
				   DF_LgLgBase, DF_HasNorm1);

		for (i = 0; i < pbTot && !b; i++)
			if (pb[i])   b = true;
		
		if (hasFrac && !b)  expon = DF_ExponMin;

		dfAssemble(pdf, sign, expon, pb);
		xfloatDEBUG{fprintf(dbOut, "<C>");}
		return;
	}

	/* Was normal, becomes normal. */
	dfAssemble(pdf, sign, expon, pb);
	xfloatDEBUG{fprintf(dbOut, "<D>");}
}


/****************************************************************************
 *
 * :: xxFrNative
 *
 ****************************************************************************/

void
xsfFrNative(XSFloat *pxsf, ALDOR_SF_TYPE *psf)
{
	int pbTot = sizeof(XSFloat) - XSF_FracIx0;
	UByte	pb[sizeof(XSFloat) - XSF_FracIx0];
	int	expon;
	Bool	sign, hasnans = SF_HasNANs, hasFrac, hasNorm1 = SF_HasNorm1;

	int	i;
	for (i = 0; i < pbTot; i++) pb[i] = 0;

	sfDissemble(psf, &sign, &expon, pb,NULL);

	hasFrac = 0;
	for (i = 0; i < pbTot; i++) if (pb[i]) hasFrac = 1;

	/* Was NAN or INF. */
	if (hasnans && expon == SF_ExponNAN) {
		xsfAssemble(pxsf, sign, XSF_ExponNAN, pb);
		xfloatDEBUG{fprintf(dbOut, "<A>");}
		return;
	}

	/* Was zero becomes zero. */
	if (expon == SF_ExponMin && !hasFrac) {
		xsfAssemble(pxsf, sign, XSF_ExponMin, pb);
		xfloatDEBUG{fprintf(dbOut, "<E>");}
		return;
	}

	/* Was subnormal becomes normal. */
	if (expon == SF_ExponMin && hasFrac) {
		expon = expon * SF_LgBase;
		fracNormalize(&expon, pbTot, pb);
		xsfAssemble(pxsf, sign, expon, pb);
		xfloatDEBUG{fprintf(dbOut, "<B>");}
		return;
	}

	expon = expon * SF_LgBase;

	if (XSF_HasNorm1 && !hasNorm1) {
		if (hasFrac) {
			fracNormalize(&expon, pbTot, pb);
			xfloatDEBUG{fprintf(dbOut, "<Dn>");}
		}
		else {  /* Becomes zero */
			xsfAssemble(pxsf, sign, XSF_ExponMin, pb);
			xfloatDEBUG{fprintf(dbOut, "<Z>");}
			return;
		}
	}

	/* Was normal, becomes normal. */
	xsfAssemble(pxsf, sign, expon, pb);
	xfloatDEBUG{fprintf(dbOut, "<C>");}
}


void
xdfFrNative(XDFloat *pxdf, double *pdf)
{
	int pbTot = sizeof(XDFloat) - XDF_FracIx0;
	UByte	pb[sizeof(XDFloat) - XDF_FracIx0];
	int	expon;
	Bool	sign, hasnans = DF_HasNANs, hasFrac, hasNorm1 = DF_HasNorm1;

	int	i;
	for (i = 0; i < pbTot; i++) pb[i] = 0;

	dfDissemble(pdf, &sign, &expon, pb,NULL);

	hasFrac = 0;
	for (i = 0; i < pbTot; i++) if (pb[i]) hasFrac = 1;

	/* Was NAN or INF. */
	if (hasnans && expon == DF_ExponNAN) {
		xdfAssemble(pxdf, sign, XDF_ExponNAN, pb);
		xfloatDEBUG{fprintf(dbOut, "<A>");}
		return;
	}

	/* Was zero becomes zero. */
	if (expon == DF_ExponMin && !hasFrac) {
		xdfAssemble(pxdf, sign, XDF_ExponMin, pb);
		xfloatDEBUG{fprintf(dbOut, "<E>");}
		return;
	}

	/* Was subnormal becomes normal. */
	if (expon == DF_ExponMin && hasFrac) {
		expon = expon * DF_LgBase;
		fracNormalize(&expon, pbTot, pb);
		xdfAssemble(pxdf, sign, expon, pb);
		xfloatDEBUG{fprintf(dbOut, "<B>");}
		return;
	}

	expon = expon * DF_LgBase;

	if (XDF_HasNorm1 && !hasNorm1) {
		if (hasFrac) {
			fracNormalize(&expon, pbTot, pb);
			xfloatDEBUG{fprintf(dbOut, "<Dn>");}
		}
		else {  /* Becomes zero */
			xdfAssemble(pxdf, sign, XDF_ExponMin, pb);
			xfloatDEBUG{fprintf(dbOut, "<Z>");}
			return;
		}
	}

	/* Was normal, becomes normal. */
	xdfAssemble(pxdf, sign, expon, pb);
	xfloatDEBUG{fprintf(dbOut, "<C>");}
}

#endif  /* FOAM_RTS */

/*****************************************************************************
 *
 * :: fracNormalize
 * :: fracDenormalize
 *
 ****************************************************************************/

void	
fracNormalize(int *pexponent, int nb, UByte *pfracbytes)
{
	int	ix1;

	/* if (*pexponent > expmin) return; */

	ix1 = bfFirst1(nb, pfracbytes);
	if (ix1 == -1) return;

	/* Shift first explicit 1 out the most significant end. */
	bfShiftUp(nb, pfracbytes, ix1 + 1, pfracbytes, int0);
	*pexponent -= ix1 + 1;
}

void	
fracDenormalize(int *pexponent, int expmin, int nb, UByte *pfracbytes,
		   int lglgBase, int hasNorm1)
{
	int	ix1;

	if (*pexponent > expmin) return;

	ix1 = expmin - *pexponent;

	/* Shift explicit 1 down in from the most significant end. */
	bfShiftDn(nb, pfracbytes, ix1 << lglgBase, pfracbytes, int0, hasNorm1);

	*pexponent += ix1;
}



int
xdfPrint(FILE *fout, XDFloat *pxdf)
{
	int	i, cc = 0;

	for (i = 0; i < 2; i++) {
		if (i != 0) fprintf(fout, ".");
		cc += fprintf(fout, "%02x", XDF_UByte(pxdf, i));
	}
	cc += fprintf(fout, ":");
	for (i = 2; i < sizeof(XDFloat); i++) {
		if (i != 2) fprintf(fout, ".");
		cc += fprintf(fout, "%02x", XDF_UByte(pxdf, i));
	}
	return cc;
}

#ifndef  FOAM_RTS

/*****************************************************************************
 *
 * :: xfloatDumpInfo
 *
 ****************************************************************************/

void
xfloatDumpInfo(FILE *fout)
{
	fprintf(fout, "--------------------------------------\n");
	fprintf(fout, "SF_HasNANs = %d\n", SF_HasNANs);
	fprintf(fout, "SF_HasNorm1 = %d\n", SF_HasNorm1);
	fprintf(fout, "SF_LgBase = %d\n", SF_LgBase);
	fprintf(fout, "SF_Excess = %d\n", SF_Excess);
	fprintf(fout, "SF_FracOff = %d\n", SF_FracOff);
	fprintf(fout, "DF_HasNANs = %d\n", DF_HasNANs);
	fprintf(fout, "DF_HasNorm1 = %d\n", DF_HasNorm1);
	fprintf(fout, "DF_LgBase = %d\n",  DF_LgBase);
	fprintf(fout, "DF_Excess = %d\n", DF_Excess);
	fprintf(fout, "DF_FracOff = %d\n", DF_FracOff);

	fprintf(fout, "XSF_HasNANs = %d\n", XSF_HasNANs);
	fprintf(fout, "XSF_HasNorm1 = %d\n", XSF_HasNorm1);
	fprintf(fout, "XSF_LgBase = %d\n", XSF_LgBase);
	fprintf(fout, "XSF_Excess = %d\n", XSF_Excess);
	fprintf(fout, "XSF_FracOff = %d\n", XSF_FracOff);
	fprintf(fout, "XDF_HasNANs = %d\n", XDF_HasNANs);
	fprintf(fout, "XDF_HasNorm1 = %d\n", XDF_HasNorm1);
	fprintf(fout, "XDF_LgBase = %d\n",  XDF_LgBase);
	fprintf(fout, "XDF_Excess = %d\n", XDF_Excess);
	fprintf(fout, "XDF_FracOff = %d\n", XDF_FracOff);

	fprintf(fout, "--------------------------------------\n");
	fprintf(fout, "SF_FracShift = %d\n", SF_FracShift);
	fprintf(fout, "SF_FracIx0 = %d\n", SF_FracIx0);
	fprintf(fout, "SF_FracSh0 = %d\n", SF_FracSh0);
	fprintf(fout, "SF_SignMask = %x\n", SF_SignMask);
	fprintf(fout, "SF_FracMask = %x\n", SF_FracMask);
	fprintf(fout, "SF_ExponMask = %x\n", SF_ExponMask);
	fprintf(fout, "SF_ExponMin = %d\n", SF_ExponMin);
	fprintf(fout, "SF_ExponNAN = %d\n", SF_ExponNAN);
	fprintf(fout, "DF_FracShift = %d\n", DF_FracShift);
	fprintf(fout, "DF_FracIx0 = %d\n", DF_FracIx0);
	fprintf(fout, "DF_FracSh0 = %d\n", DF_FracSh0);
	fprintf(fout, "DF_SignMask = %x\n", DF_SignMask);
	fprintf(fout, "DF_FracMask = %x\n", DF_FracMask);
	fprintf(fout, "DF_ExponMask = %x\n", DF_ExponMask);
	fprintf(fout, "DF_ExponMin = %d\n", DF_ExponMin);
	fprintf(fout, "DF_ExponNAN = %d\n", DF_ExponNAN);

	fprintf(fout, "XSF_FracShift = %d\n", XSF_FracShift);
	fprintf(fout, "XSF_FracIx0 = %d\n", XSF_FracIx0);
	fprintf(fout, "XSF_FracSh0 = %d\n", XSF_FracSh0);
	fprintf(fout, "XSF_SignMask = %x\n", XSF_SignMask);
	fprintf(fout, "XSF_FracMask = %x\n", XSF_FracMask);
	fprintf(fout, "XSF_ExponMask = %x\n", XSF_ExponMask);
	fprintf(fout, "XSF_ExponMin = %d\n", XSF_ExponMin);
	fprintf(fout, "XSF_ExponNAN = %d\n", XSF_ExponNAN);
	fprintf(fout, "XDF_FracShift = %d\n", XDF_FracShift);
	fprintf(fout, "XDF_FracIx0 = %d\n", XDF_FracIx0);
	fprintf(fout, "XDF_FracSh0 = %d\n", XDF_FracSh0);
	fprintf(fout, "XDF_SignMask = %x\n", XDF_SignMask);
	fprintf(fout, "XDF_FracMask = %x\n", XDF_FracMask);
	fprintf(fout, "XDF_ExponMask = %x\n", XDF_ExponMask);
	fprintf(fout, "XDF_ExponMin = %d\n", XDF_ExponMin);
	fprintf(fout, "XDF_ExponNAN = %d\n", XDF_ExponNAN);
	fprintf(fout, "--------------------------------------\n");

}


#endif /* FOAM_RTS */

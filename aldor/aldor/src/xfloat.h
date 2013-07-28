/*****************************************************************************
 *
 * xfloat.h: External floating point format
 *
 * Copyright (c) 1990-2007 Aldor Software Organization Ltd (Aldor.org).
 *
 ****************************************************************************/

#ifndef _XFLOAT_H_
#define _XFLOAT_H_

/*****************************************************************************
 *
 * The following are used to ensure portable floating point data formats
 * for library files across different platforms.
 *
 * The size macros involved represent numbers of bits/bytes written
 * to library files, and not sizes of data structures in memory.
 * As a result, they must not use 'sizeof' to create their value.
 *
 *
 * {X,}[SD]FloatFr{,X}[SD]Float provide a portable translation to and from
 * an extended IEEE floating-point format.
 *
 * Floating point formats:
 *
 * XSFloat   seeeeeeeeeeeeeeeffffffffffffffffffffffffffffffff
 * XDFloat   seeeeeeeeeeeeeeeffffffffffffffffffffffffffffffff
 *	                     ffffffffffffffffffffffffffffffff
 *
 * SIeee                     seeeeeeeefffffffffffffffffffffff
 * DIeee                     seeeeeeeeeeeffffffffffffffffffff
 *                           ffffffffffffffffffffffffffffffff
 *
 * S370                      seeeeeeeffffffffffffffffffffffff
 * D370                      seeeeeeeffffffffffffffffffffffff
 *                           ffffffffffffffffffffffffffffffff
 *
 ****************************************************************************/

#include "cport.h"
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

typedef struct {
	UByte	sexp[2];   /* sign and signed base-2 exponent */
	UByte	frac[4];   /* fraction with implicit msb */
} XSFloat;

typedef struct {
	UByte	sexp[2];   /* sign and signed base-2 exponent */
	UByte	frac[8];   /* fraction with implicit msb */
} XDFloat;

# define XSFLOAT_BYTES		6	/* Not sizeof(...)!  File rep size. */
# define XDFLOAT_BYTES		10	/* Not sizeof(...)!  File rep size. */

/*
 * These properties correspond to those defined for native floats in cport.h.
 * They do not vary, however, from platform to platform.
 */
#define XSF_HasNANs	1
#define XSF_HasNorm1	1
#define XSF_LgLgBase	0
#define XSF_Excess	0x3ffe
#define XSF_FracOff	16

#define XDF_HasNANs	1
#define XDF_HasNorm1	1
#define XDF_LgLgBase	0
#define XDF_Excess	0x3ffe
#define XDF_FracOff	16

#define XSF_UByte(px,i)	(((UByte  *) (px))[i])
#define XDF_UByte(px,i)	(((UByte  *) (px))[i])

/*
 * Classification of floating point numbers.
 */
enum floatCase {
	FLOAT_NORM,
	FLOAT_DENORM,
	FLOAT_ZERO,
	FLOAT_NAN,
	FLOAT_INF
};

typedef enum floatCase FloatCase;

extern FloatCase  sfClassify	 (ALDOR_SF_TYPE *);
extern void	  sfAssemble	 (ALDOR_SF_TYPE *,   Bool,   int,   UByte *);
extern void	  sfDissemble	 (ALDOR_SF_TYPE *,   Bool *, int *, UByte *, Bool *);
extern int	  sfExponMin	 (void);
extern int	  sfExponNAN	 (void);

extern FloatCase  dfClassify	 (double *);
extern void	  dfAssemble	 (double *,  Bool,   int,   UByte *);
extern void	  dfDissemble	 (double *,  Bool *, int *, UByte *, Bool *);
extern int	  dfExponMin	 (void);
extern int	  dfExponNAN	 (void);

extern FloatCase  xsfClassify	 (XSFloat *);
extern void	  xsfAssemble	 (XSFloat *, Bool,   int,   UByte *);
extern void	  xsfDissemble	 (XSFloat *, Bool *, int *, UByte *);
extern int	  xsfExponMin	 (void);
extern int	  xsfExponNAN	 (void);
extern void 	  xsfToNative	 (XSFloat *, ALDOR_SF_TYPE *);
extern void 	  xsfFrNative	 (XSFloat *, ALDOR_SF_TYPE *);

extern FloatCase  xdfClassify	 (XDFloat *);
extern void	  xdfAssemble	 (XDFloat *, Bool,   int,   UByte *);
extern void	  xdfDissemble	 (XDFloat *, Bool *, int *, UByte *);
extern int	  xdfExponMin	 (void);
extern int	  xdfExponNAN	 (void);
extern void 	  xdfToNative	 (XDFloat *, double *);
extern void 	  xdfFrNative	 (XDFloat *, double *);

extern void	  fracNormalize	 (int *, int nb, UByte *);
extern void	  fracDenormalize(int *, int, int, UByte *,  int, int);

extern int	  xdfPrint       (FILE *, XDFloat *pxdf);
extern void	  xfloatDumpInfo (FILE *);

#endif /*! _XFLOAT_H_ */

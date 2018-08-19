/*****************************************************************************
 *
 * cport.c: Non-portable, C-compiler specific code.
 *
 * Copyright (c) 1990-2007 Aldor Software Organization Ltd (Aldor.org).
 *
 ****************************************************************************/

#include "cport.h"

/*****************************************************************************
 *
 * 16. Pointer conversion
 *
 ****************************************************************************/

Bool
ptrEqualFn(Pointer p1, Pointer p2)
{
	return ptrEqual(p1, p2);
}

Hash
ptrHashFn(Pointer p)
{
	Hash h = (Hash) p>>2; // Bottom bit is boring.

	return h * 2654435761;
}

# if defined(CC_noncanonical_pointer) && defined(OS_MS_DOS) &&	\
         (defined(CC_BORLAND) || defined(CC_MICROSOFT))

#include <dos.h>

# define  SEG_SHIFT 4
# define  OFF_MASK  ((1 << SEG_SHIFT) - 1)

# define MAKELONG(lo, hi) \
    ((long)(((unsigned int)(lo))|(((unsigned long)((unsigned int)(hi)))<<16)))

#if defined(CC_MICROSOFT)
# define MK_FP(sel, off)    ((Pointer) MAKELONG(off, sel))
#endif

Pointer
ptrCanon(Pointer p)
{
	p = MK_FP(FP_SEG(p) + (FP_OFF(p)>>SEG_SHIFT), FP_OFF(p) & OFF_MASK);
	return p;
}


Pointer
ptrOff(const char *s, long offset)
{
	long    l = ((long) FP_SEG(s) << SEG_SHIFT) + FP_OFF(s) + offset;
	Pointer p = MK_FP((unsigned)(l >> SEG_SHIFT), l & OFF_MASK);
	return p;
}

Bool
ptrEqual(Pointer p, Pointer q)
{
	return ptrCanon(p) == ptrCanon(q);
}

Hash
ptrHash(Pointer p)
{
	return (Hash) p;
}

long
ptrDiff(const char *p, const char *q)
{
	long l = ptrToLong(p) - ptrToLong(q);
	return l;
}

long
ptrToLong(Pointer p)
{
	long l = ((long) FP_SEG(p) << SEG_SHIFT) + FP_OFF(p);
	return l;
}

Pointer
ptrFrLong(long l)
{
	Pointer p = MK_FP((unsigned)(l >> SEG_SHIFT), l & OFF_MASK);
	return p;
}

#endif /* CC_noncanonical_pointer && OS_MS_DOS && CC_BORLAND */

ThatsAll

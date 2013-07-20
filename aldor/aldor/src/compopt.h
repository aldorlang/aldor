/*****************************************************************************
 *
 * compopt.h: optimisations for various platforms (declarations)
 *
 * Copyright (c) 1990-2007 Aldor Software Organization Ltd (Aldor.org).
 *
 ****************************************************************************/

#ifndef _COMPOPT_H_
#define _COMPOPT_H_
 
#include "cport.h"
#include "optcfg.h"

/*
 * Internal (both to RTS and compiler) optimizations.
 *
 */

#ifdef OPT_Linux_i386

#define xxTimesDoubleMacro					\
void xxTimesDouble(ULong *hi, ULong *lo, ULong a, ULong b)		\
{								\
	/* asms are: <string> : <output> : <input>: <side-effects> */  \
	__asm__( "mull %3"						\
	    : "=a" (*lo), "=d" (*hi)				\
	    : "a" (a), "b" (b)					\
	    : "cc");				\
}

#define xxDivideDoubleMacro \
void xxDivideDouble(ULong *pqhi, ULong *pqlo, ULong *prem,	\
	       ULong hi, ULong lo, ULong d)		\
	  						\
{							\
	ULong qhi;					\
	__asm__("divl %4"					\
	    : "=a" (qhi), "=d" (hi)			\
	    : "d" (0), "a" (hi), "r" (d)		\
	    : "cc");			\
							\
	*pqhi = qhi;					\
							\
	__asm__("divl %4"					\
	    : "=a" (*pqlo), "=d" (*prem)		\
	    : "d" (hi), "a" (lo), "r" (d)		\
	    : "cc");			\
}

#define xxModDoubleMacro \
ULong xxModDouble(ULong hi, ULong lo, ULong d)	\
{						\
	ULong r1;				\
						\
	__asm__("divl %3"				\
	    : "=d" (hi)				\
	    : "d" (0), "a" (hi), "r" (d)	\
	    : "cc");		\
						\
	__asm__("divl %3"				\
	    : "=d" (r1)				\
	    : "d" (hi), "a" (lo), "r" (d)	\
	    : "cc" );		\
						\
	return r1;				\
}

extern __inline__ xxTimesDoubleMacro
extern __inline__ xxDivideDoubleMacro
extern __inline__ xxModDoubleMacro

extern void	xxTestGtDouble(int *, ULong, ULong, ULong, ULong);
extern void	xxPlusStep    (ULong *, ULong *, ULong, ULong, ULong);
extern void	xxTimesStep   (ULong *, ULong *, ULong, ULong, ULong, ULong);

#endif

#ifdef OPT_Sparc_v8

#define xxTimesDoubleMacro 								\
void xxTimesDouble(ULong *hi, ULong *lo, ULong a, ULong b)				\
{											\
	__asm__ ("umul %2,%3,%1;rd %%y,%0"						\
		 : "=r" (*hi), "=r" (*lo)						\
		 : "r" (a), "r" (b));							\
}

#define xxDivideDoubleMacro								\
void xxDivideDouble(ULong *pqhi, ULong *pqlo, ULong *prem,				\
	       ULong hi, ULong lo, ULong d)						\
{											\
	ULong qhi;									\
	/* It seems you have to write to %y in any case... */				\
	__asm__ ("mov %2,%%y;nop;nop;nop;udiv %3,%4,%0;umul %0,%4,%1;sub %3,%1,%1"	\
		 : "=&r" (qhi), "=&r" (hi)						\
		 : "r" (0), "r" (hi), "r" (d));						\
	*pqhi = qhi;									\
	__asm__ ("mov %2,%%y;nop;nop;nop;udiv %3,%4,%0;umul %0,%4,%1;sub %3,%1,%1"	\
		 : "=&r" (*pqlo), "=&r" (*prem)						\
		 : "r" (hi), "r" (lo), "r" (d));					\
}

#define xxModDoubleMacro 			\
ULong xxModDouble(ULong hi, ULong lo, ULong d)	\
{											\
	ULong r1, foo;									\
	/* It seems you have to write to %y in any case... */				\
	__asm__ ("mov %2,%%y;nop;nop;nop;udiv %3,%4,%0;umul %0,%4,%1;sub %3,%1,%1"	\
		 : "=&r" (r1), "=&r" (hi)						\
		 : "r" (0), "r" (hi), "r" (d));						\
											\
	__asm__ ("mov %1,%%y;nop;nop;nop;udiv %2,%3,%0;umul %0,%4,%1;sub %3,%1,%1"	\
		 : "=&r" (foo),"=&r" (r1)						\
		 : "r" (hi), "r" (lo), "r" (d));					\
											\
	return r1;									\
}

extern __inline__ xxTimesDoubleMacro
extern __inline__ xxDivideDoubleMacro
extern __inline__ xxModDoubleMacro

extern void	xxTestGtDouble(int *, ULong, ULong, ULong, ULong);
extern void	xxPlusStep    (ULong *, ULong *, ULong, ULong, ULong);
extern void	xxTimesStep   (ULong *, ULong *, ULong, ULong, ULong, ULong);

#endif

#endif /*!_COMPOPT_H_*/

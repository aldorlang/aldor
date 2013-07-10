/*****************************************************************************
 *
 * foamopt.h: optimisations for various platforms 
 *
 * Copyright (c) 1990-2007 Aldor Software Organization Ltd (Aldor.org).
 *
 ****************************************************************************/

#ifndef _FOAMOPT_H_
#define _FOAMOPT_H_
 
/* Define optimisations we want */

/* 
 * GCC inline functions are 'interesting' --- they don't guarantee
 * that an inline will be honoured, so it is a good idea to have a
 * file that declares non-extern versions.  See foamopt.c 
 *
 * The name of the game with asms is to make them as short as possible
 * gcc likes nothing better than to try and cleverly allocate
 * registers so that long asm sequences will be badly messed up.
 * Thus, for preference use "r" for arg registers, or a particular one
 * if that won't work.  It is usually a bad idea to try putting in
 * "mv" instructions into asm sequences.
 */

/* Implementations */
#ifdef OPT_Linux_i386

extern void	fiWordPlusStep     _of_((FiWord, FiWord, FiWord,
					 FiWord *, FiWord *));
extern void	fiWordTimesStep    _of_((FiWord, FiWord, FiWord, FiWord,
					 FiWord *, FiWord *));

#define fiWordTimesDoubleMacro						  \
void fiWordTimesDouble(FiWord a, FiWord b, 				  \
		  FiWord *hi, FiWord *lo)				  \
{									  \
	/* gcc asms are: <string> : <output> : <input>: <side-effects> */ \
	__asm__( "mull %3"							  \
	    : "=a" (*lo), "=d" (*hi)					  \
	    : "a" (a), "b" (b)						  \
	    : "cc");					  \
}

#define fiWordDivideDoubleMacro					\
void fiWordDivideDouble(FiWord hi, FiWord lo, FiWord d, 	\
		   FiWord *pqhi, FiWord *pqlo, 			\
		   FiWord *prem)				\
{								\
	FiWord qhi;						\
	__asm__("divl %4"						\
	    : "=a" (qhi), "=d" (hi)				\
	    : "d" (0), "a" (hi), "r" (d)			\
	    : "cc");				\
								\
	*pqhi = qhi;						\
								\
	__asm__("divl %4"						\
	    : "=a" (*pqlo), "=d" (*prem)			\
	    : "d" (hi), "a" (lo), "r" (d)			\
	    : "cc");				\
								\
}

extern __inline__ fiWordDivideDoubleMacro
extern __inline__ fiWordTimesDoubleMacro

#endif


#ifdef OPT_Sparc_v8

extern void	fiWordPlusStep     _of_((FiWord, FiWord, FiWord,
					 FiWord *, FiWord *));
extern void	fiWordTimesStep    _of_((FiWord, FiWord, FiWord, FiWord,
					 FiWord *, FiWord *));

#define fiWordTimesDoubleMacro 								\
void fiWordTimesDouble(FiWord a, FiWord b, FiWord *hi, FiWord *lo)			\
{											\
	__asm__ ("umul %2,%3,%1;rd %%y,%0"						\
		 : "=r" (*hi), "=r" (*lo)						\
		 : "r" (a), "r" (b));							\
}

#define fiWordDivideDoubleMacro								\
void fiWordDivideDouble(FiWord hi, FiWord lo, FiWord d, 				\
		    FiWord *pqhi, FiWord *pqlo, FiWord *prem)				\
{											\
	FiWord qhi;									\
	/* It seems you have to write to %y in any case... */				\
	__asm__ ("mov %2,%%y;nop;nop;nop;udiv %3,%4,%0;umul %0,%4,%1;sub %3,%1,%1"	\
		 : "=&r" (qhi), "=&r" (hi)						\
		 : "r" (0), "r" (hi), "r" (d));						\
	*pqhi = qhi;									\
	__asm__ ("mov %2,%%y;nop;nop;nop;udiv %3,%4,%0;umul %0,%4,%1;sub %3,%1,%1"	\
		 : "=&r" (*pqlo), "=&r" (*prem)						\
		 : "r" (hi), "r" (lo), "r" (d));					\
}


extern __inline__ fiWordDivideDoubleMacro
extern __inline__ fiWordTimesDoubleMacro

#endif

#endif /*!_FOAMOPT_H_*/

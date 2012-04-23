/*****************************************************************************
 *
 * bitv.h: Bit-vector operations
 *
 * Copyright (c) 1990-2007 Aldor Software Organization Ltd (Aldor.org).
 *
 ****************************************************************************/

#ifndef _BITV_H_
#define _BITV_H_

#include "axlport.h"

typedef ULong		BitvWord;

typedef BitvWord	*Bitv;

struct _BitvClass {

	Length		nbits;
	Length		nwords;

};

typedef struct _BitvClass * BitvClass;

#define	bitvClassSize(bClass)	((bClass)->nbits)

/*
 * This controls the behaviour of the Bitv operations.
 */
extern BitvClass	bitvClassCreate (int nbits);
extern void		bitvClassDestroy(BitvClass);

/*
 * General operations
 */
extern Bitv	bitvNew		(BitvClass);
extern void	bitvFree	(Bitv);
extern Bitv	bitvResize	(BitvClass, BitvClass, Bitv);
extern Bitv * 	bitvManyNew	(BitvClass, Length n);
extern void	bitvManyFree	(Bitv *);

extern int	bitvPrint	(FILE *, BitvClass, Bitv);
extern String   bitvToString    (BitvClass, Bitv);
extern int	bitvPrintDb	(BitvClass, Bitv);
extern Bool	bitvEqual	(BitvClass, Bitv, Bitv);
extern int      bitvMax         (BitvClass, Bitv);
extern int      bitvCount       (BitvClass, Bitv);
extern int      bitvCountTo     (BitvClass, Bitv, int n);

/*
 * Arithmetic: These update the result 'r'.
 */
extern void	bitvSetAll	(BitvClass, Bitv r);
extern void	bitvClearAll	(BitvClass, Bitv r);

extern int	bitvTest 	(BitvClass, Bitv r, int ix);
extern void	bitvSet  	(BitvClass, Bitv r, int ix);   
extern void	bitvClear	(BitvClass, Bitv r, int ix);   

extern void	bitvCopy	(BitvClass, Bitv r, Bitv a);
extern void	bitvNot		(BitvClass, Bitv r, Bitv a);
extern void	bitvAnd		(BitvClass, Bitv r, Bitv a, Bitv b);
extern void	bitvOr		(BitvClass, Bitv r, Bitv a, Bitv b);
extern void	bitvMinus	(BitvClass, Bitv r, Bitv a, Bitv b);

/*
 * Esoterica: for clients.
 */
extern	int	bitvUnique1IndexInRange(BitvClass, Bitv, int org, int lim);
extern  Bitv    bitvFromInt(BitvClass, int);
extern  int     bitvToInt(BitvClass, Bitv);
#endif /* !_BITV_H_ */

/*****************************************************************************
 *
 * dnf.h: Disjunctive normal form for boolean expressions.
 *
 * Copyright (c) 1990-2007 Aldor Software Organization Ltd (Aldor.org).
 *
 ****************************************************************************/

#ifndef _DNF_H_
#define _DNF_H_

#include "cport.h"

/*
 * The literals are numbers:
 * A positive number means the variable is taken to be true.  
 * A negative number means the variable is taken to be false.
 *
 * false         => []		-- 0 term "or"  = "or"s  identity, "false"
 * true          => [[]]	-- 0 term "and" = "and"s identity, "true"
 * X1            => [[1]]
 * X1/\~X2 \/ X3 => [[1,-2], [3]]
 */

typedef struct dnf_Or *	DNF;		/* OR  of terms */
typedef struct dnf_And* DNF_And;	/* AND of atoms */
typedef int		DNF_Atom;

struct dnf_Or {
	int		argc;
	DNF_And 	argv[NARY];
};

struct dnf_And {
	Length		argc;
	DNF_Atom	argv[NARY];
};

extern DNF	dnfTrue		(void);
extern DNF	dnfFalse	(void);

extern Bool	dnfIsTrue	(DNF);
extern Bool	dnfIsFalse	(DNF);

extern DNF	dnfAtom		(DNF_Atom);
extern DNF	dnfNotAtom	(DNF_Atom);

extern DNF	dnfNot		(DNF);
extern DNF	dnfAnd		(DNF, DNF);
extern DNF	dnfOr		(DNF, DNF);

extern DNF	dnfCopy		(DNF);
extern void	dnfFree		(DNF);

extern int	dnfPrint	(FILE *, DNF);

extern Bool	dnfEqual	(DNF, DNF);
extern Bool	dnfImplies	(DNF, DNF);

extern DNF	dnfFollow	(DNF);
extern void	dnfAlias	(DNF, DNF);

extern void dnfMap(Bool (*mapFn)(void*, DNF_Atom),  void * clos, DNF xx);
extern Bool dnfExpandImplies(Bool (*testFn)(void *, DNF_Atom, DNF_Atom), 
			     void *clos,
			     DNF xx, DNF yy);

extern int dnfFormatter(OStream ostream, Pointer ptr);

#endif /* !_DNF_H_ */

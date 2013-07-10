
/*****************************************************************************
 *
 * of_rrfmt.h: Dynamic format (RRFmt) optimizations.
 *
 * Copyright (c) 1990-2007 Aldor Software Organization Ltd (Aldor.org).
 *
 ****************************************************************************/

#ifndef OF_RRFMT_H
#define OF_RRFMT_H

#include "axlobs.h"
#include "optfoam.h"
#include "opttools.h"
#include "table.h"

/*****************************************************************************
 *
 * :: External entry points
 *
 ****************************************************************************/

extern	void	rrUnitEmerge		(Foam /* unit */);
	/* Replace raw records with plain records or arrays */

extern	Bool	rrRRFmtIsIndependent	(Foam /* rrfmt */);
	/* Check for independent, valid RRFmt instruction */


/*****************************************************************************
 *
 * :: Useful macros
 *
 ****************************************************************************/

#define rrIsDef(foam) (				\
		(foamTag(foam) == FOAM_Set) ||	\
		(foamTag(foam) == FOAM_Def)	\
	)

#define rrIsLocal(foam) (			\
		(foamTag(foam) == FOAM_Loc) ||	\
		(foamTag(foam) == FOAM_Par)	\
	)

#define rrIsVar(foam) (				\
		(foamTag(foam) == FOAM_Lex) ||	\
		(foamTag(foam) == FOAM_Glo) ||	\
		rrIsLocal(foam)			\
	)

#define rrReachingDefs(foam)	((Foam)udReachingDefs(foam))

#define rrDefNo(foam)		((foam)->foamGen.hdr.info.defNo)

#define rrMarkDefSeen(n)	bitvSet(rrInfo.class, rrInfo.seen, n)
#define rrDefAlreadySeen(n)	bitvTest(rrInfo.class, rrInfo.seen, n)
#define rrDefIsKnown(n)		bitvTest(rrInfo.class, rrInfo.cached, n)
#define rrDefHasEscaped(n)	bitvTest(rrInfo.class, rrInfo.escapes, n)

#define rrMarkEscapingDef(n)					\
	{							\
		bitvSet(rrInfo.class, rrInfo.cached, n);	\
		bitvSet(rrInfo.class, rrInfo.escapes, n);	\
	}

#define rrMarkNonEscapingDef(n)					\
	{							\
		bitvSet(rrInfo.class, rrInfo.cached, n);	\
		bitvClear(rrInfo.class, rrInfo.escapes, n);	\
	}


/* Information about reaching definitions */
typedef struct
{
	/*
	 * Unit-level information which is valid throughout
	 * the optimisation of all Progs.
	 */
	FoamBox		formats;	/* Format DDecls */
	Table		remap;		/* Raw record -> plain format */


	/*
	 * Prog-level information: this is updated during the
	 * optimisation of each Prog. The contents of these
	 * fields is invalid outside the prog optimisers.
	 */
	Bool		changed;	/* Prog was modified */
	AInt		npars;		/* Number of parameters */
	AInt		ndefs;		/* Number of definitions */
	BitvClass	class;		/* Class for cached/escapes */
	Bitv		seen;		/* Already visited this node */
	Bitv		cached;		/* Result is in escapes? */
	Bitv		escapes;	/* Cached results */
	VarPool		locals;		/* Extendable pool of locals */
} rrEmergeInfo;


#endif


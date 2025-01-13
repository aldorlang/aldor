#ifndef _OPTINFO_H
#define _OPTINFO_H

#include "axlobs.h"
#include "of_util.h"

struct optInfo {
	InlineState	inlState;
	DeadVarState	dvState;
	Stab		stab;
	Syme		syme;
	int		constNum;
	Bool		isGener;     	/* Is generator? */
	Foam		prog;
	Foam		seq;
	FoamList	seqBody;
	VarPool		locals;
	int		numLabels;
	int		newLabel;
	Foam		denv;
	Bool		changed;
	DvUsage		localUsage;

	FlowGraph	flog;
	PriQ		priq;		/* Priority queue for this prog	*/
	Bool		converged;	/* Usedef info available	*/
	UShort		numRefs;	/* how many progs call this     */
	unsigned	originalSize;	/* size before inlining 	*/
	unsigned	size;		/* size during inlining		*/

	UShort		optMask;	/* Pending optimizations  */

};

# define optInfoRefs(opt)	(opt)->numRefs

extern OptInfo	optInfoNew (Stab, Foam, Syme, Bool);
extern OptInfo	optInfoNew0(Stab, Foam, Syme, Bool, Bool);

#endif

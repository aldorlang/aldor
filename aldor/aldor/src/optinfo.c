#include "axlobs.h"
#include "fbox.h"
#include "optinfo.h"
#include "store.h"
#include "of_util.h"

/*
 * Create a new OptInfo structure.
 */
OptInfo
optInfoNew(Stab stab, Foam prog, Syme lhs, Bool isGener)
{
	return optInfoNew0(stab, prog, lhs, isGener, false);
}

OptInfo
optInfoNew0(Stab stab, Foam prog, Syme lhs, Bool isGener, Bool external)
{
	OptInfo		new;

	new = (OptInfo) stoAlloc(OB_Other, sizeof(struct optInfo));

	new->inlState	 = (external ? INL_Inlined : INL_NotInlined);
	new->stab	 = stab;
	new->syme	 = lhs;
	new->prog	 = prog;
	new->seq	 = prog->foamProg.body;
	new->locals	 = vpNew(fboxNew(prog->foamProg.locals));
	new->numLabels	 = prog->foamProg.nLabels;
	new->denv	 = prog->foamProg.levels;
	new->seqBody	 = 0;

	new->localUsage	 = 0;
	new->constNum	 = -1;
	new->isGener	 = isGener;

	new->flog	 = 0;
	new->priq	 = 0;
	new->converged   = false;
	new->numRefs	 = 0;
	new->originalSize = 0;
	new->size	  = 0;
	new->optMask	  = 0xffff;

	return new;
}


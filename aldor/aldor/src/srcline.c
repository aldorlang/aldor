/*****************************************************************************
 *
 * srcline.c: SrcLine data structure.
 *
 * Copyright (c) 1990-2007 Aldor Software Organization Ltd (Aldor.org).
 *
 ****************************************************************************/

#include "axlobs.h"
#include "store.h"
#include "srcline.h"

SrcLine
slineNew(SrcPos spos, int indent, String text)
{
	SrcLine sl;
	sl = (SrcLine) stoAlloc((unsigned) OB_SrcLine, sizeof(*sl));
	sl->spos          = spos;
	sl->indentation   = indent;
	sl->isSysCmd      = false;
	sl->sysCmdHandled = false;
	sl->isEndifLine   = false;
	sl->text          = strCopy(text);

	return sl;
}

SrcLine
slineNewSysCmd(SrcPos spos, int indent, String text, Bool isHandled)
{
	SrcLine sl        = slineNew(spos, indent, text);
	sl->isSysCmd      = true;
	sl->sysCmdHandled = isHandled;
	return sl;
}

void
slineFree(SrcLine sl)
{
	strFree(sl->text);
	stoFree((Pointer) sl);
}

int
slinePrint(FILE *fout, SrcLine sl)
{
	int     cc;
	cc  = fprintf(fout, "[%d] %s, line %d: ",
#if EDIT_1_0_n1_07
		      (int) sposGlobalLine(sl->spos),
		      fnameUnparseStatic(sposFile(sl->spos)),
		      (int) sposLine(sl->spos));
#else
		      sposGlobalLine(sl->spos),
		      fnameUnparseStatic(sposFile(sl->spos)),
		      sposLine(sl->spos));
#endif
	cc += fprintf(fout, "%*s", sl->indentation, "");
	cc += fprintf(fout, "%s",  sl->text);
	return cc;
}

int
sllPrint(FILE *fout, SrcLineList sll)
{
	return listPrint(SrcLine)(fout, sll, slinePrint);
}

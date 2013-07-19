/*****************************************************************************
 *
 * tconst.c: Type form constraints.
 *
 * Copyright (c) 1990-2007 Aldor Software Organization Ltd (Aldor.org).
 *
 ****************************************************************************/

#include "axlobs.h"
#include "debug.h"
#include "format.h"
#include "store.h"
#include "util.h"

Bool	tcDebug		= false;

#define	tcDEBUG(s)		DEBUG_IF(tcDebug, s)

/*
 * Type form constraints:
 *
 *	TC_Satisfies		argv[0] satisfies argv[1]
 *
 */

extern void		terrorTypeConstFailed	(TConst);

/******************************************************************************
 *
 * :: Local operations
 *
 *****************************************************************************/

local TForm
tcFollowArg(TConst tc, Length i)
{
	return tcArgv(tc)[i] = tfDefineeType(tcArgv(tc)[i]);
}

/******************************************************************************
 *
 * :: Basic operations
 *
 *****************************************************************************/

static int		tcCount			= 0;
static int		tcSerialNum		= 0;
static TConstList	tcStack			= listNil(TConst);
static TConstList	tcParents		= listNil(TConst);
#ifndef NDEBUG
static TConstList	tcList			= listNil(TConst);
#endif

void
tcInit(void)
{
	tcCount  = 0;
	tcSerialNum = 0;
	tcStack  = listNil(TConst);
	tcDEBUG(tcList = listNil(TConst));
}

void
tcFini(void)
{
	tcDEBUG(listPrint(TConst)(dbOut, tcList, tcPrint));
	if (tcCount != 0)
		bug("%d constraints not checked", tcCount);
}

void
tcSatPush(TForm S, TForm T)
{
	tcNewSat(NULL, S, T, NULL);
}

void
tcSatPop(void)
{
	tcParents = listFreeCons(TConst)(tcParents);
}

TConst
tcAlloc(TConstTag tag, TForm owner, Length argc, va_list argp)
{
	TConst		tc;
	Length		i;

	assert(owner == NULL || tfIsPending(owner));

	tc = (TConst) stoAlloc((unsigned) OB_TConst,
			       sizeof(*tc) + argc * sizeof(TForm));

	tc->tag		= tag;
	tc->pos		= NULL;
	tc->parent	= NULL;
	tc->id		= NULL;
	tc->serial	= ++tcSerialNum;
	tc->owner	= owner;
	tc->argc	= argc;
	tc->argv	= (argc ? (TForm *) (tc + 1) : NULL);

	for (i = 0; i < argc; i += 1)
		tcArgv(tc)[i] = va_arg(argp, TForm);

	for (i = 0; i < argc; i += 1) {
		AbSyn	abi = tfGetExpr(tcArgv(tc)[i]);
		if (abi && !sposIsNone(abPos(abi))) {
			tcPos(tc) = abi;
			break;
		}
	}

	assert(owner == NULL || tcParents);
	if (tcParents) tcSetParent(tc, car(tcParents));

	if (owner == NULL)
		tcParents = listCons(TConst)(tc, tcParents);
	else {
		tcDEBUG(listPush(TConst, tc, tcList));
		tcCount += 1;
	}

	return tc;
}

void
tcFree(TConst tc)
{
	int l0 = 0;
	tcDEBUG(l0 = listLength(TConst)(tcList);
		listPop(TConst, tc, tcList, tcEq);
		assert(l0 - 1 == listLength(TConst)(tcList)););

	tcCount -= 1;
	stoFree((Pointer) tc);
}

void
tcPush(TConst tc)
{
	TForm		owner = tcOwner(tc);
	TConstList	tcl;

	for (tcl = tfConsts(owner); tcl; tcl = cdr(tcl))
		if (tcEqual(tc, car(tcl))) {
			tcFree(tc);
			return;
		}
	listPush(TConst, tc, tfConsts(owner));
}

void
tcPop(TConst tc)
{
	listPop(TConst, tc, tfConsts(tcOwner(tc)), tcEq);
}

Bool
tcEq(TConst tc1, TConst tc2)
{
	return tc1 == tc2;
}

Bool
tcEqual(TConst tc1, TConst tc2)
{
	Length	i;

	if (tcTag(tc1) != tcTag(tc2) || tcArgc(tc1) != tcArgc(tc2))
		return false;

	for (i = 0; i < tcArgc(tc1); i += 1)
		if (tcFollowArg(tc1, i) != tcFollowArg(tc2, i))
			return false;

	return true;
}

int
tcPrint(FILE *f, TConst tc)
{
	int	cc = 0;

#if EDIT_1_0_n1_07
	cc += fprintf(f, "(const[%d]:  ", (int) tcSerial(tc));
#else
	cc += fprintf(f, "(const[%d]:  ", tcSerial(tc));
#endif

	switch (tcTag(tc)) {
	case TC_Satisfies:
		cc += tfPrint(f, tcArgv(tc)[0]);
		cc += fprintf(f, " satisfies ");
		cc += tfPrint(f, tcArgv(tc)[1]);
		break;
	default:
		bugBadCase(tcTag(tc));
	}

	if (tcParent(tc))
		cc += tcPrint(f, tcParent(tc));

	cc += fprintf(f, ")");

	return cc;
}

void
tcNew(TConstTag tag, TForm owner, AbSyn id, Length argc, ...)
{
	TConst	tc;
	va_list	argp;

	va_start(argp, argc);
	tc = tcAlloc(tag, owner, argc, argp);
	va_end(argp);

	tc->id = id;

	if (owner == NULL) return;

	if (tcStack && tcEqual(tc, car(tcStack))) {
		tcFree(tc);
		tc = car(tcStack);
		assert(owner != tc->owner);
		tc->owner = owner;
	}
	tcPush(tc);
}

void
tcNewSat(TForm owner, TForm S, TForm T, AbSyn a)
{
	tcNew(TC_Satisfies, owner, a, 2, S, T);
}

void
tcMove(TForm ntf, TForm otf)
{
	TConstList	tcl;

	tfFollow(ntf);

	for (tcl = tfConsts(otf); tcl; tcl = cdr(tcl))
		tcOwner(car(tcl)) = ntf;

	tfConsts(ntf) = listNConcat(TConst)(tfConsts(ntf), tfConsts(otf));
	tfConsts(otf) = listNil(TConst);

	if (tfIsMeaning(ntf))
		tfCheckConsts(ntf);
}

/******************************************************************************
 *
 * :: tcCheck.
 *
 *****************************************************************************/

void
tfCheckConsts(TForm tf)
{
	while (tfConsts(tf)) {
		TConst	tc = car(tfConsts(tf));
		tcPop(tc);
		tcCheck(tc);
		if (tcOwner(tc) == tf) tcFree(tc);
	}
}

void
tcCheck(TConst tc)
{
	Bool	result;

	listPush(TConst, tc, tcStack);

	switch (tcTag(tc)) {
	case TC_Satisfies:
		result = tfSatisfies(tcArgv(tc)[0], tcArgv(tc)[1]);
		break;
	default:
		bugBadCase(tcTag(tc));
		NotReached(result = false);
	}

	listPop(TConst, tc, tcStack, tcEq);

	if (!result) {
		tcDEBUG({
			tcPrint(dbOut, tc);
			fnewline(dbOut);
		});
		tiTopFns()->terrorTypeConstFailed(tc);
	}
}

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
#include "ti_top.h"
#include "util.h"
#include "tconst.h"
#include "tfsat.h"
#include "tform.h"
#include "ablogic.h"

Bool	tcDebug		= false;

#define tcDEBUG		DEBUG_IF(tc)	afprintf

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
	if (DEBUG(tc)) {
		tcList = listNil(TConst);
	}
}

void
tcFini(void)
{
	if (DEBUG(tc)) {
		listPrint(TConst)(dbOut, tcList, tcPrint);
	}
	if (tcCount != 0)
		bug("%d constraints not checked", tcCount);
}

void
tcSatPush(TForm S, TForm T)
{
	tcNewSat(NULL, NULL, S, T, NULL);
}

void
tcSatPop(void)
{
	tcParents = listFreeCons(TConst)(tcParents);
}

TConst
tcAlloc(TConstTag tag, TForm owner, AbLogic known, AbSyn ab0, Length argc, va_list argp)
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
	tc->known       = known;
	tc->serial	= ++tcSerialNum;
	tc->owner	= owner;
	tc->ab0 	= ab0;
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
		if (DEBUG(tc)) {
			listPush(TConst, tc, tcList);
		}
		tcCount += 1;
	}

	return tc;
}

void
tcFree(TConst tc)
{
	int l0 = 0;
	if (DEBUG(tc)) {
		l0 = listLength(TConst)(tcList);
		listPop(TConst, tc, tcList, tcEq);
		assert(l0 - 1 == listLength(TConst)(tcList));
	}
	tcDEBUG(dbOut, "tcFree: %d %pTForm\n", tc->serial, tcOwner(tc));
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
	tcDEBUG(dbOut, "tcPush: %pTForm owns %pTConst\n", owner, tc);
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

	cc += fprintf(f, "(const[%d]:  ", (int) tcSerial(tc));

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
tcNew(TConstTag tag, TForm owner, AbLogic known, AbSyn id, AbSyn ab0, Length argc, ...)
{
	TConst	tc;
	va_list	argp;

	va_start(argp, argc);
	tc = tcAlloc(tag, owner, known, ab0, argc, argp);
	va_end(argp);

	tc->id = id;

	if (owner == NULL) return;

	if (tcStack && tcEqual(tc, car(tcStack))) {
		tcFree(tc);
		tc = car(tcStack);
		assert(owner != tc->owner);
		tc->owner = owner;
	}
	tcDEBUG(dbOut, "tcNewSat: %pTForm owns %pTConst\n", tcOwner(tc), tc);
	tcPush(tc);
}

void
tcNewSat(TForm owner, AbLogic known, TForm S, TForm T, AbSyn a)
{
	tcNew(TC_Satisfies, owner, ablogCopy(known), a, NULL, 2, S, T);
}

void
tcNewSat1(TForm owner, AbLogic known, AbSyn ab0, TForm S, TForm T, AbSyn a)
{
	tcNew(TC_Satisfies, owner, ablogCopy(known), a, ab0, 2, S, T);
}

void
tcMove(TForm ntf, TForm otf)
{
	TConstList	tcl;

	tfFollow(ntf);

	for (tcl = tfConsts(otf); tcl; tcl = cdr(tcl)) {
		tcDEBUG(dbOut, "tcMove: Moving %d from %pTForm to %pTForm\n",
			tcSerial(car(tcl)), otf, ntf);
		tcOwner(car(tcl)) = ntf;
	}

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
	AbLogic known;
	extern AbLogic abCondKnown;
	
	listPush(TConst, tc, tcStack);

	switch (tcTag(tc)) {
	case TC_Satisfies:
		known = abCondKnown;
		abCondKnown = tcKnown(tc);
		result = tfSatisfies1(tc->ab0, tcArgv(tc)[0], tcArgv(tc)[1]);
		abCondKnown = known;
		break;
	default:
		bugBadCase(tcTag(tc));
		NotReached(result = false);
	}

	listPop(TConst, tc, tcStack, tcEq);
	tcDEBUG(dbOut, "tcCheck: %pTForm %pTConst %oBool\n", tcOwner(tc), tc, result);
	if (!result) {
		if (DEBUG(tc)) {
			tcPrint(dbOut, tc);
			fnewline(dbOut);
		}
		tiTopFns()->terrorTypeConstFailed(tc);
	}
}

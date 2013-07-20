/*****************************************************************************
 *
 * simpl.c: Symbol Implementations.
 *
 * Copyright (c) 1990-2007 Aldor Software Organization Ltd (Aldor.org).
 *
 ****************************************************************************/


#include "axlobs.h"
#include "debug.h"
#include "format.h"
#include "simpl.h"
#include "store.h"
#include "util.h"
#include "sefo.h"

Bool implDebug = false;

#define implDEBUG(s)		DEBUG_IF(implDebug, s)

struct SImplInfo {
	SImplTag tag;
	String   name;
};

#define implName(x)	(implInfo[implTag(x)].name)
extern struct SImplInfo implInfo[];

local SImpl implNew	    	(SImplTag, Length, ...);
local Bool  implSetConstNum0	(SImpl, int, int);
local int   implFindDefIdx0(SImpl impl, int constId);

local SImpl
implNew(SImplTag tag, Length sz, ...)
{
	SImpl 	impl;
	va_list argp;
	int 	i;

	impl = (SImpl) stoAlloc(OB_Other, fullsizeof(*impl, sz, AInt));
	
	impl->implGen.hdr.tag   = tag;
	impl->implGen.hdr.flags = 0;
	va_start(argp, sz);
	for (i=0; i<sz; i++) {
		impl->implGen.argv[i].ptr = va_arg(argp, void *);
	}
	va_end(argp);

	return impl;
}


SImpl
implNone()
{
	SImpl impl = implNew(SIMPL_None, int0);

	return impl;
}

SImpl
implNewLocal(Bool isdef, int idx)
{
	SImpl impl = implNew(SIMPL_Local, 3, idx, -1, NULL);
	
	if (isdef)
		implSetDefault(impl);

	return impl;
}

SImpl
implNewImport(Bool isdef, Lib lib, int idx)
{
	SImpl impl = implNew(SIMPL_Local, 3, -1, idx, lib);
	
	if (isdef)
		implSetDefault(impl);

	return impl;
}

SImpl
implNewInherit(TForm base)
{
	SImpl impl;
	
	impl = implNew(SIMPL_Inherit, 1, base);

	return impl;
}

SImpl
implNewBranch(AbLogic cond, SImpl implTrue, SImpl implFalse)
{
	SImpl impl = implNew(SIMPL_Branch, 3, cond, implTrue, implFalse);

	return impl;
}

SImpl
implNewCond(AbLogic cond, SImpl implTrue)
{
	SImpl impl = implNew(SIMPL_Cond, 2, cond, implTrue);

	return impl;
}

SImpl
implNewDefault(Syme syme)
{
	SImpl impl;
	
	impl = implNew(SIMPL_Default, 1, syme);

	return impl;
}

void
implSetConstNum(SImpl impl, int defId, int constId)
{
	Bool ret;

	ret = implSetConstNum0(impl, defId, constId);
	
	if (!ret)
		bug("Could not find impl");
}

local Bool 
implSetConstNum0(SImpl impl, int defId, int constId)
{
	Bool ret = false;
	switch (implTag(impl)) {
	  case SIMPL_Local:
		if (defId == -1 || 
		    impl->implLocal.defineIdx == defId) {
			impl->implLocal.constNum = constId;
			ret = true;
		}
		break;
	  case SIMPL_Branch:
		ret = implSetConstNum0(impl->implBranch.implTrue, 
				      defId, constId);
		if (!ret)
			ret = implSetConstNum0(impl->implBranch.implFalse, 
					      defId, constId);
		break;
	  case SIMPL_Cond:
		ret = implSetConstNum0(impl->implCond.impl, defId, constId);
		break;
	  case SIMPL_Default:
		break;
	  case SIMPL_Inherit:
		break;
	  default:
		bug("strange SImpl");
		break;
	}
	return ret;
}

int
implFindDefIdx(SImpl impl, int constId)
{
	Bool ret;

	ret = implFindDefIdx0(impl, constId);
	
	if (ret == -1)
		bug("Could not find impl");

	return ret;
}

local int
implFindDefIdx0(SImpl impl, int constId)
{
	Bool ret = -1;
	switch (implTag(impl)) {
	  case SIMPL_Local:
		    if (impl->implLocal.constNum == constId) 
			ret = impl->implLocal.defineIdx;
		break;
	  case SIMPL_Branch:
		ret = implFindDefIdx0(impl->implBranch.implTrue, constId);
		if (!ret)
			ret = implFindDefIdx0(impl->implBranch.implFalse, 
					      constId);
		break;
	  case SIMPL_Cond:
		ret = implFindDefIdx0(impl->implCond.impl, constId);
		break;
	  case SIMPL_Default:
		break;
	  case SIMPL_Inherit:
		break;
	  default:
		bug("strange SImpl");
		break;
	}
	return ret;
}

SImpl
implEvaluate(SImpl impl, AbLogic cond)
{
	
	SImpl newImpl = NULL;

	if (ablogIsTrue(cond))
		return impl;

	if (!impl)
		return impl;

	implDEBUG(printf("(ImplEvaluate:\n");
		  implPrintDb(impl);
		  ablogPrintDb(cond);
		  );

	switch (implTag(impl)) {
	  case SIMPL_Local:
	  case SIMPL_Default:
	  case SIMPL_None:
	  case SIMPL_Inherit:
	  case SIMPL_Import:
		newImpl = impl;
		break;
	  case SIMPL_Branch:
		if (ablogImplies(cond, impl->implBranch.cond))
			newImpl = implEvaluate(impl->implBranch.implTrue, cond);
		else if (ablogImplies(cond, ablogNot(impl->implBranch.cond)))
			newImpl = implEvaluate(impl->implBranch.implFalse, cond);
		else 
			newImpl = impl;
		break;
	  case SIMPL_Cond:
		if (ablogImplies(cond, impl->implCond.cond))
			newImpl = implEvaluate(impl->implCond.impl, cond);
		else
			newImpl = implNone();
		break;
	  default:
		bug("implEvaluate: not good");
		return impl;
	}
	implDEBUG(
		  implPrintDb(newImpl);
		  printf(")\n");
		  );
	return newImpl;
}

void
implFree(SImpl impl)
{
	/* FIXME: Should either copy or reference count */
	/*stoFree(impl); */
}

int
implPrintDb(SImpl impl)
{
	int cc = implPrint(dbOut, impl);
	fnewline(dbOut);

	return cc+1;
}

int
implPrint(FILE *file, SImpl impl)
{
	int cc = 0;

	if (impl == NULL)
		return fprintf(file, "(Impl: <null>)");
	cc += fprintf(file, "(Impl %s: ", implName(impl));
	switch (implTag(impl)) {
	  case SIMPL_None:
	  case SIMPL_Unknown:
		break;
	  case SIMPL_Inherit:
		cc += tfPrint(file, impl->implInherit.base);
		break;
	  case SIMPL_Default:
		cc += symePrint(file, impl->implDefault.def);
		break;
	  case SIMPL_Cond:
		cc += ablogPrint(file, impl->implBranch.cond);
		cc += implPrint(file, impl->implBranch.implTrue);
		break;
	  case SIMPL_Branch:
		cc += ablogPrint(file, impl->implBranch.cond);
		cc += implPrint(file, impl->implBranch.implTrue);
		cc += implPrint(file, impl->implBranch.implFalse);
		break;
	  case SIMPL_Local:
		cc += fprintf(file, "%d (%s.%d)",
			      (int)impl->implLocal.defineIdx, 
			      impl->implLocal.constLib 
			      	? libGetFileId(impl->implLocal.constLib)
			      	: "Local",
			      (int)impl->implLocal.constNum);

	  case SIMPL_Import:
		break;
	  default:
		printf("Aaarghh: %d", implTag(impl));
		break;
	}
	cc += printf(")");
	return cc;
}

struct SImplInfo implInfo[] = {
	{ SIMPL_None, 	   "None" },
	{ SIMPL_Unknown,   "Unknown" },
	{ SIMPL_Inherit,   "Inherit" },
	{ SIMPL_Cond,	   "Condition" },
	{ SIMPL_Branch,    "Branch" },
	{ SIMPL_Default,   "Default" },
	{ SIMPL_Local, 	   "Local" },
	{ SIMPL_Import,	   "Import"}
	};

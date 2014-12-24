#include "absub.h"
#include "axlobs.h"
#include "debug.h"
#include "list.h"
#include "sefo.h"
#include "stab.h"
#include "store.h"
#include "tform.h"
#include "tfsat.h"
#include "ti_top.h"
#include "utform.h"
#include "util.h"
#include "utype.h"
#include "uvar.h"

CREATE_LIST(UTForm);

UTForm utfUnknown;

void
utformInit()
{
	static Bool isInit = false;

	if (isInit)
		return;

	tfInit();

	isInit = true;
	utfUnknown = utformNewConstant(tfUnknown);
}




UTForm
utformNew(SymeList freevars, TForm tform)
{
	if (freevars == listNil(Syme)) {
		return utformNewUVar(uvarConstant(), tform);
	}
	return utformNewUVar(uvarNew(freevars, listNil(Sefo)), tform);
}

UTForm
utformNewWithCond(SymeList freevars, TForm tform, SefoList cond)
{
	if (freevars == listNil(Syme) && cond == listNil(Sefo)) {
		return utformNewUVar(uvarConstant(), tform);
	}
	return utformNewUVar(uvarNew(freevars, cond), tform);
}

UTForm
utformNewUVar(UVar uvar, TForm tf)
{
	UTForm utform = (UTForm) stoAlloc(OB_Other, sizeof(*utform));
	utform->tf = tf;
	utform->uvar = uvar;
	return utform;
}

void
utformFree(UTForm utform)
{
	tfFree(utform->tf);
	uvarUnref(utform->uvar);
}

TForm
utformConstOrFail(UTForm utform)
{
	if (utfIsConstant(utform))
		return utform->tf;
	else
		bug("Expected a constant type form here");
	return 0;
}

UTForm
utformNewConstant(TForm tf)
{
	return utformNewUVar(uvarConstant(), tf);
}

TForm
utformTForm(UTForm utf)
{
	return utf->tf;
}

Bool
utfHasVar(UTForm utf, Syme syme)
{
	return listMember(Syme)(utformVars(utf), syme, symeEqual);
}

Stab
utfGetStab(UTForm utf)
{
	return tfStab(utformTForm(utf));
}

UVar
utformUVar(UTForm utf)
{
	return uvarRef(utf->uvar);
}

SymeList
utformVars(UTForm utf)
{
	return uvarVars(utf->uvar);
}

SefoList
utformCondition(UTForm utf)
{
	return uvarConditions(utf->uvar);
}

UTForm
utformFollowOnly(UTForm utf)
{
	TForm tf = utf->tf;
	tf = tfFollowOnly(tf);
	if (tf != utf->tf)
		return utformNewUVar(uvarRef(utf->uvar), tf);
	return utf;
}

UTForm
utformFollow(UTForm utf)
{
	TForm tf = utf->tf;
	tfFollow(tf);
	utf->tf = tf;
	return utf;
}

Bool
utfIsUnknown(UTForm utf)
{
	return tfIsUnknown(utf->tf);
}

Bool
utfIsConstant(UTForm utf)
{
	return utformVars(utf) == listNil(Syme);
}

Bool
utfIsAny(UTForm utf)
{
	Syme syme;

	if (utfIsConstant(utf))
		return false;
	if (!tfIsId(utformTForm(utf)))
		return false;

	syme = tfIdSyme(utformTForm(utf));
	return listMemq(Syme)(utformVars(utf), syme);
}

UTForm
utfDefineeType(UTForm utf)
{
	TForm tf = tfDefineeType(utformTForm(utf));
	if (tf == utformTForm(utf))
		return utf;
	else
		return utformNewUVar(uvarRef(utf->uvar),
				     tfDefineeType(utformTForm(utf)));
}

Syme
utfDefineeSyme(UTForm utf)
{
	return tfDefineeSyme(utformTForm(utf));
}

Bool
utformIsAnyMap(UTForm utf)
{
	if (tfIsAnyMap(utformTForm(utf)))
		return true;

	if (!tfIsId(utformTForm(utf)))
		return false;
	/* "Ax, x" could be a map, but let's not worry about that yet.
	* Plan is to replace "x" with "Ax,y X -> Y" and "Ax,y X ->* Y" */
	return false;
}

UTForm
utfMapRet(UTForm utf)
{
	return utformNewUVar(utformUVar(utf),
			     tfMapRet(utformTForm(utf)));
}

UTForm
utfMapArg(UTForm utf)
{
	return utformNewUVar(utformUVar(utf),
			     tfMapArg(utformTForm(utf)));
}

Bool
utfIsExit(UTForm utf)
{
	return tfIsExit(utformTForm(utf));
}

Bool
utfIsRaw(UTForm utf)
{
	return tfIsRaw(utformTForm(utf));
}

UTForm
utfRawArg(UTForm utf)
{
	return utformNewUVar(utformUVar(utf), tfRawArg(utformTForm(utf)));
}

UTForm
utfRawType(UTForm utf)
{
	return utformNewUVar(utformUVar(utf), tfRawType(utformTForm(utf)));
}

Bool
utfIsTuple(UTForm utf)
{
	return tfIsTuple(utformTForm(utf));
}

UTForm
utfTupleArg(UTForm utf)
{
	return utformNewUVar(utformUVar(utf),
			     tfTupleArg(tfFollowFn(utformTForm(utf))));
}

Bool
utfIsMulti(UTForm utf)
{
	return tfIsMulti(utformTForm(utf));
}

Bool
utfIsCross(UTForm utf)
{
	return tfIsCross(utformTForm(utf));
}


UTForm
utfArgN(UTForm utf, Length i)
{
	return utformNewUVar(utformUVar(utf),
			     tfArgv(tfFollowFn(utformTForm(utf)))[i]);
}

Length
utfArgc(UTForm utf)
{
	return tfArgc(tfFollowFn(utformTForm(utf)));
}

Length
utfMultiArgc(UTForm utf)
{
	return tfMultiArgc(utformTForm(utf));
}

UTForm
utfMultiArgN(UTForm utf, Length n)
{
	return utformNewUVar(utformUVar(utf),
			     tfMultiArgN(utformTForm(utf), n));
}

Bool
utfIsEmptyMulti(UTForm utf)
{
	return tfIsEmptyMulti(utformTForm(utf));
}


UTForm
utfMultiFrList(UTFormList utfl)
{
	//  (Vx: F(x), Vy: G(y), Vx: H(x)) --> (Vx,y,z (Fx, Gx, Hz))
	SymeList origVars = listNil(Syme);
	SymeList newVars  = listNil(Syme);
	TFormList tfl = listNil(TForm);
	SefoList newCond = listNil(Sefo);
	listIter(UTForm, utf, utfl, {
			AbSub sigma = NULL;
			SefoList condition = utformCondition(utf);
			listIter(Syme, var, utformVars(utf), {
					if (listMember(Syme)(origVars, var, symeEqual)) {
						sigma = sigma == NULL ? absNew(stabFile()): sigma;
						Syme copied = symeNewLexConst(symGen(), symeType(var), car(stabFile()));
						AbSyn expr = abFrSyme(copied);
						tiTopFns()->tiSefo(stabFile(), expr);
						absExtend(var, expr, sigma);
						newVars = listCons(Syme)(copied, newVars);
					}
					else {
						newVars = listCons(Syme)(var, newVars);
						origVars = listCons(Syme)(var, origVars);
					}
				});
			if (sigma != NULL) {
				utf = utformSubst(sigma, utf);
				if (condition != listNil(Sefo))
					bug("Need to deal with conditions");
			}
			tfl = listCons(TForm)(utformTForm(utf), tfl);
			newCond = listConcat(Sefo)(newCond, condition);
		});

	return utformNewUVar(uvarNew(newVars, newCond), tfMultiFrList(listNReverse(TForm)(tfl)));
}


Length
utfCrossArgc(UTForm utf)
{
	return tfCrossArgc(utformTForm(utf));
}

UTForm
utfCrossArgN(UTForm utf, Length n)
{
	return utformNewUVar(utformUVar(utf),
			     tfCrossArgN(utformTForm(utf), n));
}

Bool
utfIsDeclare(UTForm utf)
{
	return tfIsDeclare(utformTForm(utf));
}

Bool
utfIsAnyMap(UTForm utf)
{
	return tfIsAnyMap(utformTForm(utf));
}

Bool
utfIsPackedMap(UTForm utf)
{
	return tfIsPackedMap(utformTForm(utf));
}

Bool
utfIsDefine(UTForm utf)
{
	return tfIsDefine(utformTForm(utf));
}

UTForm
utfDefineVal(UTForm utf)
{
	// Maybe this should return a UType
	return utformNewUVar(utformUVar(utf),
			     tfDefineVal(utformTForm(utf)));
}

SatMask
utfSatType(UTForm S)
{
	// Note that if S is free, then we could add a "this is a type" constraint
	return tfSatType(utformTForm(S));
}

Bool
utformEqual(UTForm utf1, UTForm utf2)
{
	return tfEqual(utformConstOrFail(utf1),
		       utformConstOrFail(utf2));
}

UTForm
utformSubst(AbSub sigma, UTForm utf)
{
	SymeList newVars = listNil(Syme);
	SymeList vars = utformVars(utf);
	if (absIsEmpty(sigma)) {
		return utf;
	}

	while (vars != listNil(Syme)) {
		if (absLookup(car(vars), NULL, sigma) == NULL)
			newVars = listCons(Syme)(car(vars), newVars);
		vars = cdr(vars);
	}
	return utformNewUVar(uvarNew(newVars, sefoListSubst(sigma, utformCondition(utf))),
			     tformSubst(sigma, utformTForm(utf)));
}

local AbEmbed utfAsMultiEmbedTuple(UTForm utf);
local AbEmbed utfAsMultiEmbedCross(UTForm utf, Length argc);

AbEmbed
utfAsMultiEmbed(UTForm utf, Length argc)
{
	/*
	UTForm	ntf;
	AbEmbed result = AB_Embed_Fail;
	ntf = utfDefineeType(utf);
	utf  = utformFollowOnly(utf);

	// can we embed the given number of args into utf
	if (utfIsMulti(ntf) &&
	    (utfMultiArgc(ntf) == argc ||
	     (utfMultiArgc(ntf) > argc && utfMultiHasDefaults(ntf))))
		result |= AB_Embed_Identity;

	if (argc == 1)
		result |= AB_Embed_Identity;

	result |= utfAsMultiEmbedTuple(utf);

	result |= utfAsMultiEmbedCross(utf, argc);

	if (argc == 0 && utfIsDefine(utf))
		result |= AB_Embed_Identity;

	return result;
	*/
	return tfAsMultiEmbed(utformTForm(utf), argc);
}

local AbEmbed
utfAsMultiEmbedTuple(UTForm utf)
{
	AbEmbed result = AB_Embed_Fail;

	if (utfIsTuple(utf))
		result |= AB_Embed_ApplyMultiToTuple;
	else if (utfIsAny(utf))
		result |= AB_Embed_ApplyMultiToTuple;

	return result;
}

local AbEmbed
utfAsMultiEmbedCross(UTForm utf, Length argc)
{
	AbEmbed result = AB_Embed_Fail;

	if (utfIsCross(utf) && utfCrossArgc(utf) == argc)
		result |= AB_Embed_ApplyMultiToCross;
	else if (utfIsAny(utf))
		result |= AB_Embed_ApplyMultiToCross;

	return result;
}


UTForm
utfAsMultiArgN(UTForm utf, Length argc, Length n)
{
	return utformNewUVar(utformUVar(utf),
			     tfAsMultiArgN(utformTForm(utf), argc, n));
}

Length
utfAsMultiArgc(UTForm utf)
{
	return tfAsMultiArgc(utformTForm(utf));
}

Bool
utfMultiHasDefaults(UTForm T)
{
	return tfMultiHasDefaults(utformTForm(T));
}


AbSyn
utfAsMultiSelectArg(AbSyn ab, Length argc, Length n, AbSynGetter argf,
		    UTForm utfi, Bool * def, Length * pos)
{
	return tfAsMultiSelectArg(ab, argc, n, argf, utformTForm(utfi), def, pos);
}

Bool
utfIsPending(UTForm utf)
{
	return tfIsPending(utformTForm(utf));
}

UType
utfExpr(UTForm utf)
{
	return utypeNewUVar(utformUVar(utf), tfExpr(utformTForm(utf)));
}

UTForm
utformFromType(UType type)
{
	return utformNewUVar(utypeUVar(type),
			     tfFullFrAbSyn(stabFile(), utypeSefo(type)));
}

void
utfPrintDb(UTForm utf)
{
	utfPrint(dbOut, utf);
	fnewline(dbOut);
}

int
utfPrint(FILE *fout, UTForm utf)
{
	struct ostream os;
	int cc;

	ostreamInitFrFile(&os, fout);
	cc = utfOStreamWrite(&os, false, utf);
	ostreamClose(&os);

	return cc;
}

int
utfOStreamWrite(OStream ostream, Bool deep, UTForm utf)
{
       int c;

       if (utformCondition(utf)) {
               return ostreamPrintf(ostream,
				    deep
				    ? "<ForAll: %pSymeList %pAbSynList %#pTForm>"
				    : "<ForAll: %pSymeList %pAbSynList %pTForm>",
                                    utformVars(utf), utformCondition(utf), utformTForm(utf));
       }
       else {
               return ostreamPrintf(ostream,
				    deep ? "<ForAll: %pSymeList %#pTForm>" : "<ForAll: %pSymeList %pTForm>",
                                    utformVars(utf), utformTForm(utf));
       }

       return c;

}


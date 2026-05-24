#include "ablogic.h"
#include "absub.h"
#include "absyn.h"
#include "debug.h"
#include "format.h"
#include "stab.h"
#include "sefo.h"
#include "store.h"
#include "tform.h"
#include "tfsat.h"
#include "ti_top.h"
#include "utform.h"
#include "util.h"
#include "unify.h"
#include "utype.h"
#include "utyperes.h"
#include "uvar.h"

Bool utypeDebug = false;
#define utypeDEBUG			DEBUG_IF(utype)		afprintf

/*
 import from List(?T: with);

 list(list(2)) -->
    2 --> Integer
    list --> ? -> List ?
    list --> ? -> List ?

    unify(?, Integer) --> ? == Integer

  MapOperations(T: with, L: ListType T): with
      map: (L, T -> T) -> L

  MapOperations2(T: with, X: with, L: ListType T, M: ListType X): with
      map: (L, T -> X) -> M

  map([1,2,3], f)

  f:  Integer -> Integer
  [1,2,3]: List Integer

  map: [T, L] (L, T->T) -> L

  map([1,2,3], f)
    ->> [T, L] (L, T -> T)... (List Integer, Integer -> Integer)
    ->> (List Integer, Integer -> Integer)
    : List Integer

*/


/* Local */
local UTypeResult utypeResultExtend(UTypeResult result, UTypeResult resultToAdd, Syme symeToAdd, Sefo sefoToAdd);
local UTypeResult utypeUnifyResult(UTypeResult result1, UTypeResult result2, Syme syme, Sefo sefo1, Sefo sefo2);
local UTypeResult utypeUnifySefo(SymeList sl1, SymeList sl2, Sefo sefo1, Sefo sefo2);
local UTypeResult utypeUnifyId(SymeList sl1, SymeList sl2, Syme syme1, Sefo sefo2);

local Bool        utypeResultValidate(UTypeResult result);
local UTypeResult utypeResultMergeInner(SymeList vars1, SymeList vars2, UTypeResult res1, UTypeResult res2);
local UTypeResult utypeResultMergeSubsts(UTypeResult res, SymeList vars1, SymeList vars2, UTypeSubstList substs);

local SefoList    utypeResultMergeConditions(UTypeResult res, SefoList ut1x, SefoList ut2x);
local UTypeResult utypeResultCheckCondition(UTForm ut1, UTForm ut2, UTypeResult result);
local Bool        utypeResultCheckConditionParts(SymeList vars, UTypeResult result);

Bool
utformCanUnify(UTForm ut1, UTForm ut2)
{
	UTypeResult res = utformUnify(ut1, ut2);
	Bool ret;
	ret = !utypeResultIsFail(res);
	utypeResultFree(res);

	return ret;
}

local UTypeResult utformUnifyInner(UTForm ut1, UTForm ut2, TForm tf1, TForm tf2);

UTypeResult
utformUnify(UTForm ut1, UTForm ut2)
{
	UTypeResult res;
	if (utfIsConstant(ut1) && utfIsConstant(ut2)) {
		Bool eq = tfEqual(utformTForm(ut1), utformTForm(ut2));
		return eq ? utypeResultEmpty() : utypeResultFailed();
	}
	res = utformUnifyInner(ut1, ut2, utformTForm(ut1), utformTForm(ut2));

	if (res != NULL) {
		SefoList merged = utypeResultMergeConditions(res, utformCondition(ut1), utformCondition(ut2));
		res->conditions = merged;
	}

	assert(utypeResultValidate(res));
	res = utypeResultCheckCondition(ut1, ut2, res);
	return res;
}

static int ucount=0;
local UTypeResult
utformUnifyInner(UTForm ut1, UTForm ut2, TForm tf1, TForm tf2)
{
	UTypeResult result;
	int i;
	int serial = ucount++;

	tfFollow(tf1);
	tfFollow(tf2);

	utypeDEBUG(dbOut, "(Unify %d: %pSymeList %pTForm %pSymeList %pTForm\n",
		   serial, utformVars(ut1), tf1,
		   utformVars(ut2), tf2);

	if (!(tfIsId(tf1) && utfHasVar(ut1, tfIdSyme(tf1)))
	    && tfIsId(tf2) && utfHasVar(ut2, tfIdSyme(tf2)))
		result = utformUnifyInner(ut2, ut1, tf2, tf1);
	else if (tfIsId(tf1) && utfHasVar(ut1, tfIdSyme(tf1))) {
		AbSyn expr = tfExpr(tf2);
		if (tiTopFns()->tiCanSefo(expr)) {
			tiTopFns()->tiSefo(stabFile(), expr);
		}
		result = utypeUnifyId(utformVars(ut1), utformVars(ut2), tfIdSyme(tf1), expr);
	}
	else if (tfTag(tf1) != tfTag(tf2)) {
		utypeDEBUG(dbOut, "Unify %d: Failed (tag)\n", serial);
		result = utypeResultFailed();
	}

	else if (tfIsGeneral(tf1) && tfIsGeneral(tf2)) {
		result = utypeUnifySefo(utformVars(ut1), utformVars(ut2), tfExpr(tf1), tfExpr(tf2));
	}
	else if (tf1->argc != tf2->argc) {
		utypeDEBUG(dbOut, "Unify %d: Failed argc\n", serial);
		result = utypeResultFailed();
	}
	else {
		result = utypeResultEmpty();
		for (i=0; i < tf1->argc && !utypeResultIsFail(result); i++) {
			UTypeResult utrI = utformUnifyInner(ut1, ut2, tfArgv(tf1)[i], tfArgv(tf2)[i]);
			result = utypeResultMergeInner(utformVars(ut1), utformVars(ut2), result, utrI);
		}
	}
	utypeDEBUG(dbOut, "Unify %d: %pUTypeResult)\n", serial, result);
	return result;
}


UTypeResult
utypeUnify(UType ut1, UType ut2)
{
	UTypeResult res = utypeUnifySefo(utypeVars(ut1), utypeVars(ut2), ut1->sefo, ut2->sefo);
	assert(utypeResultValidate(res));
	return res;
}

local Bool
utypeSubstMatchSyme(UTypeSubst subst, void *p)
{
	return subst->syme == p;
}

UTypeResult
utypeResultMerge(UTypeResult res1, UTypeResult res2)
{
	return utypeResultMergeInner(listNil(Syme), listNil(Syme), res1, res2);
}

local UTypeResult
utypeResultMergeInner(SymeList vars1, SymeList vars2, UTypeResult res1, UTypeResult res2)
{
	if (utypeResultIsFail(res1) || utypeResultIsFail(res2))
		return utypeResultFailed();
	if (utypeResultIsEmpty(res1))
		return res2;
	if (utypeResultIsEmpty(res2))
		return res1;

	SefoList conds1 = res1->conditions;
	SefoList conds2 = res2->conditions;

	res1 = utypeResultMergeSubsts(res1, vars1, listConcat(Syme)(vars2, res2->news), res2->substs);

       SefoList conditions = utypeResultMergeConditions(res1, conds1, conds2);

       if (utypeDebug && (conds1 != listNil(Sefo) || conds2 != listNil(Sefo))) {
               afprintf(dbOut, "Merge Conditions: %pAbSynList %pAbSynList\n ---> %pAbSynList", conds1, conds2, conditions);
       }
       res2->conditions = conditions;
       assert(utypeResultValidate(res1));

       return res1;
}


local SefoList
utypeResultMergeConditions(UTypeResult res, SefoList ut1x, SefoList ut2x)
{
	SefoList result = listNil(Sefo);
	AbSub    sigma;

	utypeDEBUG(dbOut, "(Merge conditions %pUTypeResult %pAbSynList %pAbSynList\n",
		   res, ut1x, ut2x);
	if (ut1x == listNil(Sefo) && ut2x == listNil(Sefo)) {
		return listNil(Sefo);
	}

	sigma = utypeResultSigma(res);

	while (ut1x != listNil(Sefo)) {
		Sefo sefo = car(ut1x);
		ut1x = cdr(ut1x);
		Sefo subst = sefoSubst(sigma, sefo);
		if (!listMemq(Sefo)(result, subst) && !listMember(Sefo)(result, subst, sefoEqual)) {
			result = listCons(Sefo)(subst, result);
		}
	}
	while (ut2x != listNil(Sefo)) {
		Sefo sefo = car(ut2x);
		ut2x = cdr(ut2x);
		Sefo subst = sefoSubst(sigma, sefo);
		utypeDEBUG(dbOut, "Merge conditions %pAbSyn --> %pAbSyn)\n", sefo, subst);
		if (!listMemq(Sefo)(result, subst) && !listMember(Sefo)(result, subst, sefoEqual)) {
			result = listCons(Sefo)(subst, result);
		}
	}
	utypeDEBUG(dbOut, "Merge conditions ---> %pAbSynList)\n",
		   result);

	return result;
}


/* Vars as the new variables introduced by the merge process */
local UTypeResult
utypeResultMergeSubsts(UTypeResult res, SymeList vars1, SymeList vars2, UTypeSubstList substs)
{
	static int count;
	Bool failed = false;
	int serialThis = count++;

	if (utypeDebug) {
		afprintf(dbOut, "(Merge Substs %d %pUTypeResult\n", serialThis, res);
		afprintf(dbOut, " Merge Substs %d %pSymeList\n", serialThis, vars1);
		afprintf(dbOut, " Merge Substs %d %pSymeList\n", serialThis, vars2);
	}

	while (substs != listNil(UTypeSubst) && !failed) {
		UTypeSubst subst2 = car(substs);
		UTypeSubst subst1 = utypeResultSubst(res, subst2->syme);
		substs = cdr(substs);

		if (utypeDebug) {
			afprintf(dbOut, " Merge Substs %d %pSyme %pSefo\n", serialThis,
				 subst2->syme, subst2->sefo);
		}
		if (subst1 != NULL) {
			UTypeResult newres = utypeUnifySefo(vars1, vars2, subst1->sefo, subst2->sefo);
			if (utypeResultIsFail(newres)) {
				failed = true;
			}
			else {
				vars2 = listConcat(Syme)(newres->news, vars2);
				substs = listConcat(UTypeSubst)(newres->substs, substs);
			}
		}
		else {
			SymeList freeVars = listConcat(Syme)(utypeResultSymes(res), vars2);
			UType newType = utypeResultApply(res, utypeNew(freeVars, subst2->sefo));
			UTypeSubstList oldSubsts = res->substs;
			// This is likely to get quadratic.. Should keep lists of free vars
			while (oldSubsts != listNil(UTypeSubst)) {
				UTypeSubst subst = car(oldSubsts);
				subst->sefo = utypeSubstApply(subst2, subst->sefo);
				oldSubsts = cdr(oldSubsts);
			}
			utypeResultAdd(res, subst2->syme, utypeSefo(newType));
		}
	}

	if (utypeDebug) {
		afprintf(dbOut, " Merge Substs %d %pUTypeResult)\n", serialThis, res);
	}

	return failed? utypeResultFailed() : res;
}


local UTypeResult
utypeUnifyResult(UTypeResult result1, UTypeResult result2, Syme syme, Sefo sefo1, Sefo sefo2)
{
	UTypeResult result;

	result = utypeUnifySefo(result1->news, result2->news, sefo1, sefo2);
	utypeResultAdd(result, syme, sefo2);
	return result;
}

local UTypeResult
utypeUnifySefo(SymeList sl1, SymeList sl2, Sefo sefo1, Sefo sefo2)
{
	UTypeResult result;
	int i;
	int serial = ucount++;

	utypeDEBUG(dbOut, "(Unify %d: %pSymeList %pSefo %pSymeList %pSefo\n", serial, sl1, sefo1, sl2, sefo2);

	if (!(abIsId(sefo1) && listMemq(Syme)(sl1, abSyme(sefo1)))
	    && abIsId(sefo2) && listMemq(Syme)(sl2, abSyme(sefo2)))
		result = utypeUnifySefo(sl2, sl1, sefo2, sefo1);
	else if (abIsId(sefo1) && listMemq(Syme)(sl1, abSyme(sefo1))) {
		result = utypeUnifyId(sl1, sl2, abSyme(sefo1), sefo2);
	}
	else if (abIsId(sefo1)) {
		if (sefoEqual(sefo1, sefo2))
			result = utypeResultEmpty();
		else
			result = utypeResultFailed();
	}
	else if (abTag(sefo1) != abTag(sefo2)) {
		result = utypeResultFailed();
	}
	else if (abArgc(sefo1) != abArgc(sefo2)) {
	        result = utypeResultFailed();
	}
	else {
		result = utypeResultEmpty();
		for (i=0; !utypeResultIsFail(result) && i < abArgc(sefo1); i++) {
			UTypeResult utrI = utypeUnifySefo(sl1, sl2, abArgv(sefo1)[i], abArgv(sefo2)[i]);
			result = utypeResultMergeInner(sl1, sl2, result, utrI);
		}
	}

	utypeDEBUG(dbOut, "Unify %d ---> %pUTypeResult)\n", serial, result);
	return result;
}

local UTypeResult
utypeUnifyId(SymeList sl1, SymeList sl2, Syme syme1, Sefo sefo2)
{
	assert(listMemq(Syme)(sl1, syme1));
	if (symeEqual(syme1, abSyme(sefo2))) {
		return utypeResultEmpty();
	}
	if (abIsId(sefo2) && listMemq(Syme)(sl2, abSyme(sefo2))) {
		Syme syme3 = symeClone(syme1);
		afprintf(dbOut, "UnifyId: %pSyme %pSefo\n", syme1, sefo2);
		return utypeResultTwo(syme1, abFrSyme(syme3),
				      abSyme(sefo2), abFrSyme(syme3), syme3);
	}
	else {
		int pos;
		listFind(Syme)(sefoSymes(sefo2), syme1, symeEqual, &pos);
		if (pos != -1) {
			return utypeResultFailed();
		}
		else {
			return utypeResultOne(syme1, sefo2);
		}
	}
}

UType
utypeResultApply(UTypeResult result, UType utype)
{
	SymeList freevars;
	AbSub    sigma;
	SymeList symes;

	assert(!utypeResultIsFail(result));

	if (utypeIsConstant(utype))
		return utype;

	sigma = utypeResultSigma(result);

	freevars = listCopy(Syme)(result->news);
	symes = utypeVars(utype);

	while (symes != listNil(Syme)) {
		if (utypeResultSubst(result, car(symes)) != NULL)
			freevars = listCons(Syme)(car(symes), freevars);
		symes = cdr(symes);
	}

	return utypeNew(freevars, sefoSubst(sigma, utypeSefo(utype)));
}

UTForm
utypeResultApplyTForm(UTypeResult result, UTForm utf)
{
	SymeList freevars;
	AbSub    sigma;
	SymeList symes;
	SefoList condition;

	assert(!utypeResultIsFail(result));

	if (utfIsConstant(utf))
		return utf;

	sigma = utypeResultSigma(result);

	freevars = listCopy(Syme)(result->news);
	symes = utformVars(utf);

	while (symes != listNil(Syme)) {
		Syme syme = car(symes);
		symes = cdr(symes);
		if (utypeResultSubst(result, syme) != NULL) {
			continue;
		}
		freevars = listCons(Syme)(syme, freevars);
	}

       condition = sefoListSubst(sigma, utformCondition(utf));
       return utformNewUVar(uvarNew(freevars, condition),
                            tfSubst(sigma, utformTForm(utf)));
}

local UTypeResult
utypeResultCheckCondition(UTForm ut1, UTForm ut2, UTypeResult result)
{
	if (!utypeResultIsFail(result)) {
		SymeList symes = listConcat(Syme)(utformVars(ut1), listCopy(Syme)(utformVars(ut2)));
		Bool isKnown = utypeResultCheckConditionParts(symes, result);
		utypeDEBUG(dbOut, "Condition: %pUTypeResult\n", result);
		utypeDEBUG(dbOut, "Condition: %pSefoList --> %oBool\n", result->conditions, isKnown);
		if (!isKnown) {
			utypeResultFree(result);
			result = utypeResultFailed();
		}
		listFree(Syme)(symes);
	}
	return result;
}

local Bool
utypeResultCheckConditionParts(SymeList vars, UTypeResult result)
{
	SefoList conditions, tmp;
	Bool ok = true;

	tmp = listCons(Sefo)(NULL, listNil(Sefo));

	for (conditions = result->conditions; conditions && ok; conditions = cdr(conditions)) {
		Sefo     cond  = car(conditions);
		SymeList sefoVars = sefoSymes(cond);
		SatMask mask;
		TForm tfdom, tfcat;
		AbSyn dom, cat;

		car(tmp) = cond;

		if (listContainsAnyq(Syme)(sefoVars, vars)) {
			utypeDEBUG(dbOut, "Check condition %pAbSyn -> Contains var\n", cond);
			continue;
		}

		utypeDEBUG(dbOut, "Check condition %pAbSynList %oBool\n", tmp, ablogIsListKnown(tmp));
		if (ablogIsListKnown(tmp))
			continue;

		if (abTag(cond) != AB_Has)
			continue;

		dom = cond->abHas.expr;
		cat = cond->abHas.property;

		tfdom = abGetCategory(dom);
		tfcat = abTForm(cat) ? abTForm(cat) : tiTopFns()->tiGetTopLevelTForm(ablogTrue(), cat);

		mask = tfSat(tfSatBupMask(), tfdom, tfcat);
		ok = tfSatPending(mask) || tfSatSucceed(mask);
	}
	return ok;
}

AbSub
utypeResultSigma(UTypeResult result)
{
	AbSub sigma;
	UTypeSubstList substs;

	sigma = absNew(stabFile());
	substs = result->substs;
	while (substs != listNil(UTypeSubst)) {
		Syme syme = car(substs)->syme;
		Sefo sefo = car(substs)->sefo;
		substs = cdr(substs);

		sigma = absExtend(syme, sefo, sigma);
	}

	return sigma;
}

local Bool
utypeResultValidate(UTypeResult result)
{
	UTypeSubstList substs;
	SymeList symes;

	if (result == NULL) {
		return true;
	}
	substs = result->substs;
	symes = utypeResultSymes(result);

	while (substs != listNil(UTypeSubst)) {
		Syme syme = car(substs)->syme;
		Sefo sefo = car(substs)->sefo;

		if (listContainsAny(Syme)(sefoSymes(sefo), symes, symeEqual)) {
			afprintf(dbOut, "Bad result: %pUTypeResult\n", result);
			return false;
		}
		SymeList rpts = listMatchAll(Syme)(symes, syme, (Bool(*)(Syme, void*)) symeEqual);
		if (listLength(Syme)(rpts) != 1) {
			afprintf(dbOut, "Bad result - Repeated id: %pUTypeResult\n", result);
			return false;
		}
		substs = cdr(substs);
	}

	listFree(Syme)(symes);
	return true;
}


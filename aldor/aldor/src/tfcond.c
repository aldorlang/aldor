#include "axlobs.h"
#include "debug.h"
#include "format.h"
#include "stab.h"
#include "store.h"
#include "tfcond.h"

CREATE_LIST(TfCondElt);

extern	Bool		tfDebug;
#define tfCondDEBUG	DEBUG_IF(tf)	afprintf

local TfCondElt tfCondEltNewFull(Stab stab, AbSynList absynList, AbLogic known);

TfCond
tfCondNew()
{
	TfCond tfcond = (TfCond) stoAlloc(OB_Other, sizeof(*tfcond));

	tfcond->conditions = listNil(TfCondElt);
	tfcond->containsEmpty = false;
	tfcond->known = ablogFalse();

	return tfcond;
}

void
tfCondFree(TfCond cond)
{
	stoFree(cond);
}


local TfCondElt
tfCondEltNewFull(Stab stab, AbSynList absynList, AbLogic known)
{
	TfCondElt tfcondElt = (TfCondElt) stoAlloc(OB_Other, sizeof(*tfcondElt));

	tfcondElt->stab = stab;
	tfcondElt->list = absynList;
	tfcondElt->known = ablogCopy(known);

	return tfcondElt;
}

TfCondElt
tfCondEltNew(Stab stab, AbSynList absynList)
{
	return tfCondEltNewFull(stab, absynList, ablogFalse());
}

TfCondElt
tfCondEltNewKnown(Stab stab, AbLogic known)
{
	return tfCondEltNewFull(stab, listNil(AbSyn), known);
}

void
tfCondEltFree(TfCondElt condElt)
{
	stoFree(condElt);
}


TfCond
tfCondFloat(Stab stab, TfCond tfcond)
{
	TfCond newTfCond;
	TfCondEltList conditionElts = tfcond->conditions;
	int floatDepth = stabLevelNo(stab);
	Bool containsEmpty = false;

	TfCondEltList filteredConditions = listNil(TfCondElt);
	
	tfCondDEBUG(dbOut, "tform depth: %d\n", floatDepth);
	while (conditionElts != listNil(TfCondElt) && !containsEmpty) {
		TfCondElt elt = car(conditionElts);
		AbSynList filteredCondition = listNil(AbSyn);
		AbSynList condition = elt->list;

		while (condition != listNil(AbSyn)) {
			ULong idepth = abOuterDepth(elt->stab, car(condition));
			tfCondDEBUG(dbOut, "ConditionDepth: %pAbSyn %d/%d\n",
				    car(condition), idepth, floatDepth);
			if (floatDepth >= idepth) {
				tfCondDEBUG(dbOut, "Keeping %pAbSyn\n", car(condition));
				filteredCondition = listCons(AbSyn)(car(condition), 
								    filteredCondition);
			}
			condition = cdr(condition);

		}
		tfCondDEBUG(dbOut, "Floating conditions - filtered: %pAbSynList\n", 
			    filteredCondition);
		if (filteredCondition == listNil(AbSyn)) {
			containsEmpty = true;
		}
		else {
			TfCondElt filteredConditionElt = tfCondEltNew(stab, filteredCondition);
			filteredConditions = listCons(TfCondElt)(filteredConditionElt, filteredConditions);
		}
		tfCondDEBUG(dbOut, "Floating conditions: %pAbSynList\n", 
			    car(conditionElts)->list);
		conditionElts = cdr(conditionElts);
	}

	
	newTfCond = tfCondNew();
	if (containsEmpty || filteredConditions == listNil(TfCondElt)) {
		newTfCond->containsEmpty = true;
		newTfCond->conditions = listNil(TfCondElt);
	}
	else {
		newTfCond->containsEmpty = false;
		newTfCond->conditions = filteredConditions;
	}


	return newTfCond;
}

TfCond
tfCondMerge(TfCond c1, Stab stab, TfCondElt condition)
{
	if (condition == NULL)
		return c1;

	if (c1 == NULL) {
		c1 = tfCondNew();
	}
	if (c1->containsEmpty) {
		return c1;
	}
	if (condition == NULL || ablogIsTrue(condition->known)) {
		c1->containsEmpty = true;
	}
	else if (condition->list != listNil(AbSyn)) {
		TfCondElt conditionElt = tfCondEltNew(stab, condition->list);
		c1->conditions = listCons(TfCondElt)(conditionElt, c1->conditions);
	}

	c1->known = ablogOr(c1->known, condition->known);

	return c1;
}

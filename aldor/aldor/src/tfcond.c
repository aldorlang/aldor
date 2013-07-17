#include "axlobs.h"
#include "store.h"
#include "tfcond.h"

CREATE_LIST(TfCondElt);

extern Bool tfDebug;
#define	tfCondDEBUG(s)		DEBUG_IF(tfDebug, s)

TfCond
tfCondNew()
{
	TfCond tfcond = (TfCond) stoAlloc(OB_Other, sizeof(*tfcond));

	tfcond->conditions = NULL;
	tfcond->containsEmpty = false;

	return tfcond;
}

void
tfCondFree(TfCond cond)
{
	stoFree(cond);
}


TfCondElt
tfCondEltNew(Stab stab, AbSynList absynList)
{
	TfCondElt tfcondElt = (TfCondElt) stoAlloc(OB_Other, sizeof(*tfcondElt));

	tfcondElt->stab = stab;
	tfcondElt->list = absynList;

	return tfcondElt;
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
	
	tfCondDEBUG(afprintf(dbOut, "tform depth: %d\n", floatDepth));
	while (conditionElts != listNil(TfCondElt) && !containsEmpty) {
		TfCondElt elt = car(conditionElts);
		AbSynList filteredCondition = listNil(AbSyn);
		AbSynList condition = elt->list;

		while (condition != listNil(AbSyn)) {
			ULong idepth = abOuterDepth(elt->stab, car(condition));
			tfCondDEBUG(afprintf(dbOut, "ConditionDepth: %pAbSyn %d/%d\n", 
					     car(condition), idepth, floatDepth));
			if (floatDepth >= idepth) {
				tfCondDEBUG(afprintf(dbOut, "Keeping %pAbSyn\n", car(condition)));
				filteredCondition = listCons(AbSyn)(car(condition), 
								    filteredCondition);
			}
			condition = cdr(condition);

		}
		tfCondDEBUG(afprintf(dbOut, "Floating conditions - filtered: %pAbSynList\n", 
				     filteredCondition));
		if (filteredCondition == listNil(AbSyn)) {
			containsEmpty = true;
		}
		else {
			TfCondElt filteredConditionElt = tfCondEltNew(stab, filteredCondition);
			filteredConditions = listCons(TfCondElt)(filteredConditionElt, filteredConditions);
		}
		tfCondDEBUG(afprintf(dbOut, "Floating conditions: %pAbSynList\n", 
				    car(conditionElts)->list));
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
	Buffer buf = bufNew();
	if (condition == NULL)
		return c1;

	if (c1 == NULL) {
		c1 = tfCondNew();
	}
	if (c1->containsEmpty) {
		return c1;
	}
	if (condition == NULL) {
		c1->containsEmpty = true;
	}
	else {
		TfCondElt conditionElt = tfCondEltNew(stab, condition->list);
		c1->conditions = listCons(TfCondElt)(conditionElt, c1->conditions);
	}
	return c1;
}

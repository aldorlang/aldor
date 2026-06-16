#ifndef UTYPE_H
#define UTYPE_H
#include "axlobs.h"
/**
 * Universally quantified types
 */

// UNIFY

UTForm		utformSubst	(AbSub sigma, UTForm utf);
UTypeResult	utformUnify	(UTForm ut1, UTForm ut2);
Bool		utformCanUnify	(UTForm ut1, UTForm ut2);
UTypeResult	utypeUnify	(UType utype1, UType utype2);

UType		utypeResultApply	(UTypeResult res, UType utype);
UTForm		utypeResultApplyTForm	(UTypeResult res, UTForm tf);
UTypeResult	utypeResultApplyResult	(UTypeResult result, UTypeResult extra);

UTypeResult	utypeResultMerge	(UTypeResult res1, UTypeResult res2);

#endif

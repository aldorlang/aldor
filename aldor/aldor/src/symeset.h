#ifndef _SYMESET_H_
#define _SYMESET_H_
#include "ttable.h"
#include "symbol.h"
#include "syme.h"

/* A SymeSet contains a collection of symes - we hold the names
 * separately as these can be used to determine quickly if a set does
 * not contain a syme of the given name
 */

typedef struct SymeSet {
	SymbolTSet names;
	SymeList symes;
} *SymeSet;

extern SymeSet 		symeSetFrSymes	(SymeList symes);
extern void    		symeSetFree	(SymeSet symeSet);

extern Bool		symeSetIsEmpty	(SymeSet symeSet);
extern Bool		symeSetMember	(SymeSet symeSet, Syme syme);
extern Bool		symeSetMayHave	(SymeSet symeSet, Symbol symbol);
extern SymbolTSet	symeSetNames	(SymeSet symeSet);
extern SymeSet 		symeSetUnion	(SymeSet symeSet1, SymeSet symeSet2);
extern SymeList 	symeSetList	(SymeSet symeSet);

extern SymeList		symeSetSymesForSymbol(SymeSet symeSet, Symbol symbol);

extern int 		symeSetFormat	(OStream ostream, SymeSet symeSet);

#endif

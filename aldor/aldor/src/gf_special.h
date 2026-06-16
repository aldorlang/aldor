#ifndef GF_SPECIAL_H
#define GF_SPECIAL_H

#include "foam.h"

extern Bool gen0IsEnumLit(Syme syme);
extern AInt gen0EnumIndex(TForm whole, Symbol sym); // NB: Maybe pull callers in

extern Foam gen0SpecialOp(FoamTag type, Syme syme, Length argc,
			  AbSyn *argv, Foam *vals);

extern TForm gen0SpecialKeyType(TForm tf);

extern void gfm0SpecialMatch(Syme syme, AbSyn ab, Foam in);

#endif

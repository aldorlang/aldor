#ifndef _FEX_H
#define _FEX_H
#include "axlobs.h"

extern int               foamWrSExpr    (FILE *, Foam, ULong sxioMode);
extern Foam              foamRdSExpr    (FILE *, FileName *, int *lno);

extern SExpr             foamToSExpr    (Foam);
extern Foam              foamFrSExpr    (SExpr);

#endif

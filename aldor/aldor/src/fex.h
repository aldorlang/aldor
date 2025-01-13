#ifndef _FEX_H
#define _FEX_H
#include "axlobs.h"

enum foamSxFlags {
	FOAMSX_None = 0,
	FOAMSX_Syme = 1,
	FOAMSX_LineInfo = (1<<1)
};

typedef Enum(foamSxFlags) FoamSxFlags;

#if 0
// Emacs is confused by Enum(x)
}
#endif

extern int               foamWrSExpr    (FILE *, Foam, ULong sxioMode);
extern Foam              foamRdSExpr    (FILE *, FileName *, int *lno);

extern SExpr             foamToSExprExtra (Foam, FoamSxFlags);
extern SExpr             foamToSExpr      (Foam);
extern Foam              foamFrSExpr      (SExpr);

#endif

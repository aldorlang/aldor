/*****************************************************************************
 *
 * opttools.h: Generic Optimization Tools
 *
 * Copyright (c) 1990-2007 Aldor Software Organization Ltd (Aldor.org).
 *
 ****************************************************************************/

#ifndef _OPTTOOLS_H
#define  _OPTTOOLS_H

#include "axlobs.h"

#include "of_comex.h"
#include "of_cprop.h"
#include "usedef.h"




# if 0
union VarInfoUnion {
	ExpInfo		expInfo;	/* used by: comex */
	Foam		foam;		/* used by: cprop, usedef */

} ;

typedef union  VarInfoUnion * VarInfo;

#endif

typedef Pointer VarInfo;

DECLARE_LIST(VarInfo);

typedef int (* VarInfoPrintFn)(VarInfo) ;

#if 0

union AssociationTypeUnion {
	VarInfoList	list;
	VarInfo		single;
};

typedef union AssociationTypeUnion * AssociationType;

#endif

typedef Pointer AssociationType;

DECLARE_LIST(AssociationType);

/*****************************************************************************
 *
 * :: Exports
 *
 ****************************************************************************/

extern VarInfoList	otGetVarInfoList	(Foam);

/* NOTE: you may use the otSetVarInfo macro*/
extern void		otSetVarInfo0		(VarInfo, Foam);

/* NOTE: you may use the otAddVarInfo macro*/
extern void		otAddVarInfo0		(VarInfo, Foam);

extern void		otProgInfoInit		(UShort, int, int, Foam);
extern void		otProgInfoFini		(void);
extern void 		otPrintVarAssociations	(VarInfoPrintFn);

extern Bool		otIsMovableData		(Foam);
extern Bool		otIsConstSyme		(Syme);
extern Bool 		otSymeIsFoamConst	(Syme);
extern Bool		otIsForcer		(Foam);
extern void		otTransferFoamInfoToSyme(Syme, Foam);
extern void		otTransferFoamInfo	(SymeList, Foam);

/****************************************************************************
 *
 * :: Macros
 *
 ****************************************************************************/

# define OT_ASSOCIATION_SINGLE		0x0001
# define OT_ASSOCIATION_LIST		0x0002
# define OT_ASSOCIATION_VECTOR		0x0004

# define otAddVarInfo(info, var)   otAddVarInfo0((VarInfo) (info), var)
# define otSetVarInfo(info, var)   otSetVarInfo0((VarInfo) (info), var)

# define otIsDef(foam)		(foamTag(foam) == FOAM_Set ||	\
				 foamTag(foam) == FOAM_Def)

# define otIsVar(foam)		(foamTag(foam) == FOAM_Loc ||	\
				 foamTag(foam) == FOAM_Par ||	\
				 foamTag(foam) == FOAM_Lex ||	\
				 foamTag(foam) == FOAM_Glo)

# define otIsVar(foam)		(foamTag(foam) == FOAM_Loc ||	\
				 foamTag(foam) == FOAM_Par ||	\
				 foamTag(foam) == FOAM_Lex ||	\
				 foamTag(foam) == FOAM_Glo)

# define otIsLocalVar(foam)	(foamTag(foam) == FOAM_Loc ||	\
				 foamTag(foam) == FOAM_Par)

# define otIsNonLocalVar(foam)	(foamTag(foam) == FOAM_Lex ||	\
				 foamTag(foam) == FOAM_Glo)

/* Note that foamTag(foam) is always >= FOAM_DATA_START */
# define otIsFoamConst(foam)    (foamTag(foam) < FOAM_DATA_LIMIT)

# define otDereferenceCast(foam)   \
	while (foamTag(foam) == FOAM_Cast) foam = foam->foamCast.expr;

#endif /* _OPTTOOLS_H */

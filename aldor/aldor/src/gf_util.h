/*****************************************************************************
 *
 * gf_util.h: Common declarations and macros for foam code generation.
 *
 * Copyright (c) 1990-2007 Aldor Software Organization Ltd (Aldor.org).
 *
 ****************************************************************************/

#ifndef _GF_UTIL_H_
#define _GF_UTIL_H_

#include "axlobs.h"
#include "foamsig.h"
#include "of_util.h"

extern Bool genfoamDebug, genfoamHashDebug, genfoamConstDebug;

#define genfDEBUG		DEBUG_IF(genfoamDebug)
#define genfHashDEBUG		DEBUG_IF(genfoamHashDebug)
#define genfConstDEBUG		DEBUG_IF(genfoamConstDebug)

#define genfNumProg(_d,_s)	fprintf(dbOut, "numProgs[%d] = %s\n", (_d), (_s))

/*
 * Structure describing the domains that have been brought into scope at a
 * given Foam lambda level. Works on constant domains only.
 */
typedef enum { HT_Id, HT_TFormHash, HT_SefoHash, HT_SefoHashExporter } HashType;

typedef struct domainCache {
	HashType	type;
	void *		args[2];
	Foam		hashVar;
} *DomainCache;


DECLARE_LIST(DomainCache);
DECLARE_LIST(VarPool);

/*
 * The types of lambda levels that the code generator creates.
 */
typedef enum {
	GF_Lambda,
	GF_Add0,
	
	GF_START_TYPE,	
		GF_File = GF_START_TYPE,
		GF_Add1,
		GF_Default,
		GF_DefaultCat,
	GF_END_TYPE = GF_DefaultCat,

	GF_Gener0,
	GF_Gener1,

	GF_Saved
} GenFoamTag;

#define gen0IsDomLevel(tag) ((tag) >= GF_START_TYPE && (tag) <= GF_END_TYPE)

typedef enum {
	PT_Std,
	PT_DeepStart,
	PT_Gener = PT_DeepStart,
	PT_ExFn
} ProgType;

#define gen0InGener(pt) ((pt) == PT_Gener)
#define gen0InDeep(pt)  ((pt) >= PT_DeepStart)

/*
 * Structure describing the state of code generation for a single lambda.
 */
typedef struct gfs {
	GenFoamTag	tag;		/* Type of lambda level we create */
	int		stabLevel;	/* level number of symbol table */
	int		foamLevel;	/* nesting level of Foam progs */
	Stab		stab;		/* lambda's symbol table */
	VarPool		localPool;	/* prog's locals */
	VarPool		lexPool;	/* prog's lexicals */
	VarPoolList	envLexPools;	/* stack of previous lexical envs */
	FoamBox		params;		/* prog's parameters */
	AIntList	formatStack;	/* nested prog lexical level stack */
	AIntList	formatUsage;	/* stack of levels actually used */
	AIntList	fluidsUsed;	/* Fluids pushed in this scope */
	Foam		program;	/* the foam for the prog */
	int		yieldCount;	/* number of yields in prog */
	ProgType	progType;	/* type of current prog */
	int		labelNo;	/* current label number on prog */
	AIntList	yieldLabels;	/* label numbers for each yield */
	int		yieldPlace;	/* label for yield result */
	Foam		yieldValueVar;	/* variable holding yield value */
	FoamList	lines;		/* the code for the prog */
	FoamList	inits;		/* the initialization code */
	FoamList *	importPlace;	/* Positions that are safe for gets */
	AIntList	importPlacePrev;/* safe places nested by where */
	SymeList	funImportList;	/* functions imported into prog */
	TFormList	domImportList;	/* domains imported into prog */
	FoamList	domList;	/* vars for imported domains */
	Bool		hasTemps;	/* true iff prog has temp variables */
	FoamList	envVarStack;	/* for inner "where" envs */
	AIntList	envFormatStack; /* stack of "where" env formats */
	int		whereNest;	/* depth of where nesting at lambda */
	TForm		type;		/* return type of prog. */
	AbSyn		param;		/* params for lambda bodies. */
	AbSyn		exporter;	/* exporter for add bodies. */
	struct gfs	*parent;	/* state for parent prog */
	struct gfs	*base;		/* State saved in pushed env */
	DomainCacheList domCache;	/* instantiated domains in scope */
	Foam		dbgContext;	/* Used in debugger calls */
} *GenFoamState;

/*
 * Structure describing state of code for exports of a domain
 */

typedef struct tes {
	int 		foamLevel;	/* Level for current type */
	Foam 		defMap;         /* Bitmap of set locals */
	SymeList 	domExportList;  /* List of exported symbols */
	Syme		selfSyme;	/* % 			    */
	FoamList	varList;  	/* List of temp. vars defined */
	Table		domInittedTbl;  /* Those exports with a defn */
	Table		domCondTbl;     /* Conditional exports */
	Foam		names;
	Foam 		types;
	Foam		vals;	
	Foam		szVar;		/* Stash for size variable */
	FoamList	setPlace;	/* slot for patching size */
	int 		size;		/* n exports processed so far */
	Foam		self;
	Foam		selfHash;
} *ExportState;
/*
 * Names for extra slots needed in certain N-ary nodes.
 */

# define OpSlot	   1
# define TypeSlot  1
# define EnvSlot   1
# define ProtoSlot 1


/*
 * Macros for constructing builtin arrays and records in foam.
 */
#define gen0ANew(vec, type, size) \
	foamNewSet(foamCopy(vec), foamNewANew(type, foamNewSInt(size)))

#define gen0ASet(vec, i, type, val) \
        foamNewSet(foamNewAElt(type, foamNewSInt(i), foamCopy(vec)), val)

#define gen0RNew(rec, fmt) \
	foamNewSet(foamCopy(rec), foamNewRNew(fmt))

#define gen0RSet(rec, fmt, i, val) \
	foamNewSet(foamNewRElt(fmt, foamCopy(rec), i), val)

#define gen0FoamKind(syme)	(symeHasFoamKind(syme) ? symeFoamKind(syme) : \
				 symeFoamKind(symeOriginal(syme)))

/*#define gen0SetFoamKind(syme,k) symeSetFoamKind(syme, k)*/
void gen0SetFoamKind(Syme, FoamTag);

#define gen0VarIndex(syme)	(symeHasVarIndex(syme) ? symeVarIndex(syme) : \
				 symeVarIndex(symeOriginal(syme)))
#define gen0SetVarIndex(syme,v) symeSetVarIndex(syme, v)

/* GenC assumes this */
#define gen0InitialiserName(libname) (strCopy(libname))

#define gen0TempLex(type)	gen0TempLex0(type, emptyFormatSlot)
#define	gen0Temp(type)		gen0Temp0(type, emptyFormatSlot)
#define	gen0TempLocal(type)	gen0TempLocal0(type, emptyFormatSlot)

extern GenFoamState	gen0State;

extern FoamList		gen0GlobalList,
			gen0FormatList,
#ifdef NEW_FORMATS
			gen0ParamsList,
#endif
			gen0DeclList,
			gen0ProgList;

extern int		gen0NumGlobals,
			gen0RealFormatNum,
			gen0FormatNum,
#ifdef NEW_FORMATS
			gen0NumParams,
#endif
			gen0NumProgs,
			gen0FwdProgNum,
			gen0GenerFormat,
			gen0GenerRetFormat,
			gen0LazyFunFormat;

extern String		gen0ProgName,
			gen0DefName,
			gen0FileName;

extern Bool		gen0SmallHashCodes;

extern FoamSigList 	gen0LazySigList;
extern AIntList		gen0LazyConstTypeList;
extern AIntList		gen0LazyConstDefnList;
extern AIntList		gen0BuiltinExports;

extern TForm		gen0AbType		(AbSyn ab);
extern Foam	   	genImplicit		(AbSyn, AbSyn, FoamTag);
extern void		gen0AddConst		(AInt, AInt);
extern void		gen0AddFormat		(AInt, AInt);
extern Foam		gen0GetGlobal		(AInt);
extern int		gen0AddGlobal		(Foam);
extern Foam             gen0BuiltinCCall	(FoamTag, String, String, 
						 Length, ...);
extern void      	gen0DefSequence		(AbSyn absyn);
extern Foam		gen0ExtendSyme		(Syme);
extern void		gen0IncLexLevels	(Foam, AInt);
extern void		gen0AddLexLevels	(Foam, int);
extern GenFoamState	gen0NewState		(Stab, int, GenFoamTag);
extern GenFoamState	gen0NthState		(AInt);
extern void		gen0PushFormat		(int);
extern Foam		gen0Syme		(Syme);
extern Foam		gen0Temp0		(int, int);
extern Foam		gen0TempLocal0		(int, int);
extern Foam		gen0NewLex		(int, int);
extern Foam		gen0TempLex0		(int, int);
extern Foam		gen0TempLexNth		(int, int);
extern Foam		gen0TempFrDDecl		(AInt, Bool);
extern FoamTag		gen0Type		(TForm, AInt *);
extern void		gen0UseStackedFormat	(AInt);
extern void		gen0Vars		(Stab stab);

extern int		gen0GetImportLexLevel	(Syme);
extern int		gen0GetSefoLexLevel	(Sefo);
extern SefoList		gen0GetSefoInnerSefos	(Sefo);
extern int		gen0FoamImportLevel	(AInt);

extern Bool	   	gen0AddImportPlace	  (FoamList *);
extern void	   	gen0ResetImportPlace	  (FoamList);

extern void		genFoamStmt		(AbSyn);
extern Foam		genFoamVal		(AbSyn);
extern Foam		genFoamType		(AbSyn);
extern Foam 	   	genFoamBit	  	(AbSyn);

extern String		gen0GlobalName		(String, Syme);
extern void		genGlobalInfo		(Foam, String *, 
						 String *, int *);

extern Foam	   gen0MakeMultiEvaluable (int, int, Foam);
extern void		gen0IssueDCache		(void);

extern Bool 		gen0IsCatDefForm	(GenFoamState);
extern Bool 		gen0IsCatInner		(void);

extern Foam	        gen0CharArray	  	(String);

extern AInt		gen0RecordFormatNumber	(TForm);
extern AInt		gen0MultiFormatNumber	(TForm);
extern AInt		gen0MFmtNumberForSig	(int, FoamTag*);
extern AInt	   	gen0MakeTupleFormat  	(void);
extern AInt 		gen0StdDeclFormat	(Length, String *, FoamTag *, AInt *);
extern void 		gen0SetDDeclUsage	(AInt, FoamDDeclTag);

extern int		gen0RootEnv		(void);

#endif /* !_GF_UTIL_H_ */

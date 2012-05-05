/*****************************************************************************
 *
 * stab.h: Symbol table definitions.
 *
 * Copyright (c) 1990-2007 Aldor Software Organization Ltd (Aldor.org).
 *
 ****************************************************************************/

#ifndef _STAB_H_
#define _STAB_H_

# include "axlobs.h"

/******************************************************************************
 *
 * :: Type Form Uses
 *
 *****************************************************************************/

typedef struct tformUses *TFormUses;

DECLARE_LIST(TFormUses);

struct tformUses {
	BPack(Bool)	isImported;
	BPack(Bool)	isExplicitImport;
	BPack(Bool)	isParamImport;
	BPack(Bool)	isCategoryImport;
	BPack(Bool)	isCatConditionImport;
	TForm		tf;
	TQual		exports;
	TQual		imports;
	TQual		inlines;
	TQualList       cascades;
	AbSynList	extension;
	AbSynList	extendees;
	SymbolList	declarees;
	TFormUsesList	dependents;		/* Fields for 1st top. sort */
	TFormUsesList	dependees;
	Length		nbefore;
	Length		nafter;
	TFormUsesList	cdependents;		/* Fields for clique sort */
	TFormUsesList	cdependees;
	Length		ncbefore;
	Length		ncafter;
	Bool		sortMark;		/* Fields for each top. sort */
	TFormUsesList	outEdges;
	Length		inDegree;
	Bool		cmarked;		/* Fields for union-find. */
	TFormUses	crep;
};

/******************************************************************************
 *
 * :: Stab levels
 *
 *****************************************************************************/

struct stabLevel {
	ULong		lexicalLevel;		/* Lexical depth	*/
	ULong		lambdaLevel;		/* Lambda nesting depth */
	ULong		serialNo;		/* Unique index		*/
	Hash		hash;			/* Lazy syme hash code  */
	BPack(Bool)	isLocked;		/* Can modify level?	*/
	BPack(Bool)	isChecked;		/* All tforms checked?	*/
	BPack(Bool)	isSubstable;		/* Any substable symes? */
	UShort		intStepNo;		/* interactive step     */
	Table		tbl;			/* Symbol->StabEntry tbl*/
	StabList	children;		/* child symbol tables	*/
	SrcPos		spos;			/* start of scope posn	*/
	SymbolList	idsInScope;		/* ids seen in scope	*/
	AbSynList	labelsInScope;		/* labels seen in scope */

	struct {
		TFormUsesList	list;		/* all type forms used  */
		Table		table;		/* used for large lvls	*/
	}		tformsUsed;

	TFormList	tformsUnused;		/* registered but unused */
	SymeList	boundSymes;		/* List of bound symes */
	SymeList	extendSymes;		/* List of extend symes */
};

/******************************************************************************
 *
 * :: Stab level accessors
 *
 *****************************************************************************/

# define	stabLevelNo(stab)		(car(stab)->lexicalLevel)
# define	stabLambdaLevelNo(stab)		(car(stab)->lambdaLevel)
# define	stabSerialNo(stab)		(car(stab)->serialNo)
# define	stabLevelIsLocked(stab)		(car(stab)->isLocked)
# define	stabLevelIsSubstable(stab)	(car(stab)->isSubstable)
# define	stabLevelIsLarge(stab)		(car(stab)->tformsUsed.table)
# define	stabGetBoundSymes(stab)		(car(stab)->boundSymes)

# define	stabLockLevel(stab)		(car(stab)->isLocked = true)
# define	stabUnlockLevel(stab)		(car(stab)->isLocked = false)

# define	stabClrSubstable(stab)		(car(stab)->isSubstable=false)
# define	stabSetSubstable(stab)		(car(stab)->isSubstable=true)

/******************************************************************************
 *
 * :: Stab entries
 *
 *****************************************************************************/

/* Each stab entry caches lists of symes/types keyed by conditions. */
struct stabEntry {
	Length		argc;
	AbLogic *	condv;
	SymeList *	symev;
	TPoss *		possv;
	SymeList        pending;
};

/******************************************************************************
 *
 * :: Basic operations
 *
 *****************************************************************************/

extern void		stabInitGlobal		(void);
extern void		stabInitFile		(void);
extern void		stabFiniGlobal		(void);
extern void		stabFiniFile		(void);
extern Stab		stabGlobal		(void);
extern Stab		stabFile		(void);

extern Stab             stabNewGlobal           (void);
extern Stab             stabNewFile             (Stab);

extern ULong		stabMaxSerialNo		(void);
extern UShort		stabMaxDefnNum		(void);

#define			STAB_LEVEL_LARGE	(1<<1)
#define			STAB_LEVEL_LOOP		(1<<2)
#define			STAB_LEVEL_WHERE	(1<<3)

extern Stab		stabPushLevel		(Stab, SrcPos, ULong);
extern Stab		stabPopLevel		(Stab);
extern void		stabFree		(Stab);

extern void		stabSeeOuterImports	(Stab);
extern void		stabGetSubstable	(Stab);
extern void		stabExtendMeanings	(Stab, Syme);
extern SymeList		symeListExtend		(SymeList, Syme);
extern Syme		symeListHasExtension	(SymeList, Syme);
extern Syme		symeListHasExtendee	(SymeList, Syme);

extern SymeList		stabGetMeanings		(Stab, AbLogic, Symbol);
extern TPoss		stabGetTypes		(Stab, AbLogic, Symbol);

extern Syme		stabGetSelf		(Stab);
extern Syme		stabGetExportMod	(Stab, SymeList,Symbol,TForm);
extern Bool		stabGetLex		(Stab, Symbol);
extern Syme		stabGetLibrary		(Symbol);
extern Syme		stabGetArchive		(Symbol);

extern void		stabUseMeaning		(Stab, Syme);
extern Bool		stabHasMeaning		(Stab, Syme);
extern Syme		stabAddMeaning		(Stab, Syme);
extern void		stabPutMeanings		(Stab, SymeList);

extern Syme		stabDefParam		(Stab, Symbol, TForm);
extern Syme		stabDefLexConst		(Stab, Symbol, TForm);
extern Syme		stabDefLexVar		(Stab, Symbol, TForm);
extern Syme		stabDefFluid		(Stab, Symbol, TForm);
extern Syme		stabDefImport		(Stab, Symbol, TForm, TForm);
extern Syme		stabDefExport		(Stab, Symbol, TForm, Doc);
extern Syme		stabDefExtendee		(Stab, Symbol, TForm, Doc);
extern Syme		stabDefExtend		(Stab, Symbol, TForm);
extern Syme		stabDefLibrary		(Stab, Symbol, TForm, Lib);
extern Syme		stabDefArchive		(Stab, Symbol, TForm, Archive);
extern Syme		stabDefForeign		(Stab, Symbol, TForm,
						 ForeignOrigin);
extern Syme		stabDefBuiltin		(Stab, Symbol, TForm,
						 FoamBValTag);

extern void		stabUseMeaningShadow	(AbSyn);
extern void		stabUseMeaningUnshadow	(void);
extern void		stabSetSyme		(Stab, AbSyn, Syme, AbLogic);

extern Bool		stabIsUndeclaredId	(Stab, Symbol);
extern void		stabDeclareId		(Stab, Symbol, TForm);

extern Bool             stabIsChild             (Stab parent, Stab child);

extern int		stabPrint		(FILE *, Stab);
extern int		stabPrintTo		(FILE *, Stab, int minlev);
extern int		tfuPrint		(FILE *, TFormUses);
extern int		tfulPrint		(FILE *, TFormUsesList);

extern SymeList		stabGetExportedSymes	(Stab);
extern TQualList	stabImportFrom		(Stab, TQual);

/*
 * TForms
 */

extern TForm	 	stabGetTForm		(Stab, AbSyn, TForm failed);
extern TForm	 	stabDefTForm		(Stab, TForm);

extern Bool	 	stabIsImportedTForm	(Stab, TForm);
extern TForm	 	stabMakeUsedTForm	(Stab, AbSyn, TfCondElt);
extern TForm	 	stabMakeImportedTForm	(Stab, TForm);
extern TForm	 	stabExportTForm	    	(Stab, TForm);
extern TForm	 	stabImportTForm	    	(Stab, TForm);
extern TForm	 	stabInlineTForm	    	(Stab, TForm);
extern TForm	 	stabQualifiedExportTForm(Stab, AbSyn, TForm);
extern TForm	 	stabQualifiedImportTForm(Stab, AbSyn, TForm);
extern TForm	 	stabQualifiedInlineTForm(Stab, AbSyn, TForm);
extern TForm		stabExplicitlyImportTForm(Stab, TForm);
extern TForm	 	stabCategoricallyImportTForm(Stab, TForm);
extern TForm	 	stabParameterImportTForm(Stab, TForm);
extern TForm	 	stabAddTFormExtension	(Stab, TForm, AbSyn);
extern TForm	 	stabAddTFormExtendees	(Stab, TForm, AbSyn);
extern TForm	 	stabAddTFormDeclaree	(Stab, TForm, AbSyn);
extern TForm	 	stabAddTFormQuery	(Stab, TForm, TForm);
extern TForm	 	stabFindOuterTForm	(Stab, AbSyn);
extern TFormUses 	stabFindTFormUses	(Stab, AbSyn);

/*
 * Labels
 */

# define	 	stabGetLevelLabels(stab)  (car(stab)->labelsInScope)
extern void	 	stabAddLabel(Stab stab, AbSyn label);
extern AbSynList 	stabGetAllLabels(Stab stab);
extern Bool	 	stabLabelExists(Stab stab, Symbol label);
extern Bool	 	stabLabelExistsInThisStab(Stab stab, Symbol label);
extern AbSynList 	stabGetLabels(Stab stab, Symbol label);
extern AbSyn	 	stabGetLabelInThisStab(Stab stab, Symbol label);

/* following is used in generation of .asy files */

struct typesUsed {
    AbSynList	typesImported;
    AbSynList	typesInlined;
    AbSynList	typesOther;
};

struct typesUsed *getAllTypesUsed(StabLevel sl);

#endif /* _STAB_H_ */

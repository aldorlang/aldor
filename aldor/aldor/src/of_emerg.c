/*****************************************************************************
 *
 * of_emerg.c: Environment merging.
 *
 * Copyright (c) 1990-2007 Aldor Software Organization Ltd (Aldor.org).
 *
 ****************************************************************************/

/*
 * This file implements data structure merging.
 * 
 * Environment merging destructively modifies a foam unit by replacing
 * heap allocated storage by stack allocated storage when it can.
 *
 * emerge knows about four ways of creating new storage:
 *	FOAM_RNew	-- create a new record.
 *	FOAM_PushEnv	-- create a new lexical environment level.
 *	FOAM_ANew	-- create a new array.
 *	procedure entry	-- create a new lexical environment level.
 *
 * and one way of explicitly releasing allocated storage:
 *	FOAM_Free
 *
 * For each of these types of storage we analyze whether or not it escapes.
 * A piece of storage is deemed to escape if we pass it to another function,
 * it is returned from the function or it is pointed to by another structure
 * which escapes.
 * 
 * [Actually we don't mind if it is returned from a function]
 * 
 * Once we determine that a structure does not escape (and for arrays that
 * it indexed only by constant values), then we replace all the elements
 * of the structure with local variables. We make sure that all local 
 * variables that are aliased to non-escaping storage get expanded too.
 *
 * To Do:
 *	Get rid of -Wemerge-noalias option
 *	New environments: FOAM_TRNew, FOAM_IRElt, FOAM_TRElt
 *	Better flow anlysis.
 *		[ More acurately determine when variables alias each other.
 *		The analysis is currently too conservative, and will not merge
 *		variables which are multiply aliased, but these can be merged
 *		if they are not side-effected. ] TTT does not think we need 
 *              better flow analysis here. If we do the expanding properly
 *              (which TTT thinks we do), other later optimisations will clean
 *              up after us.
 */

# include "axlphase.h"
# include "of_deadv.h"
# include "of_util.h"

Bool	emergeDebug	= false;
#define emergeDEBUG(s)	DEBUG_IF(emergeDebug, s)

typedef enum emUsageState {
        EM_DontKnow,            /* what we start with */
	EM_NotEnv,		/* Not a candidate for merging */
	EM_EscapingEnv,		/* A candidate, but environment escapes */
	EM_NonEscapingEnv,	/* A candidate which doesn't escape */
	EM_AliasNonEsc          /* is used as an alias of non-escaping env */
} EmUsageState;

typedef struct emUsage {
	EmUsageState	used;		/* Usage flag */
	int		format;		/* format of env  (size of array) */
	Foam		type;		/* Foam instruction creating env */
	Foam		*remap;		/* Vector of remaps to locals */
	BPack(Bool)	mark;		/* mark for detecting circular links */
	BPack(Bool)	isSet;		/* has an assignment been seen? */
	struct emUsage	*link;		/* for following aliases of locals */
} *EmUsage;

local void	emMergeDefs		(Foam defs);
local void	emMergeProg		(Foam prog);
local EmUsage	emMakeUsageVec		(Foam ddecl);
local void	emMarkUsage		(Foam expr);
local void	emMarkLocal		(Foam loc);
local void	emMarkDef		(Foam def);
local void	emMarkParents		(Foam);
local void	emMarkCallArgs		(Length argc, Foam *argv);
local void	emMarkClos		(Foam clos);
local void	emMarkAElt		(Foam aelt);
local void	emMarkRElt		(Foam aelt);
local void	emMarkRRElt		(Foam aelt);
local Bool	emCheckRElt		(Foam relt, int fmt);
local void	emComputeRemap		(Foam prog);
local void	emRemapEnv0		(EmUsage, EmUsage);
local void	emMergeEnv		(int n, Bool);
local void	emMergeArray		(int n, Foam, Bool);
local Foam	emNewBody		(void);
local void	emAddStmt		(Foam stmt);
local void	emMakeNewLocals		(Foam prog);
local void	emMergeEnvs		(Foam prog);
local Foam	emMergeDef		(Foam def);
local Foam	emNewEnvSet		(Foam def, Foam lhs, Foam rhs);
local Foam	emMergeExpr		(Foam expr);
local Foam	emMergeRElt		(Foam expr, Foam env, int index);
local Foam	emMergeEElt		(Foam expr, Foam env, int index);
local Foam	emMergeLex		(Foam lex);
local Foam	emMergeEEnv		(Foam eenv);
local Foam	emMergeFree		(Foam foam);
local Foam	emMergeReturn		(Foam expr);
local Foam	emMergeAElt		(Foam aelt);
local EmUsage	emUsageAliasing 	(Foam loc);
local EmUsage	emUsage 		(Foam foam);
local EmUsage	emUsageFromLocalIndexAliasing	(long index);
local EmUsage	emUsageFromLocalIndex(long index);
#ifndef NDEBUG
local long      emIndexFromUsage        (EmUsage u);
#endif

local void      emCopyUsedTagsTo        (EmUsageState *arr);
local Bool      emUsedTagsChanged       (EmUsageState *arr);
local void      emNormaliseUsage        (void);
local void      emCleanTypeUsage        (void);
local void	emNewParentTable	(void);
local void	emFreeParentTable	(void);
local void	emSetParent		(Foam child, Foam parent);
local Foam	emGetParent		(Foam child);
local Foam	emGetNthParent		(Foam child, int *level);
local Foam 	emUnEEnv		(Foam foam);

Bool		emChanged;		/* True iff the unit changed. */
EmUsage		emLocalUsage;		/* Local usage vector for a prog. */
int		emNumLocals;		/* Number of locals in the prog */
int		emOrigNumLocals;	/* Number of locals before merging */
FoamList	emNewLocals;		/* Decls of locals added to prog. */
FoamList	emStmtList;		/* Statements added to prog. */
Foam		*emFormats;		/* Formats for the unit. */
int             emCount;                /* how many times are we merging */
int             emDefNo;                /* the current definition index being processed */

Bool		emNoAlias = false;	/* Avoid alias code for non-envs */


/*****************************************************************************
 *
 * :: Macros
 *
 ****************************************************************************/

#define emIsArray(foam) ((foam) && foamTag(foam) == FOAM_ANew)

#define emOldLocal(i) (foamTag(i) != FOAM_Loc ||	\
		       ((i)->foamLoc.index < emOrigNumLocals))

/*****************************************************************************
 *
 * :: Main code
 *
 ****************************************************************************/

void
emSetNoAlias()
{
	emNoAlias = true;
}


/*
 * Merge environments until no more merging is possible. 
 * We use emCount
 */
void
emMergeUnit(Foam unit)
{
	assert(foamTag(unit) == FOAM_Unit);
	emFormats = unit->foamUnit.formats->foamDFmt.argv;
	emChanged = true;
	
	for (emCount = 0; emChanged && emCount < 10 ; emCount++) {
		dvElim(unit);
		emChanged = false;
		emMergeDefs(unit->foamUnit.defs);
	}
	
	assert(foamAudit(unit));
}

/*
 * For each program definition in a unit, merge it.
 */

int emDebugDefNo = 1; /* this is useful when debugging a perticular prog */

local void
emMergeDefs(Foam defs)
{

        for(emDefNo =0; emDefNo<foamArgc(defs); emDefNo++) {
	        Foam	def = defs->foamDDef.argv[emDefNo];
		Foam	prog;

/* #if 0*/ /* enable this if you want to see in-out foam when debugging */
		emergeDEBUG({  
			if(emDefNo==emDebugDefNo) {
				printf("Prog--index%d--count%d<<\n",emDefNo,emCount); 
				foamWrSExpr(dbOut, def,SXRW_Default);
			}
		});
/* #endif */
		assert(foamTag(def) == FOAM_Def);
		prog = def->foamDef.rhs;
		if (foamTag(prog) == FOAM_Prog)
			emMergeProg(prog);

/* #if 0*/ /* enable this if you want to see in-out foam when debugging */
		emergeDEBUG({
			if(emDefNo==emDebugDefNo) {
				printf("Prog--index%d--count%d>>\n",emDefNo,emCount); 
				foamWrSExpr(dbOut, def,SXRW_Default);
			}
		});
/* #endif */
	}
}


/*
 * Merge the environments for a single program.
 */
local void
emMergeProg(Foam prog)
{
	int emMarkCount = 1 ;
	EmUsageState  *  usedArray;
	emLocalUsage	 = emMakeUsageVec(prog->foamProg.locals);
	emOrigNumLocals	 = foamDDeclArgc(prog->foamProg.locals);
	
	/* previously on ER ... , no seriously, this keeps all the used 
	   tags from the previous marking run */
	usedArray = (EmUsageState *) stoAlloc(OB_Other, (emOrigNumLocals+1) * sizeof(EmUsageState));

	emLocalUsage->format = prog->foamProg.levels->foamDEnv.argv[0];
	emNewParentTable();

	emMarkUsage(prog->foamProg.body); /*first time */
	emCopyUsedTagsTo(usedArray);

	for ( ; emMarkCount<10 ;emMarkCount++) {
		emCleanTypeUsage();
		emMarkUsage(prog->foamProg.body);
		if ( ! emUsedTagsChanged(usedArray) ) break;
		emCopyUsedTagsTo(usedArray);
	}

	emComputeRemap(prog);

#if 0 /* enable this if you want to see in-out sets when debugging */
        emergeDEBUG({
		fprintf(stderr,"Pass:%2d Prog:%2d #locals:(%3d -> %3d)\n",
			emCount, emDefNo,emOrigNumLocals, emNumLocals);
	})
#endif 
	if (!emChanged) return;

	/* now that the marking is done we can get rid of all links 
	   except to non-escaped storage */
	emNormaliseUsage();	
	
	emMergeEnvs(prog);
	
	if (emLocalUsage->used == EM_NonEscapingEnv)
		prog->foamProg.levels->foamDEnv.argv[0] = emptyFormatSlot;
	emFreeParentTable();
}


/*
 * Make an empty usage vector for the locals in a program.
 */
local EmUsage
emMakeUsageVec(Foam ddecl)
{
	int	i, size;
	EmUsage emu;
	
	assert(foamTag(ddecl) == FOAM_DDecl);
	size = foamDDeclArgc(ddecl) + 1;	/* +1 for Env(0) */
	emu = (EmUsage) stoAlloc(OB_Other, size*sizeof(struct emUsage));

	for(i=0; i < size; i++ ) {
		emu[i].used   = EM_DontKnow;
		emu[i].remap  = 0;
		emu[i].link   = 0;
		emu[i].mark   = 0;
		emu[i].type   = 0;
		emu[i].isSet  = false;
		emu[i].format = emptyFormatSlot;
	}
	emu[0].used = EM_NonEscapingEnv;	/* for Env(0) */
	return emu;
}


/*
 * Traverse an arbitrary Foam expression, looking for environment usage,
 * and marking escaping environments.
 */
local void
emMarkUsage(Foam expr)
{
	Foam	*argv;
	int	argc;

	foamIter(expr, arg, emMarkUsage(*arg));

	switch(foamTag(expr)) {
	case FOAM_Set:
	case FOAM_Def:
		emMarkDef(expr);
		return;
	case FOAM_AElt:
		emMarkAElt(expr);
		return;
	case FOAM_RElt:
		emMarkRElt(expr);
		return;
	case FOAM_RRElt:
		emMarkRRElt(expr);
		return;
	case FOAM_OCall: {
		Foam	env = expr->foamOCall.env;
		emMarkLocal(env);
		argv = expr->foamOCall.argv;
		argc = foamArgc(expr) - 3;
		break; }
	case FOAM_CCall:
		argv = expr->foamCCall.argv;
		argc = foamArgc(expr) - 2;
		break;
	case FOAM_PCall:
		argv = expr->foamPCall.argv;
		argc = foamArgc(expr) - 3;
		break;
	case FOAM_BCall:
		argv = expr->foamBCall.argv;
		argc = foamArgc(expr) - 1;
		break;
	case FOAM_Clos:
		emMarkClos(expr);
		return;
	case FOAM_Return:
		/*!! need to write a copy on returner! */
		emMarkLocal(expr->foamReturn.value);
		return;
	default:
		return;
	}
	emMarkCallArgs(argc, argv);
}

#define emIsMergeable(x)	\
	(foamTag(x) == FOAM_Loc || (foamTag(x) == FOAM_Env && \
				    (x)->foamEnv.level == 0))

/*
 * Mark a local as an escaping environment, traversing alias links.
 */
local void
emMarkLocal(Foam var)
{
	Foam nvar;
	EmUsage		u;

 	foamDereferenceCast(var);

	if (foamTag(var) == FOAM_Values) {
		int	i;
		for(i=0; i<foamArgc(var); i++)
			emMarkLocal(var->foamValues.argv[i]);
		return;
	}

	nvar = emUnEEnv(var);
	if (nvar != var) emChanged = true;

	do {
 		foamDereferenceCast(nvar);

		if (emIsMergeable(nvar)) {
		        /* modify the used field of the actual storage */
			u = emUsageAliasing(nvar); 
			u->used = EM_EscapingEnv;
		}
		nvar = emGetParent(nvar);
	} while(nvar);
}

/*
 * Predicate identifying the foam instructions which create environmnets.
 */
#define emIsMergingCandidate(foam) (foamTag(foam) == FOAM_RNew	  || \
				    foamTag(foam) == FOAM_PushEnv || \
				    (foamTag(foam) == FOAM_ANew	  && \
				     emIsSInt(foam->foamANew.size)))

#define emIsSInt(foam)	\
	(foamTag(foam) == FOAM_SInt || \
	 (foamTag(foam) == FOAM_Cast   \
	  && foamTag(foam->foamCast.expr) == FOAM_SInt))

#define emSIntValue(foam) \
	(foamTag(foam) == FOAM_SInt ? foam->foamSInt.SIntData : \
	 foam->foamCast.expr->foamSInt.SIntData)

/*
 * Check a definition for usage.  This function finds the creation
 * of new environments and keeps track of local variable aliases.
 * The logic could be cleaned up a bit.
 */
local void
emMarkDef(Foam def)
{
	Foam	lhs = def->foamDef.lhs;
	Foam	rhs = def->foamDef.rhs;
 	Foam	rhs0 = rhs;
	FoamTag ltag = foamTag(lhs), rtag = foamTag(rhs);
	EmUsage usage;
	Bool 	wasSet = 999;

	assert(foamTag(def) == FOAM_Set || foamTag(def) == FOAM_Def);

	if (rtag == FOAM_PushEnv)
		emSetParent(lhs, rhs->foamPushEnv.parent);

	if (ltag == FOAM_Loc) {
		/* Update the root usage for this local */
		usage         = emUsage(lhs);
		wasSet 	      = usage->isSet;
		usage->isSet  = true;
		if (emIsMergingCandidate(rhs)) {
			/*
			 * What is the actual usage of this local
			 * taking into account aliasing etc.
			 */
			usage = emUsageAliasing(lhs);


			/* Check for reuse of locals */
			if (usage->type) {
				/*
				 * This local has already been allocated
				 * heap storage and we are now allocating
				 * some more. Since the new structure may
				 * be of a different type or size we have
				 * to mark it as escaping.
				 */
				emMarkLocal(lhs);
				return;
			}


			/* Has this local escaped already? */
			if (usage->used != EM_EscapingEnv) {
				if (rtag == FOAM_PushEnv)
					usage->format =rhs->foamPushEnv.format;
				else if (rtag == FOAM_RNew) {
					if (emCheckRElt(lhs, rhs->foamRNew.format))
						usage->format = rhs->foamRNew.format;
					else
						emMarkLocal(lhs);
				}
				else 
					usage->format =
						emSIntValue(rhs->foamANew.size);
				usage->used = EM_NonEscapingEnv;
				usage->type = rhs;
			}
			return;
		}
	}
	else
		emMarkParents(lhs);

	foamDereferenceCast(rhs0);

	if (foamTag(rhs0) == FOAM_Loc) {
		if (ltag == FOAM_Loc) {
			if (emUsageAliasing(lhs)->used ==
			    EM_EscapingEnv) {
				emMarkLocal(rhs0);
				emMarkLocal(lhs);
			}
			else if (emUsage(lhs)->link == 0 && !wasSet &&
				 emUsage(rhs0)->type && 
				 foamTag(emUsage(rhs0)->type)
				 == FOAM_PushEnv) {
				Foam parent = emGetParent(rhs0);
				emUsage(lhs)->link = emUsage(rhs0);
				if (parent)
					emSetParent(lhs, parent);
			}
			else if (!emNoAlias && emUsage(lhs)->link == 0) {
				Foam parent = emGetParent(rhs0);
				emUsage(lhs)->link = emUsageAliasing(rhs0);
				if (parent)
					emSetParent(lhs, parent);
			}
			else {
			        /* the lhs is already associated with another link */
			        assert(emUsage(lhs)->link);
				assert(emUsageAliasing(lhs)->used == EM_NonEscapingEnv ||
				       emUsageAliasing(lhs)->used == EM_DontKnow);
				if (emUsageAliasing(lhs)->used == EM_DontKnow) 
					emUsageAliasing(lhs)->link = emUsageAliasing(rhs0);
				emUsage(lhs)->link = emUsageAliasing(rhs0);
				/* emMarkLocal(rhs0); */
			}
		}
		else
			emMarkLocal(rhs);
	}
	else if (foamTag(rhs0) == FOAM_Env) {
		emSetParent(lhs, foamNewEnv(rhs0->foamEnv.level + 1));
		if (rhs0->foamEnv.level == 0) {
			emUsage(lhs)->link = emUsage(rhs0);
		}
	}
	else if (foamTag(rhs0) == FOAM_MFmt) {
		/*
		 * Strictly speaking we ought to always mark the lhs
		 * of a multi-valued set as escaping or at least we
		 * ought to check both sides of the definition. For
		 * now we don't because it always seems to work out
		 * okay except when the rhs is a catch expression.
		 */
		Foam	mval = rhs0->foamMFmt.value;


		/* Check for a catch on the rhs */
		if (foamTag(mval) == FOAM_Catch)
		{
			/* Mark the lhs (a FOAM_Values hopefully) */
			emMarkLocal(lhs);
		}
	}
}

/*
 * Mark a local variable and its parents as escaping.
 */
local void
emMarkParents(Foam foam)
{
	do {
		if (emIsMergeable(foam))
			emMarkLocal(foam);
		foam = emGetParent(foam);
	} while (foam);
}

/*
 * Mark the arguments of a function call as escaping.
 */
local void
emMarkCallArgs(Length argc, Foam *argv)
{
	int	i;
	for(i=0; i<argc; i++)
		emMarkLocal(argv[i]);
}

/*
 * Mark closure environments as escaping.
 */
local void
emMarkClos(Foam clos)
{
	Foam	env = clos->foamClos.env;

	emMarkLocal(env);
}

/*
 * Mark an array elt.
 */
local void
emMarkAElt(Foam aelt)
{
	Foam	index = aelt->foamAElt.index;
	Foam	array = aelt->foamAElt.expr;

 	foamDereferenceCast(array);

	if (foamTag(array) != FOAM_Loc) return;
	if (!emIsSInt(index))
		emMarkLocal(array);
	else if (emSIntValue(index) >= emUsageAliasing(array)->format)
		emUsageAliasing(array)->format = emSIntValue(index)+1;
	return;
}

/*
 * Mark a raw-record as escaping
 */
local void
emMarkRRElt(Foam rrelt)
{
	emMarkLocal(rrelt->foamRRElt.data);
	return;
}

/* 
 * Mark a record element
 * [actually just check that the record format is the same]
 */
local void
emMarkRElt(Foam relt)
{
	Foam var = relt->foamRElt.expr;

	if (!emCheckRElt(var, relt->foamRElt.format))
		emMarkLocal(var);
}

local Bool
emCheckRElt(Foam var, int fmt)
{
	foamDereferenceCast(var);

	if (foamTag(var) != FOAM_Loc) return false;
	
	if (emUsageAliasing(var)->format == fmt)
		return true;
	if (emUsageAliasing(var)->format == emptyFormatSlot)
		return true;
	if (foamEqual(emFormats[emUsageAliasing(var)->format],
		      emFormats[fmt]))
		return true;
	
	return false;
}


/*
 * compute the environment locals variable remap vector.
 */
local void
emComputeRemap(Foam prog)
{
	long	i;
	EmUsage	env0 = &emLocalUsage[0];

	emNumLocals = foamDDeclArgc(prog->foamProg.locals);


	emNewLocals = listNil(Foam);
	if (env0->used == EM_NonEscapingEnv) 
		emMergeEnv((int)0,false);
	if (env0->link && (env0->link->used == EM_NonEscapingEnv))
		emMergeEnv((int)0,true);
	
	for (i =0 ; i < emOrigNumLocals ; i++) {
		EmUsage	alias;
		EmUsage loc = emUsageFromLocalIndex(i);

		/* make locals for the heap allocated object */
		if (loc->used == EM_NonEscapingEnv)
			emMergeEnv(i+1,false);

		/* make locals for an "alias" of a non-escaping object */  
		alias = emUsageFromLocalIndexAliasing(i);
		if (loc->link && (alias->used == EM_NonEscapingEnv))
		{
			loc->link = alias;


			/* Careful with (Env 0) remap */
			if (alias == env0)
				emRemapEnv0(env0, loc);
			else
				emMergeEnv(i+1,true);
		}
	}

	emMakeNewLocals(prog);
}


/*
 * Ensure that locals for EElts of aliases of (Env 0) all map
 * to the same local. We assume that all aliases of (Env 0)
 * always point to (Env 0) and are never reassigned to other
 * records of the same format.
 */
local void
emRemapEnv0(EmUsage env0, EmUsage usage)
{
	int	size, format, i;


	/* Check the format */
	format = env0->format;


	/* Unused environments can't be emerged */
	if (format == emptyFormatSlot) return;


	/* How many elements are there in the environment? */
	size = foamDDeclArgc(emFormats[format]);


	/* Create a new remap vector */
	usage->remap = (Foam *)stoAlloc(OB_Other, size*sizeof(Foam));


	/* Copy the mappings for (Env 0) */
	for (i = 0;i < size; i++)
		usage->remap[i] = foamCopy(env0->remap[i]);
}


/*
 * Create the locals for the nth local environment.
 */
local void
emMergeEnv(int n, Bool isAlias)
{
	int	size, format, i;
	Foam	ddecl, type;

	type = isAlias ? emUsageFromLocalIndexAliasing(n-1)->type :
		emLocalUsage[n].type;

	if (emIsArray(type)) {
		emMergeArray(n, type, isAlias);
		return;
	}

	format = isAlias ? emUsageFromLocalIndexAliasing(n-1)->format : 
		emLocalUsage[n].format;
	if (format == emptyFormatSlot) return;

	ddecl = emFormats[format];
	size  = foamDDeclArgc(ddecl);
	emLocalUsage[n].remap = (Foam *)stoAlloc(OB_Other, size*sizeof(Foam));

	for(i=0; i < size; i++) {
		Foam	decl = foamCopy(ddecl->foamDDecl.argv[i]);

		emNewLocals = listCons(Foam)(decl, emNewLocals);
		emLocalUsage[n].remap[i] = foamNewLoc(emNumLocals++);
		emChanged = true;
	}
}

/*
 * Create locals for an array
 */
local void
emMergeArray(int n, Foam type, Bool isAlias)
{
	int	size, i, eltType;
	Foam	decl;

        assert(foamTag(type) == FOAM_ANew);
	assert(emIsSInt(type->foamANew.size));

	size	= emUsageFromLocalIndexAliasing(n-1)->format;
	eltType = type->foamANew.eltType;
	decl	= foamNewDecl(eltType, strCopy(""), emptyFormatSlot);

	emLocalUsage[n].remap = (Foam *) stoAlloc(OB_Other, size*sizeof(Foam));
		
	for(i = 0; i < size; i++) {
		emNewLocals = listCons(Foam)(foamCopy(decl), emNewLocals);
		emLocalUsage[n].remap[i] = foamNewLoc(emNumLocals++);
		emChanged = true;
	}
}

/*
 * Create new locals for the merged environment.
 */
local void
emMakeNewLocals(Foam prog)
{
	int		i;
	Foam		newFoam, locals;
	FoamList	l;

	locals = prog->foamProg.locals;
	newFoam = foamNewEmpty(FOAM_DDecl, emNumLocals + 1);
	newFoam->foamDDecl.usage = locals->foamDDecl.usage;

	for(i=0; i<foamDDeclArgc(locals); i++)
		newFoam->foamDDecl.argv[i] = locals->foamDDecl.argv[i];
	emNewLocals = listNReverse(Foam)(emNewLocals);
	for(l = emNewLocals, i = foamDDeclArgc(locals); l; l = cdr(l), i++)
		newFoam->foamDDecl.argv[i] = car(l);
	stoFree(locals);
	listFree(Foam)(emNewLocals);
	prog->foamProg.locals = newFoam;
}


/*
 * Add a statement to the current function being merged.
 */
local void
emAddStmt(Foam stmt)
{
	emStmtList = listCons(Foam)(stmt, emStmtList);
}

/*
 * Transform the statements in a function body to merged environments.
 */
local void
emMergeEnvs(Foam prog)
{
	Foam	body = prog->foamProg.body;
	int	argc = foamArgc(body), i;

	assert(foamTag(body) == FOAM_Seq);
	emStmtList = listNil(Foam);
	for(i=0; i<argc; i++)
		emAddStmt(emMergeExpr(body->foamSeq.argv[i]));
	foamFreeNode(body);
	prog->foamProg.body = emNewBody();
}

/*
 * Create a new function body from the statement list.
 */
local Foam
emNewBody()
{
	Foam		newFoam;
	FoamList	l;
	int		i;

	newFoam = foamNewEmpty(FOAM_Seq, listLength(Foam)(emStmtList));
	emStmtList = listNReverse(Foam)(emStmtList);
	for(i=0, l = emStmtList; l; l = cdr(l), i++) {
		newFoam->foamSeq.argv[i] = car(l);
	}
	listFree(Foam)(emStmtList);
	return newFoam;
}

/*
 * transform a foam expression to merge non-escaping environments.
 */
local Foam
emMergeExpr(Foam expr)
{
	Foam	nexpr = expr, env;
	int	index;

	foamIter(expr, arg, *arg = emMergeExpr(*arg));

	switch(foamTag(expr)) {
	case FOAM_Set:
	case FOAM_Def:
		nexpr = emMergeDef(expr);
		break;
	case FOAM_EEnv:
		nexpr = emMergeEEnv(expr);
		break;
	case FOAM_EElt:
		index = expr->foamEElt.lex;
		env   = expr->foamEElt.ref;
		nexpr = emMergeEElt(expr, env, index);
		break;
	case FOAM_RElt:
		index = expr->foamRElt.field;
		env   = expr->foamRElt.expr;
		nexpr  = emMergeRElt(expr, env, index);
		break;
	case FOAM_Return:
		nexpr = emMergeReturn(expr);
		break;
	case FOAM_AElt:
		nexpr = emMergeAElt(expr);
		break;
	case FOAM_Lex:
		nexpr = emMergeLex(expr);
		break;
	case FOAM_Free:
		nexpr = emMergeFree(expr);
		break;
	default:
		break;
	}
	/*	if (expr != nexpr) foamFreeNode(expr); */
	return nexpr;
}


/*
 * Transform an assignment.
 */
local Foam
emMergeDef(Foam def)
{
	Foam	lhs = def->foamDef.lhs;
	Foam	rhs = def->foamDef.rhs;
	FoamTag ltag = foamTag(lhs), rtag = foamTag(rhs);

	if (rtag == FOAM_PushEnv) {
		emSetParent(lhs, rhs->foamPushEnv.parent);
		return def;
	}

	if (rtag == FOAM_Loc && ltag == FOAM_Loc && emOldLocal(rhs) && emOldLocal(lhs)) {
		if (emUsage(lhs)->remap) {
			if (emUsage(rhs)->remap == 0) {
				assert(0 && "lhs is remapped and rhs isn't");
			}
		}
		else {
			if (emUsage(rhs)->remap) {
				assert (0 && "rhs is remapped and lhs isn't");
			}
		}
	}
	
	if (ltag == FOAM_Loc) {
		if (!emOldLocal(lhs)) return def;
		return emNewEnvSet(def, lhs, rhs);
	}

	return def;
}

/*
 * Transform an assignment.
 */
local Foam
emNewEnvSet(Foam def, Foam lhs, Foam rhs)
{
	int	format, i, size, rtag;
	Foam	ddecl;
	EmUsage aliasLhs;
	Bool	isArray;
	FoamTag	arrType = FOAM_NOp;  /* Quit warnings */

	assert (emOldLocal(lhs));

	foamDereferenceCast(rhs);

	rtag = foamTag(rhs);

	aliasLhs = emUsageAliasing(lhs);


	if (aliasLhs->used != EM_NonEscapingEnv) return def;

	/* we are setting a non-escaped or an alias */

	format = aliasLhs->format;
	if (emIsArray(aliasLhs->type)) {
		isArray = true;
		size = format;
		arrType = (aliasLhs->type)->foamANew.eltType;
	}
	else {
		isArray = false;
		ddecl  = emFormats[format];
		size   = foamDDeclArgc(ddecl);
	}

	if (rtag == FOAM_Loc && emOldLocal(rhs) && emUsage(rhs)->remap) {
		for(i=0; i<size; i++)
 		        emAddStmt(foamNewSet(foamCopy(emUsage(lhs)-> remap[i]),
					     foamCopy(emUsage(rhs)-> remap[i])));
		emChanged = true;
		return foamNewNOp();
	}
	if ( rtag == FOAM_PushEnv || rtag == FOAM_Env || rtag == FOAM_RNew || rtag == FOAM_ANew) {
	        assert(rtag != FOAM_PushEnv); /* has been dealt in emMergeDef */
		if (emUsage(lhs)->used == EM_NonEscapingEnv) return foamNewNOp();
		return def;
	}
	
	emAddStmt(def);

	if (isArray) {
		for(i=0; i<size; i++)
			emAddStmt(foamNewSet(foamCopy(emUsage(lhs)->remap[i]),
					     foamNewAElt(arrType,
							 foamNewSInt(i),
							 foamCopy(lhs))));
							 
	}
	else {
		for(i=0; i<size; i++)
			emAddStmt(foamNewSet(foamCopy(emUsage(lhs)->remap[i]),
					     foamNewRElt(format,
							 foamCopy(lhs), i)));
	}

	emChanged = true;
	return foamNewNOp();
}


/*
 * Transfor a record elt.
 */
local Foam
emMergeRElt(Foam expr, Foam env, int index)
{
	foamDereferenceCast(env);

	if (foamTag(env) != FOAM_Loc) return expr;

	if (!emOldLocal(env)) return expr;

	if (emUsageAliasing(env)->used != EM_NonEscapingEnv) return expr;

	emChanged = true;

	/* 	assert (expr->foamRElt.format == emUsageAliasing(env)->format); 
		no, because there may be a cast so the records don't have to be identical 
	*/
	assert (emUsage(env)->remap);

	return foamCopy(emUsage(env)->remap[index]);
}

/*
 * transform an environment elt.
 */
local Foam
emMergeEElt(Foam eelt, Foam env, int index)
{
	int		format = eelt->foamEElt.env;
	int		level  = eelt->foamEElt.level;
	int		oindex = eelt->foamEElt.lex;
	EmUsage		usage;

	foamDereferenceCast(env);

	env = emGetNthParent(env, &level);
	if (level == eelt->foamEElt.level && level > 0)
		return eelt;

	emChanged = true;

	if (level > 0)
		return foamNewEElt(format, foamCopy(env), level, oindex);

	if (!emIsMergeable(env))
		return foamNewEElt(format, foamCopy(env), level, oindex);

	if (!emOldLocal(env))
		return foamNewEElt(format, foamCopy(env), level, oindex);

	usage = emUsageAliasing(env);

	if (usage->used != EM_NonEscapingEnv)
		return foamNewEElt(format, foamCopy(env), level, oindex);

	assert(emUsage(env)->remap); 
	return foamCopy(emUsage(env)->remap[index]);
}

local Foam
emMergeLex(Foam lex)
{
	if (lex->foamLex.level != 0)
		return lex;
	if (emLocalUsage->used != EM_NonEscapingEnv)
		return lex;
	emChanged = true;
	return foamCopy(emLocalUsage->remap[lex->foamLex.index]);
}

/*
 * Transform an environment parent instruction.
 */
local Foam
emMergeEEnv(Foam eenv)
{
	Foam	env = eenv->foamEEnv.env;
	int	level = eenv->foamEEnv.level;

	foamDereferenceCast(env);

	env = emGetNthParent(env, &level);
	if (level == eenv->foamEEnv.level)
		return eenv;

	emChanged = true;
	env = foamCopy(env);
	assert((int)level >= 0); /* bug 1168 */
	return level == 0 ? env : foamNewEEnv(level, env);
}

/*
 * Remove the `(Free x)' statement if x is no more heap-allocated.
 */
local Foam
emMergeFree(Foam foam)
{
	Foam 	expr, newFoam;

	expr = foam->foamFree.place;

	foamDereferenceCast(expr);

	if (foamTag(expr) != FOAM_Loc) return foam;
	
	if (!emOldLocal(expr)) return foam;
	if (emUsageAliasing(expr)->used != EM_NonEscapingEnv) return foam;

	newFoam = foamNewNOp();

	return newFoam;
}

/*
 * create a new record object for returning.
 */
local Foam
emMergeReturn(Foam ret)
{
	int	size, format, i;
	Foam	newFoam, expr;

	foamDereferenceCast(ret);

	expr = ret->foamReturn.value;
	if (foamTag(expr) != FOAM_Loc) return ret;

	if (!emOldLocal(expr)) return ret;
	if (emUsageAliasing(expr)->used != EM_NonEscapingEnv) return ret;

	format = emUsageAliasing(expr)->format;
	size   = foamDDeclArgc(emFormats[format]);
	newFoam	   = foamNewEmpty(FOAM_Rec, size+1);
	newFoam->foamRec.format = format;
	for(i=0; i<size; i++)
		newFoam->foamRec.eltv[i+1] =
			foamCopy(emUsageAliasing(expr)->remap[i]);
	emChanged = true;

	return newFoam;
}

/*
 * Merge an array reference.
 */
local Foam
emMergeAElt(Foam aelt)
{
	Foam	index = aelt->foamAElt.index;
	Foam	array = aelt->foamAElt.expr;
	EmUsage usage;
	int	i;

	foamDereferenceCast(array);

	if (foamTag(array) != FOAM_Loc) return aelt;
	if (!emOldLocal(array)) return aelt;

	usage = emUsageAliasing(array);
	if (usage->used != EM_NonEscapingEnv) return aelt;

	assert (emIsSInt(index));
	i = emSIntValue(index);
	return foamCopy(usage->remap[i]);
}

/*
 * Return the representative usage descriptor for an environment.
 */
local EmUsage
emUsageAliasing(Foam foam)
{
	EmUsage		orig, val;
	Bool            isCircular ;
	
	isCircular = 0;

	if (foamTag(foam) == FOAM_Loc)
		val = emUsageFromLocalIndex(foam->foamLoc.index);
	else {
		assert(foamTag(foam) == FOAM_Env);
		val = emLocalUsage;	/* for Env(0) */
	}

	orig = val;
	while (val->link) {
		if (val->mark == 1) {
			isCircular =1 ; /* oops , must be a circular link */
			break; 
		}
		val->mark = 1;
		val = val->link;

	}
	while (orig->link) {
		if (orig->mark == 0) break;
		orig->mark = 0;
		orig = orig->link;
	}
	return val;
}


local EmUsage
emUsage(Foam foam)
{
	assert(foam!=NULL);
	if (foamTag(foam) == FOAM_Loc)
	        return emUsageFromLocalIndex(foam->foamLoc.index);
	else {
		assert(foamTag(foam) == FOAM_Env);
		return emLocalUsage;	/* for Env(0) */
	}
}


local EmUsage
emUsageFromLocalIndexAliasing(long index)
{
	EmUsage		orig, val;
	Bool            isCircular ;
	
	isCircular = 0;

	val = emUsageFromLocalIndex(index);

	orig = val;
	/* follow the links all the way , marking as we go */
	while (val->link) {
		if (val->mark == 1) {
			isCircular =1 ; /* oops , must be a circular link */
			break; 
		}
		val->mark = 1;
		val = val->link;

	}
	/* restore the marks */
	while (orig->link) {
	        if (orig->mark == 0) break;
		orig->mark = 0;
		orig = orig->link;
	}
	return val;
}

local EmUsage
emUsageFromLocalIndex(long index)
{
        /* Loc 0 =>  index 0 => array[1] */
	return &(emLocalUsage[index + 1]);
}

#ifndef NDEBUG
local long
emIndexFromUsage(EmUsage u)
{
        return (u - &emLocalUsage[1]);
}
#endif

Table emParentTable;

/*
 * Create a hash table for tracking environment parents.
 */
local void
emNewParentTable()
{
	emParentTable = tblNew((TblHashFun) foamHash, (TblEqFun) foamEqual);
}

/*
 * Free the table.
 */
local void
emFreeParentTable()
{
	tblFree(emParentTable);
}

/*
 * Set an environment's parent
 */
local void
emSetParent(Foam child, Foam parent)
{
	Foam newParent;
	newParent = emUnEEnv(parent);
	
	/* Strip aliases out where poss */
	if (parent != newParent) {
	        emergeDEBUG({
			printf("child:"); foamWrSExpr(dbOut, child,SXRW_Default);
			printf("parent:"); foamWrSExpr(dbOut, parent,SXRW_Default);
			printf("newParent:"); foamWrSExpr(dbOut, newParent,SXRW_Default);
		});
		emChanged = true;
	}
	
	tblSetElt(emParentTable, (TblKey) child, (TblElt) newParent);
}


local Foam
emUnEEnv(Foam foam)
{
	int lvl;
	Foam env;
	
	if (foamTag(foam) != FOAM_EEnv)
		return foam;
	
	lvl  = foam->foamEEnv.level;
	env = foam->foamEEnv.env;
	
	while (lvl > 0) {
		Foam nextEnv = emGetParent(env);
		if (!nextEnv)
			return foam;
		env = nextEnv;
		lvl--;
	}
	return env;
}

/*
 * Query an environment's parent.
 */
local Foam
emGetParent(Foam child)
{
	return (Foam) tblElt(emParentTable, (TblKey) child, (TblElt) 0);
}

local Foam
emGetNthParent(Foam child, int *level)
{
	Foam	parent;

	while (*level > 0 && (parent = emGetParent(child)) != NULL) {
		child = parent;
		*level -= 1;
	}
	
	return child;
}


local void
emCleanTypeUsage() 
{ 
	int i;
	for (i = 0 ; i < emOrigNumLocals ;i++) {
		emUsageFromLocalIndex((long)i)->type = (Foam) 0 ;
	}
	
}


local void
emNormaliseUsage()
{ 
	int i;
        emergeDEBUG({ fprintf(stderr,"--------------------\n");})
		for (i = 0 ; i < emOrigNumLocals ;i++){
			EmUsage use = emUsageFromLocalIndex((long)i);
				
			/* If non-escaping and not root link ... */
			if (use->link) {
				if (use->link->used != EM_NonEscapingEnv) {
					/* assert(use->link->link == 0); */
					use->link   = 0; /* MUST clobber this */
				}
				else {
					use->used = EM_AliasNonEsc;
					assert(use->remap);
					emergeDEBUG ({
						fprintf(stderr, "%3ld is aliased to %3ld\n",
							emIndexFromUsage(use), emIndexFromUsage(use->link));
					})
						}
					
			}
		}
		
}



local void 
emCopyUsedTagsTo(EmUsageState * arr)
{
	int i;
	for (i = 0 ; i < (emOrigNumLocals + 1) ;i++){
		arr[i] = emLocalUsage[i].used;
	}
}

local Bool
emUsedTagsChanged(EmUsageState *arr)
{
	int i;
	for (i = 0 ; i < (emOrigNumLocals + 1) ;i++){
		if (arr[i] != emLocalUsage[i].used) {
			emergeDEBUG({
				fprintf(stderr,"emLocalUsage[%3d].used has changed!\n",i);
			})
			return true;
		}
	}
	return false;
}



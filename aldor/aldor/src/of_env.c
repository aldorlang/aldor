/*****************************************************************************
 *
 * of_env.c: Environment optimisation
 *
 * Copyright (c) 1990-2007 Aldor Software Organization Ltd (Aldor.org).
 *
 ****************************************************************************/

#include "axlobs.h"

/*
 * ToDo: Consider splitting environments, deleting "empty" environments, etc.
 */
Bool	oeDebug	= false;
#define oeDEBUG(s)	DEBUG_IF(oeDebug, s)

typedef enum { ProgUnknown, ProgForeign, ProgLocal } OeProgOrigin;
typedef struct oeProgInfo {
	Bool done;
	OeProgOrigin origin;
	int  parent;
	Foam oldDEnv;
	Foam newDEnv;
} OeProgInfo;

#define oeOldEnv(i)  (oeProgInfo[i].oldDEnv)
#define oeNewEnv(i)  (oeProgInfo[i].newDEnv)

static void 	oeFindEnvs	(Foam);
static void	oeCheckEnv	(Foam prog, int i);
static void 	oeCheckExpr	(Foam expr);
static void	oeCheckClos	(Foam, Foam);
static void	oeLexRef	(int target);
static void	oeEnvRef	(int target);

static void	oeRenewEnvs	(Foam unit);

static Foam 	   oeWholeUnit;
static OeProgInfo *oeProgInfo;
static int 	   oeThisProg;

void
oeUnit(Foam unit)
{
	Foam defs;
	int i;

	defs = unit->foamUnit.defs;
	oeProgInfo = (OeProgInfo*) stoAlloc(OB_Other, foamArgc(defs) * sizeof(OeProgInfo));
	oeWholeUnit = unit;
	
	for (i=0; i<foamArgc(defs); i++) {
		oeProgInfo[i].done    = false;
		oeProgInfo[i].origin  = ProgUnknown;
		oeProgInfo[i].parent  = -1;
		oeProgInfo[i].oldDEnv = NULL;
		oeProgInfo[i].newDEnv = NULL;
	}

	oeFindEnvs(unit);
	oeRenewEnvs(unit);

	stoFree(oeProgInfo);
	oeProgInfo = NULL;
}

static void
oeFindEnvs(Foam unit)
{
	Foam defs;
	int i;

	defs = unit->foamUnit.defs;

	for(i=0; i<1/*foamArgc(defs)*/; i++) {
		Foam	def = defs->foamDDef.argv[i];
		Foam	prog;
oeDEBUG(printf("Prog<<\n"));
oeDEBUG(foamWrSExpr(dbOut, def,SXRW_Default));
		assert(foamTag(def) == FOAM_Def);
		prog = def->foamDef.rhs;
		if (foamTag(prog) == FOAM_Prog)
			oeCheckEnv(prog, i);
oeDEBUG(printf("Prog>>\n"));
oeDEBUG(foamWrSExpr(dbOut, prog,SXRW_Default));
	}
}

static void
oeCheckEnv(Foam prog, int i)
{
	Foam env;
	int  oeOldProg;
	if (oeProgInfo[i].done)
		return;

	oeProgInfo[i].done = true;
	oeOldProg = oeThisProg;
	oeThisProg = i;
	env = prog->foamProg.levels;
	oeProgInfo[i].oldDEnv = env;
	env = foamCopy(env);
	oeProgInfo[i].newDEnv = env;
	oeProgInfo[i].done = true;

	for (i=0; i<foamArgc(env); i++)
		 env->foamDEnv.argv[i]= emptyFormatSlot;

	oeCheckExpr(prog->foamProg.body);
	oeThisProg = oeOldProg;
}

static void
oeCheckExpr(Foam expr)
{
	int i;
	switch (foamTag(expr)) {
	  case FOAM_Clos:
		oeCheckClos(expr->foamClos.env, expr->foamClos.prog);
		break;
	  case FOAM_OCall:
		oeCheckClos(expr->foamOCall.env, expr->foamOCall.op);
		for (i=0; i<foamArgc(expr) - 3; i++)
			oeCheckExpr(expr->foamOCall.argv[i]);
		return;
	  default:
		break;
	}

	foamIter(expr, arg, oeCheckExpr(*arg));
	
	switch (foamTag(expr)) {
	  case FOAM_Clos:
		break;
	  case FOAM_Lex:
		oeLexRef(expr->foamLex.level);
		break;
	  case FOAM_Env:
		oeEnvRef(expr->foamEnv.level);
		break;
	  default:
		break;
	}
}

static void
oeCheckClos(Foam env, Foam prog)
{
	Foam def;
	int pid, ppid;
	int depth;
	
	assert(foamTag(prog) == FOAM_Const);
	pid = prog->foamConst.index;

	if (foamTag(env) != FOAM_Env) {
		oeCheckExpr(env);
		if (oeProgInfo[pid].origin == ProgUnknown)
			oeProgInfo[pid].origin = ProgForeign;
		return;
	}
	oeProgInfo[pid].origin = ProgLocal;

	depth = env->foamEnv.level;
	ppid = oeThisProg;
	oeLexRef(depth);
	while (depth != 0) {
		ppid = oeProgInfo[ppid].parent;
		depth--;
	}
	assert(ppid >= 0);
	oeProgInfo[pid].parent = ppid;
	if (oeProgInfo[ppid].origin == ProgForeign)
		oeProgInfo[ppid].origin = ProgForeign;

	def = oeWholeUnit->foamUnit.defs->foamDDef.argv[pid];
	assert(foamTag(def) == FOAM_Def);
	oeCheckEnv(def->foamDef.rhs, pid);
}

static void
oeLexRef(int target)
{
	int i;
	int pid = oeThisProg;

	i = target;
	do {
		oeNewEnv(pid)->foamDEnv.argv[i] = oeOldEnv(pid)->foamDEnv.argv[i];
		pid = oeProgInfo[pid].parent;
		i--;
	} while (i >= 0 && pid != -1);


}

static void 
oeEnvRef(int target)
{
	int i;
	int pid = oeThisProg;

	for (i = target; i < foamArgc(oeOldEnv(pid)) - 1; i++) {
		oeLexRef(i);
	}
}


static void
oeRenewEnvs(Foam unit)
{
	Foam defs;
	int i;

	defs = unit->foamUnit.defs;
	for(i=0; i<foamArgc(defs); i++) {
		Foam	def = defs->foamDDef.argv[i];
		Foam	prog;
		assert(foamTag(def) == FOAM_Def);
		prog = def->foamDef.rhs;
		if (foamTag(prog) == FOAM_Prog
		    && oeProgInfo[i].origin == ProgLocal) {
			oeDEBUG({
				printf("Old %d:\n", i);
				foamPrintDb(oeOldEnv(i));
				printf("New %d:\n", i);
				foamPrintDb(oeNewEnv(i));
				if (!foamEqual(oeOldEnv(i), oeNewEnv(i)))
					foamPrintDb(prog);
			});
			foamFree(prog->foamProg.levels);
			prog->foamProg.levels = oeNewEnv(i);
		}
		
	}
}

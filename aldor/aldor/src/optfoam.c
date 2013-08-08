/*****************************************************************************
 *
 * optfoam.c: Foam-to-foam optimizaion.
 *
 * Copyright (c) 1990-2007 Aldor Software Organization Ltd (Aldor.org).
 *
 ****************************************************************************/

#include "axlobs.h"
#include "debug.h"
#include "flatten.h"
#include "flog.h"
#include "format.h"
#include "inlutil.h"
#include "loops.h"
#include "of_argsub.h"
#include "of_cfold.h"
#include "of_comex.h"
#include "of_cprop.h"
#include "of_deada.h"
#include "of_deadv.h"
#include "of_emerg.h"
#include "of_env.h"
#include "of_hfold.h" 
#include "of_inlin.h"
#include "of_jflow.h"
#include "of_killp.h"
#include "of_peep.h"
#include "of_retyp.h"
#include "of_rrfmt.h"
#include "optfoam.h"
#include "store.h"
#include "strops.h"

Bool	optfDebug = false;

#define optfDEBUG	DEBUG_IF(optfDebug)

static int optInline;
static int optInlineAll;
static int optJFlowLimit;
static int optInlineLimit;
int optInlineRoof;
static int optConstFold;
static int optFloatFold;
static int optHashFold;
static int optDeadVar;
static int optDeadAssign;
static int optPeepHole;
static int optCommExp;
static int optEnvMerge;
static int optEnvOpts;
static int optCopyProp;
static int optJumpFlow;
static int optCast;
static int optCC;
static int optArgSub;
static int optCcFnonstd;
static int optIgnoreAsserts;
static int optKillPointers;
static int optCleanStack;
static int optEmergeRRFmt;

#define OPT_Negate		"no-"
#define	OPT_All	        	"all"
#define OPT_InlineAll		"inline-all"

#define	OPT_MaxQLevel		9
#define	OPT_MaxLevel		4
#define OPT_DefaultLevel	1

static int optLevel = OPT_DefaultLevel;

enum optNature {
	OPT_FLAG,
	OPT_FLOAT
};

struct optControl {
	String		name;
	enum optNature	nature;
	int		*pvar;
	int		value[OPT_MaxLevel+1];
};

/******************************************************************************
 *
 * :: Local operations
 *
 *****************************************************************************/

local void      optInitFormatters();

local  void	optSetAllTo		(Bool);
local  void	optSetLevel		(int);
local  void 	optPrintOpts		(FILE *fout);
local  void	optSetJFlowLimit	(void);
local  void	optOptimizationsInit	(void);

	
/*
 * Control Option processing: note that -Qinline-limit and -Qinline-size
 * arguments supplied by the user are multiplied by 100.
 */
struct optControl	optControl[] = {
/* name		nature	  pvar		       Q0   Q1    Q2    Q3    Q4. */
{"inline",	OPT_FLAG,  &optInline,	      { 0,  0,    1,    1,    1}},
{OPT_InlineAll,	OPT_FLAG,  &optInlineAll,     { 0,  0,    0,    1,    1}},
{"inline-limit",OPT_FLOAT, &optInlineLimit,   { 0,  0,  500,  600,  800}},
{"inline-size",	OPT_FLOAT, &optInlineRoof,    { 0,  0, 1000, 2000, 4000}},
{"cfold",	OPT_FLAG,  &optConstFold,     { 0,  1,    1,    1,    1}},
{"ffold",	OPT_FLAG,  &optFloatFold,     { 0,  0,    1,    1,    1}},
{"hfold",	OPT_FLAG,  &optHashFold,      { 0,  1,    1,    1,    1}},
{"deadvar",	OPT_FLAG,  &optDeadVar,	      { 0,  1,    1,    1,    1}},
{"dassign",	OPT_FLAG,  &optDeadAssign,    { 0,  0,    1,    1,    1}},
{"peep",	OPT_FLAG,  &optPeepHole,      { 0,  1,    1,    1,    1}},
{"cprop",	OPT_FLAG,  &optCopyProp,      { 0,  0,    1,    1,    1}},
{"cse",		OPT_FLAG,  &optCommExp,	      { 0,  0,    1,    1,    1}},
{"env",		OPT_FLAG,  &optEnvOpts,	      { 0,  0,    1,    1,    1}},
{"emerge",	OPT_FLAG,  &optEnvMerge,      { 0,  0,    1,    1,    1}},
{"emerge-rr",  	OPT_FLAG,  &optEmergeRRFmt,   { 0,  0,    1,    1,    1}},
{"flow",	OPT_FLAG,  &optJumpFlow,      { 0,  0,    1,    1,    1}},
{"cast",	OPT_FLAG,  &optCast,	      { 0,  0,    1,    1,    1}},
{"cc",		OPT_FLAG,  &optCC,	      { 0,  0,    1,    1,    1}},
{"del-assert",	OPT_FLAG,  &optIgnoreAsserts, { 0,  0,    1,    1,    1}},
{"cc-fnonstd",  OPT_FLAG,  &optCcFnonstd,     { 0,  0,    0,    0,    0}},
/* The next three are experimental or future-work */
{"killp",  	OPT_FLAG,  &optKillPointers,  { 0,  0,    0,    0,    0}},
{"cstack",  	OPT_FLAG,  &optCleanStack,    { 0,  0,    0,    0,    0}},
{"argsub",  	OPT_FLAG,  &optArgSub,        { 0,  0,    0,    0,    0}},
{ 0 }
};

/* optInlineLimit for            Q5   Q6    Q7     Q8   Q9   */
static int optQInlineLimit[] = {1000, 1400, 1800, 3000,  -1};

/*
 * Set all flags to the given value.
 */
local void
optSetAllTo(Bool flag)
{
	int	i;

	for (i = 0; optControl[i].name; i++)
		if (optControl[i].nature == OPT_FLAG)
			*optControl[i].pvar = flag;
}

/*
 * Set all flags and control variables to the values suitable for the
 * given optimization level.
 */
local void
optSetLevel(int lev)
{
	int	i, index;

        optLevel = lev;
#if OLD_SET_LEVEL
	optInlineLimit = optQInlineLimit[lev - OPT_MaxLevel -1];
	index = (lev > OPT_MaxLevel ? OPT_MaxLevel : lev);

	for (i = 0; optControl[i].name; i++)
		*optControl[i].pvar = optControl[i].value[index];
#else
	index = (lev > OPT_MaxLevel ? OPT_MaxLevel : lev);

	for (i = 0; optControl[i].name; i++)
		*optControl[i].pvar = optControl[i].value[index];

	if (lev > OPT_MaxLevel)
		optInlineLimit = optQInlineLimit[lev - OPT_MaxLevel - 1];
#endif
}

local void 
optPrintOpts(FILE *fout)
{
	int i;
	fprintf(fout, "Optimizations selected:\n");
	for (i = 0; optControl[i].name; i++)
		fprintf(fout, "%15s %d\n", optControl[i].name, *optControl[i].pvar);
}

void
optInit(void)
{
	optSetLevel(OPT_DefaultLevel);
	optInitFormatters();
}

int
optSetStdOptimization(void)
{
	return optSetOptimization("2");
}

void
optSetLoopOption(void)
{
	inlSetGenerators();
}

int
optSetOptimization(String opt)
{
	int	i;
	String	s;
	Bool	isOn;

	/* LEVELS: -Qn for n in 0..OPT_MaxLevel */
	i = opt[0] - '0';
	if (!opt[1] && 0 <= i && i <= OPT_MaxQLevel) {
		optSetLevel(i);
		return 0;
	}
	
	/* INTEGERS: -Qxxx=nnn or -Qxxx:nnn */
	for (i = 0; optControl[i].name; i++) {
		if (optControl[i].nature != OPT_FLOAT) continue;
		if ((s = strAIsPrefix(optControl[i].name, opt)) == 0) continue;
		if (*s != '=' && *s != ':') continue;

		s += 1;
		*optControl[i].pvar = (int) (100 * atof(s));
		return 0;
	}

	/* FLAGS: -Qxxx -Qno-xxx ... */
	isOn = true;
	while (s = strAIsPrefix(OPT_Negate, opt), s) {
		isOn = !isOn;
		opt  = s;
	}

	if (strAEqual(opt, OPT_All)) {
		optSetAllTo(isOn);
		return 0;
	}
	if (strEqual(opt, OPT_InlineAll)) {
		optInlineAll = isOn;
		if (isOn) optInline = isOn;
		return 0;
	}

	for (i = 0; optControl[i].name; i++) {
		if (!strEqual(opt, optControl[i].name)) continue;
		if (optControl[i].nature != OPT_FLAG) continue;
		*optControl[i].pvar = isOn;
		return 0;
	}

	/* FAILED */
	return -1;
}

Foam
optimizeFoam(Foam foam)
{
	Bool	newConsts = false;
	int	i, iters;
	optfDEBUG{optPrintOpts(dbOut);}

	optOptimizationsInit();

	if (optDeadVar)   {
		optfDEBUG{fprintf(dbOut, "Starting deadvar...\n");}
		dvElim(foam);
		DEBUG{stoAudit();}
	}
	if (optInline) 	  {
		optfDEBUG{fprintf(dbOut, "Starting inline...\n");}
		inlineUnit(foam, optInlineAll, optInlineLimit, true);
		DEBUG{stoAudit();}
	}
	/* Maybe we ought to cprop before cfold? */
	if (optConstFold || optFloatFold) {
		optfDEBUG{fprintf(dbOut, "Starting cfold...\n");}
		newConsts = cfoldUnit(foam, optConstFold, optFloatFold);
		DEBUG{stoAudit();}
	}
	while (newConsts && optInline) {
		optfDEBUG{fprintf(dbOut, "Starting expr inline...\n");}
		/* If const folding made new constants, inline them. */
		inlineUnit(foam, optInlineAll, optInlineLimit, false);
		newConsts = cfoldUnit(foam, optConstFold, optFloatFold);
		DEBUG{stoAudit();}
	}
	if (optHashFold) {
		optfDEBUG{fprintf(dbOut, "Starting hfold...\n");}
		hfoldUnit(foam);
		cpropUnit(foam, false);
		cfoldUnit(foam, optConstFold, optFloatFold);
		DEBUG{stoAudit();}
	}
	if (optEmergeRRFmt)  {
		optfDEBUG{fprintf(dbOut, "Starting emerge-rr...\n");}
		rrUnitEmerge(foam);
		DEBUG{stoAudit();}
	}
	if (optEnvMerge)  {
		optfDEBUG{fprintf(dbOut, "Starting emerge...\n");}
		emMergeUnit(foam);
		DEBUG{stoAudit();}
	}
	if (optDeadVar)   {
		optfDEBUG{fprintf(dbOut, "Starting deadvar...\n");}
		dvElim(foam);
		DEBUG{stoAudit();}
	}
	if (optEnvOpts) {
		optfDEBUG{fprintf(dbOut, "Starting env. opts...\n");}
		oeUnit(foam);
		DEBUG{stoAudit();}
	}
	if (optCast) {
		optfDEBUG{fprintf(dbOut, "Starting retype...\n");}
		retypeUnit(foam);
		DEBUG{stoAudit();}
	}

	if (optLevel > 5)
	    	iters = 5;
	else if (optLevel == 0)
		iters = 1;
	else
		iters = optLevel;

	for (i=0; i<iters; i++) {
	optfDEBUG{fprintf(dbOut, "STARTING LOOP (%d)\n", i);}

		optfDEBUG{foamWrSExpr(dbOut, foam, int0);}
#if 0
		if (optInline) 	  {
			optfDEBUG{fprintf(dbOut, "Starting inline...\n");}
			inlineUnit(foam, optInlineAll, optInlineLimit, true);
			DEBUG{stoAudit();}
		}
#endif
		if (optCopyProp)  {
			optfDEBUG{fprintf(dbOut, "Starting cprop...\n");}
			cpropUnit(foam, i == 0);
			DEBUG{stoAudit();}
		}
		if (optPeepHole)  {
			optfDEBUG{fprintf(dbOut, "Starting peep...\n");}
			peepUnit(foam,optFloatFold);
			DEBUG{stoAudit();}
		}
		if (optConstFold || optFloatFold) {
			optfDEBUG{fprintf(dbOut, "Starting cfold...\n");}
			newConsts = cfoldUnit(foam, optConstFold,optFloatFold);
			DEBUG{stoAudit();}
		}
		if (optCommExp)  {
			optfDEBUG{fprintf(dbOut, "Starting cse...\n");}
			cseUnit(foam);
			DEBUG{stoAudit();}
		}
#if 0
		while (newConsts && optInline) {
			optfDEBUG{fprintf(dbOut, "Starting expr inline...\n");}
			/* If const folding made new constants, inline them. */
			inlineUnit(foam, optInlineAll, optInlineLimit, false);
			newConsts = cfoldUnit(foam, optConstFold, optFloatFold);
			DEBUG{stoAudit();}
		}
#endif
#if 0
		if (optArgSub)  {
			optfDEBUG{fprintf(dbOut, "Starting argsub...\n");}
			argsubUnit(foam);
			DEBUG{stoAudit();}
		}
#endif
		if (optJumpFlow)  {
			optfDEBUG{fprintf(dbOut, "Starting jflow...\n");}
	       		jflowUnit(foam, optJFlowLimit);
			DEBUG{stoAudit();}
		}
		if (optDeadAssign) {
			optfDEBUG{fprintf(dbOut, "Starting dead assign...\n");}
			deadAssign(foam);
			DEBUG{stoAudit();}
		}
	
		if (optDeadVar)   {
			optfDEBUG{fprintf(dbOut, "Starting deadvar...\n");}
			dvElim(foam);
			DEBUG{stoAudit();}
		}
#if 0
		if (optEnvMerge)  {
			optfDEBUG{fprintf(dbOut, "Starting emerge...\n");}
			emMergeUnit(foam);
			DEBUG{stoAudit();}
		}
#endif
	}

	if (optEnvOpts) {
		optfDEBUG{fprintf(dbOut, "Starting env. opts...\n");}
		oeUnit(foam);
		DEBUG{stoAudit();}
	}

	if (optPeepHole)  {
		optfDEBUG{fprintf(dbOut, "Starting peep...\n");}
		peepUnit(foam,optFloatFold);
		DEBUG{stoAudit();}
	}

	optfDEBUG{fprintf(dbOut, "(Starting patchUnit...)\n");}

	fpPatchUnit(foam);

	/*
 	* inlSetAfterInline turns on checking for domainGetExport
 	* when using -W runtime.
 	*/
	inlSetAfterInline();

	if (optInline) {
		optfDEBUG{fprintf(dbOut, "(Starting unitInfoRefresh...)\n");}
		inuUnitInfoRefresh(foam);
	}

	/* Remove any nested CCalls, etc */
	/*flattenUnit(foam);*/

	optfDEBUG{fprintf(dbOut, "Optimizations finished.\n");}

	foamAuditAll(foam, 0xffff);

	assert(foamAudit(foam));

	return foam;
}

/* This seems reasonable. Note: assumes that with -Q3 limit=600 */
local void
optSetJFlowLimit(void)
{
	if (optInlineLimit > -1)
		optJFlowLimit = optInlineLimit / 20;
	else
		optJFlowLimit = 30000;
}

Bool
optIsCcOptimizeWanted(void)
{
	return optCC;
}

Bool
optIsCcFNonStdWanted(void)
{
	return optCcFnonstd;
}

Bool 
optIsFloatFoldWanted(void)
{
  return optFloatFold;
}

Bool
optIsIgnoreAssertsWanted(void)
{
	return optIgnoreAsserts;
}

Bool
optIsDeadVarWanted(void)
{
	return optDeadVar;
}

Bool
optIsKillPointersWanted(void)
{
	return optKillPointers;
}

Bool
optIsCleanStackWanted(void)
{
	return optCleanStack;
}

Bool
optIsMaxLevel(void)
{
	return optLevel == OPT_MaxQLevel;
}

/* Put here a every call needed to initialize an optimization. */
local void
optOptimizationsInit()
{
	dvInit();
	optSetJFlowLimit();
}

/*
 * :: Formatters
 */
local int flogFormatter(OStream stream, Pointer p);
local int loopFormatter(OStream stream, Pointer p);
local int bbFormatter(OStream stream, Pointer p);
local int bbufFormatter(OStream stream, Pointer p);
local int bbListFormatter(OStream stream, Pointer p);
local int loopListFormatter(OStream stream, Pointer p);

local void
optInitFormatters()
{
	fmtRegisterFull("FlowGraph", flogFormatter, false);

	fmtRegisterFull("Loop", loopFormatter, false);
	fmtRegisterFull("LoopList", loopListFormatter, true);

	fmtRegisterFull("BBlock", bbFormatter, false);
	fmtRegisterFull("BBlockList", bbListFormatter, true);

	fmtRegisterFull("BlockBuf", bbufFormatter, false);
	
}

local int
flogFormatter(OStream stream, Pointer p)
{
	FlowGraph flog = (FlowGraph) p;
	char *sep;
	int c=0;
	int i;
	
	c += ostreamPrintf(stream, "(Flog:");
	for (i=0; i < flogBlockC(flog); i++) {
		c += ostreamPrintf(stream, " %pBBlock", flogBlock(flog, i));
	}
	c += ostreamPrintf(stream, ")");

	return c;
}

local int
loopFormatter(OStream stream, Pointer p)
{
	Loop loop = (Loop) p;
	int c;

	c = ostreamPrintf(stream, "[L: %d %pBBlockList]", loop->header->label, loop->blockList);
	
	return c;
}

local int
bbFormatter(OStream stream, Pointer p)
{
	BBlock bb = (BBlock) p;
	int c;
	c = ostreamPrintf(stream, "[B %s %d In: %pBlockBuf Out: %pBlockBuf]", 
			  bb->kind < FOAM_LIMIT ? foamStr(bb->kind) : "???",
			  bb->label, bb->entries, bb->exits);
	return c;
}

local int
bbufFormatter(OStream stream, Pointer p)
{
	BlockBuf bf = (BlockBuf) p;
	int c = 0;
	int i;
	char *sep = "";

	ostreamWriteChar(stream, '(');
	c++;
	for (i=0; i < bbufCount(bf); i++) {
		BBlock bb = bbufBlock(bf, i);
		c += ostreamPrintf(stream, "%s%d", sep, bb->label);
		sep = " ";
	}
	ostreamWriteChar(stream, ')');
	c++;

	return c;
}

local int
loopListFormatter(OStream ostream, Pointer p)
{
	LoopList list = (LoopList) p;
	return listFormat(Loop)(ostream, "Loop", list);
}
local int
bbListFormatter(OStream ostream, Pointer p)
{
	BBlockList list = (BBlockList) p;
	return listFormat(BBlock)(ostream, "BBlock", list);
}

#include "axlobs.h"
#include "debug.h"
#include "gf_syme.h"
#include "stab.h"
#include "store.h"
#include "sefo.h"
#include "lib.h"

static Length	gen0SymeTableC;
static Syme	*gen0SymeTableV;

static Length	gen0DefSymeTableC;
static Syme 	*gen0DefSymeTableV;

/*
 * This whole file is a bit ugly - we've got an array of stuff that is
 * needed to sefo.c can form closures over syme graphs.  The array is
 * lazily populated by genfoam.  Splitting it out to here means that
 * there isn't a dependency from syme.c to genfoam.c.
 */


void
gen0InitConstTable(int maxScobindDef)
{
	Length	i;
/* Old way */	
	gen0SymeTableC = stabMaxDefnNum() + 1;
	gen0SymeTableV = (Syme *) stoAlloc((unsigned)OB_Other,
					   gen0SymeTableC * sizeof(Syme));

	for (i = 0; i < gen0SymeTableC; i += 1)
		gen0SymeTableV[i] = NULL;
/* New way */
	gen0DefSymeTableC = maxScobindDef;
	gen0DefSymeTableV = (Syme *) stoAlloc((unsigned)OB_Other,
					       gen0SymeTableC * sizeof(Syme));

	for (i = 0; i < gen0DefSymeTableC; i += 1)
		gen0DefSymeTableV[i] = NULL;
}

/*
 * This function may be invoked during genfoam as well as during
 * the optimisation phases. During genfoam we never want to change
 * the const number associated with a given syme: if we try to do
 * so, it means that the syme is conditional with more than one
 * implementation and isn't "const". During optimisation however,
 * we must allow the const num to be modified.
 *
 *  unique => changing const num means clobber it
 * ~unique => changing const num works
 */
void
genSetConstNum(Syme syme, int defineIdx, UShort index, Bool unique)
{
	Length	dindex = symeDefnNum(syme);
	ULong	cnum = symeConstNum(syme); /* 0 <= cnum <= 0xffff */


	/* Old way */
	assert(dindex < gen0SymeTableC);

	/*
	 * Conditional symes with multiple implementations are not
	 * const. We detect this here if we see a syme whose const
	 * num is already set and isn't `index', and `unique' is
	 * true. Conditional symes with one possible implementation
	 * are okay: if the type checker allowed the call then we
	 * can inline it.
	 */
	if (unique && cnum <= 0x3fff && cnum != index) {
		/*
		 * Conditional syme with multiple implementations.
		 * We have to record the fact that we have stomped
		 * on the stored const num otherwise the next time
		 * we got here we would think the syme was okay.
		 */
		symeClrConstNum(syme);
		symeSetMultiCond(syme);
		gen0SymeTableV[dindex] = (Syme)NULL;
	}
	else if (!symeIsMultiCond(syme)) {
		symeSetConstNum(syme, (int) index);
		gen0SymeTableV[dindex] = syme;
	}


	/* New way */
#if 0	
	SImpl   nimpl;
	if (defineIdx != -1) {
		gen0DefSymeTableV[defineIdx] = syme;
	}
	if (symeImpl(syme)) {
		implSetConstNum(symeImpl(syme), defineIdx, index);
	}
#endif
}

Bool
genHasConstNum(Syme syme)
{
	Length	dindex;
	
	if (symeExtension(syme))
		syme = symeExtension(syme);

	if (symeHasConstNum(syme) && 
	    symeConstLib(syme) != NULL)
		return true;

	dindex = symeDefnNum(syme);
	
	if (0 < dindex && dindex < gen0SymeTableC && gen0SymeTableV[dindex]) {
		symeSetConstNum(syme, symeConstNum(gen0SymeTableV[dindex]));
		symeSetHashNum(syme,  symeHashNum(gen0SymeTableV[dindex]));
		symeSetDVMark(syme,   symeDVMark(gen0SymeTableV[dindex]));
	}

	return symeHasConstNum(syme);
}

UShort
genGetConstNum(Syme syme)
{
	return symeConstNum(syme);
}

void
genGetConstNums(SymeList symes)
{
	for (; symes; symes = cdr(symes))
		genHasConstNum(car(symes));
}

void
gen0KillSymeConstNums()
{
	Length	i;

	for (i = 0; i < gen0SymeTableC; i += 1)
		if (gen0SymeTableV[i])
			symeClrConstNum(gen0SymeTableV[i]);
}

SymeList
genGetSymeInlined(Syme syme)
{
	Length	dindex;

	if (symeExtension(syme)) syme = symeExtension(syme);

	dindex = symeDefnNum(syme);

	if (dindex < gen0SymeTableC && gen0SymeTableV[dindex])
		return symeInlined(gen0SymeTableV[dindex]);
	else
		return listNil(Syme);
}

void
genKillOldSymeConstNums(int generation)
{
	Length	i;
	
	for (i = 0; i < gen0SymeTableC; i += 1) {
		Syme syme = gen0SymeTableV[i];
		if (!syme) continue;
		if (symeDVMark(syme) < generation)
			symeClrConstNum(syme);
	}
}

/*****************************************************************************
 *
 * :: Compile-time debugging facilities for GDB.
 *
 ****************************************************************************/

/*
 * This is a debugging function which allows the contents
 * of gen0SymeTable to be seen. This table provides a mapping
 * between constant numbers and definition numbers.
 */
void
symeTablePrintDb(void)
{
	int	i;
	Syme	syme;

	for (i = 0;i < gen0SymeTableC;i++)
        {
		syme = gen0SymeTableV[i];
		if (syme)
		{
			AInt c = (AInt)-1;
			AInt d = symeDefnNum(syme);

			if (symeHasConstNum(syme))
				c = symeConstNum(syme);

			(void)fprintf(dbOut, "[%2d]: ", i);
			(void)fprintf(dbOut, " (Const %2d, ", (int)c);
			(void)fprintf(dbOut, " Defn %2d) ", (int)d);
			symePrintDb(syme);
		}
        }
}


/****
 ****/
#define otSymeConstSetEnvIndep(s)  symeSetConstFlag(s, 0)
#define otSymeConstClrEnvIndep(s)  symeClrConstFlag(s, 0)
#define otSymeConstEnvIndep(s)     symeConstFlag(s, 0)


/******************************************************************************
 *
 * :: otIsConstSyme
 *
 *****************************************************************************/

local Bool		otSefoIsParameterized	(Sefo);

Bool
otIsConstSyme(Syme syme)
{
	Bool	result = true;

	if (syme == NULL || symeHasDefault(syme))
		result = false;

	else if (symeIsImport(syme)) {
		TForm	tf = symeExporter(syme);
		if (tfIsGeneral(tf) && otSefoIsParameterized(tfExpr(tf)))
			result = false;
	}

	return result;
}

/* Return true if a leaf of sefo has a symbol meaning
 * which is a SYME_Param.
 */
local Bool
otSefoIsParameterized(Sefo sefo)
{
	if (abIsLeaf(sefo)) {
		Syme	syme = abSyme(sefo);
		assert(syme);
		if (symeIsParam(syme))
			return true;
	}
	else {
		Length	i;
		for (i = 0; i < abArgc(sefo); i += 1)
			if (otSefoIsParameterized(abArgv(sefo)[i]))
				return true;
	}

	return false;
}

Bool 
otSymeIsFoamConst(Syme syme)
{
	TForm tf;
	Sefo  sf;

	if (syme == NULL)
		return false;

	if (symeHasDefault(syme))
		return false;

	if (!otSymeConstEnvIndep(syme))
		return false;

	if (!symeIsImport(syme) && !symeIsExport(syme))
		return false;

	if (symeIsExport(syme))
		return true;

	tf = symeExporter(syme);

	if (!tfIsGeneral(tf))
		return true;
	
	sf = tfExpr(tf);
	if (abIsLeaf(sf))
		return otIsConstSyme(abSyme(sf));
	else
		return !otSefoIsParameterized(sf->abApply.op);
}

void
otTransferFoamInfoToSyme(Syme syme, Foam unit)
{
	Foam def, prog;
	int cNum = symeConstNum(syme);
	
	if (symeConstLib(syme) == NULL &&
	    cNum != SYME_NUMBER_UNASSIGNED) {
		def = unit->foamUnit.defs->foamDDef.argv[cNum];
		prog = def->foamDef.rhs;
		if (foamTag(prog) == FOAM_Prog && 
		    foamProgHasNoEnvUse(prog))
			otSymeConstSetEnvIndep(syme);
		else
			otSymeConstClrEnvIndep(syme);
	}
}

void
otTransferFoamInfo(SymeList symes, Foam foam)
{
	genGetConstNums(symes);

	for ( ; symes; symes = cdr(symes))
		otTransferFoamInfoToSyme(car(symes), foam);

}

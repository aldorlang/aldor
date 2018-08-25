
#include "formatters.h"
#include "axlobs.h"
#include "syme.h"
#include "symeset.h"
#include "freevar.h"
#include "bigint.h"
#include "java/javacode.h"
#include "ostream.h"
#include "format.h"
#include "sefo.h"
#include "tposs.h"
#include "strops.h"
#include "errorset.h"
#include "tconst.h"
#include "ttable.h"

local int tfFormatter(OStream stream, Pointer p);
local int tfListFormatter(OStream stream, Pointer p);

local int tpossFormatter(OStream stream, Pointer p);
local int fvFormatter(OStream stream, Pointer p);

local int tconstFormatter(OStream stream, Pointer p);

local int symeFormatter(OStream stream, Pointer p);
local int symeSetFormatter(OStream stream, Pointer p);
local int symeListFormatter(OStream stream, Pointer p);
local int symeListListFormatter(OStream stream, Pointer p);

local int symeConditionFormatter(OStream stream, Pointer p);
local int symeConditionListFormatter(OStream stream, Pointer p);

local int tsetFormatter(OStream stream, Pointer p);

local int ptrFormatter(OStream stream, Pointer p);
local int ptrListFormatter(OStream stream, Pointer p);

local int aintFormatter(OStream stream, Pointer p);
local int aintListFormatter(OStream stream, Pointer p);

local int stringFormatter(OStream stream, Pointer p);
local int stringListFormatter(OStream stream, Pointer p);

local int bintFormatter(OStream stream, Pointer p);
local int symbolFormatter(OStream stream, Pointer p);

local int errorSetFormatter(OStream stream, Pointer p);
local int javaCodeFormatter(OStream stream, Pointer p);


void
fmttsInit()
{
	fmtRegister("TForm", tfFormatter);
	fmtRegister("TFormList", tfListFormatter);

	fmtRegister("FreeVar", fvFormatter);
	fmtRegister("TPoss", tpossFormatter);

	fmtRegister("TConst", tconstFormatter);

	fmtRegister("Syme", symeFormatter);
	fmtRegister("SymeSet", symeSetFormatter);
	fmtRegister("SymeList", symeListFormatter);
	fmtRegister("SymeListList", symeListListFormatter);

	fmtRegister("SymeC", symeConditionFormatter);
	fmtRegister("SymeCList", symeConditionListFormatter);

	fmtRegister("TSet", tsetFormatter);

	fmtRegister("Ptr", ptrFormatter);
	fmtRegister("PtrList", ptrListFormatter);

	fmtRegister("AInt", aintFormatter);
	fmtRegister("AIntList", aintListFormatter);

	fmtRegister("String", stringFormatter);
	fmtRegister("StringList", stringListFormatter);

	fmtRegister("BInt", bintFormatter);
	fmtRegister("Symbol", symbolFormatter);

	fmtRegister("ErrorSet", errorSetFormatter);
	fmtRegister("JavaCode", javaCodeFormatter);
}


/*
 * :: Formatted output
 */

local int
symeFormatter(OStream ostream, Pointer p)
{
	int c;

	c = symeOStreamWrite(ostream, p);

	return c;
}

local int
symeConditionFormatter(OStream ostream, Pointer p)
{
	Syme syme = (Syme) p;
	int c;

	c = symeOStreamWrite(ostream, syme);
	c += listFormat(AbSyn)(ostream, "AbSyn", (AbSynList) symeCondition(syme));

	return c;
}

local int
tsetFormatter(OStream ostream, Pointer p)
{
	PointerTSet tset = (PointerTSet) p;
	PointerTSetIter iter;
	String sep = "";
	int c = 0;

	c += ostreamWrite(ostream, "{", -1);
	for (iter = tsetIter(Pointer)(tset);
	     tsetIterHasNext(Pointer)(iter);
	     iter = tsetIterNext(Pointer)(iter)) {
		c += ostreamWrite(ostream, sep, -1);
		c += ptrFormatter(ostream, tsetIterElt(Pointer)(iter));
		sep = ", ";
	}

	c += ostreamWrite(ostream, ")", -1);
	return c;
}

local int
ptrFormatter(OStream ostream, Pointer p)
{
	char buf[20];
	int c;
	
	sprintf(buf, "%p", p);
	c = ostreamWrite(ostream, buf, -1);

	return c;
}

local int
aintFormatter(OStream ostream, Pointer p)
{
	char buf[20];
	int c;

	sprintf(buf, AINT_FMT, (AInt) p);
	c = ostreamWrite(ostream, buf, -1);

	return c;
}


local int
stringFormatter(OStream ostream, Pointer p)
{
	String string = (String) p;
	return ostreamWrite(ostream, string, -1);
}


local int
bintFormatter(OStream ostream, Pointer p)
{
	String s = bintToString((BInt) p);
	int c = ostreamWrite(ostream, s, -1);
	strFree(s);

	return c;
}

local int
symbolFormatter(OStream ostream, Pointer p)
{
	String s = symString((Symbol) p);
	int c = ostreamWrite(ostream, s, -1);

	return c;
}


local int
tfFormatter(OStream ostream, Pointer p)
{
	int c;

	c = tformOStreamWrite(ostream, p);

	return c;
}

local int
fvFormatter(OStream ostream, Pointer p)
{
	FreeVar fv = (FreeVar) p;
	int c;

	c = ostreamPrintf(ostream, "[FV: %pSymeList]", fvSymes(fv));

	return c;
}

local int
tpossFormatter(OStream ostream, Pointer p)
{
	TPoss tp = (TPoss) p;
	int c;

	c = tpossOStreamWrite(ostream, tp);

	return c;
}

local int
errorSetFormatter(OStream ostream, Pointer p)
{
	ErrorSet errorSet = (ErrorSet) p;
	int i;

	i = ostreamPrintf(ostream, "[E: %pStringList]", errorSet->list);

	return i;
}

local int
javaCodeFormatter(OStream ostream, Pointer p)
{
	JavaCode jco = (JavaCode) p;
	int c;

	c = ostreamPrintf(ostream, "%pSExpr", jcoSExpr(jco));

	return c;
}
local int
tconstFormatter(OStream ostream, Pointer p)
{
	TConst tc = (TConst) p;
	int i;

	i = ostreamPrintf(ostream, "[TC: %pTForm %pTForm]", tc->argv[0], tc->argv[1]);

	return i;
}

local int
symeSetFormatter(OStream ostream, Pointer p)
{
	SymeSet symeSet = (SymeSet) p;
	return symeSetFormat(ostream, symeSet);
}

local int
tfListFormatter(OStream ostream, Pointer p)
{
	TFormList list = (TFormList) p;
	return listFormat(TForm)(ostream, "TForm", list);
}

local int
symeListFormatter(OStream ostream, Pointer p)
{
	SymeList list = (SymeList) p;
	return listFormat(Syme)(ostream, "Syme", list);
}

local int
symeListListFormatter(OStream ostream, Pointer p)
{
	SymeListList list = (SymeListList) p;
	return listFormat(SymeList)(ostream, "SymeList", list);
}

local int
symeConditionListFormatter(OStream ostream, Pointer p)
{
	SymeList list = (SymeList) p;
	return listFormat(Syme)(ostream, "SymeC", list);
}

local int
ptrListFormatter(OStream ostream, Pointer p)
{
	AbSynList list = (AbSynList) p;
	return listFormat(AbSyn)(ostream, "Ptr", list);
}

local int
aintListFormatter(OStream ostream, Pointer p)
{
	AIntList list = (AIntList) p;
	return listFormat(AInt)(ostream, "AInt", list);
}

local int
stringListFormatter(OStream ostream, Pointer p)
{
	StringList list = (StringList) p;
	return listFormat(String)(ostream, "String", list);
}

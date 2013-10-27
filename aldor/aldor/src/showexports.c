#include <stdlib.h>

#include "axlobs.h"
#include "absyn.h"
#include "token.h"
#include "stab.h"
#include "tform.h"
#include "tqual.h"
#include "tinfer.h"
#include "srcpos.h"
#include "srcline.h"
#include "scan.h"
#include "linear.h"
#include "parseby.h"
#include "abnorm.h"
#include "macex.h"
#include "syscmd.h"
#include "stab.h"
#include "symbol.h"
#include "syme.h"
#include "abpretty.h"
#include "format.h"
#include "comsg.h"
#include "scobind.h"
#include "abuse.h"
#include "opsys.h"
#include "ablogic.h"
#include "sexpr.h"
#include "spesym.h"
#include "debug.h"
#include "optfoam.h"

local AbSyn shexpParse(String);

/*
 * Usage: showexports libName type-expression
 * Example: showexports libaldor.al 'List(Integer)'
 */
int
main(int argc, char *argv[])
{
	osInit();
	sxiInit();
	keyInit();
	ssymInit();
	dbInit();
	stabInitGlobal();
	tfInit();
	foamInit();
	optInit();
	tinferInit();
	pathInit();

	sposInit();
	ablogInit();
	comsgInit();

	macexInitFile();
	comsgInit();
	scobindInitFile();
	stabInitFile();

	fileAddLibraryDirectory(".");

	String archive = argv[1];
	String expression = argv[2];

	scmdHandleLibrary("LIB", archive);

	AbSyn ab = shexpParse(expression);
	Stab stab = stabFile();
	Syme syme = stabGetArchive(symInternConst("LIB"));
	AbSyn arAbSyn = abNewId(sposNone, symInternConst("LIB"));
	AbSyn boolean = abNewId(sposNone, symInternConst("Boolean"));

	stabImportTForm(stab, tiGetTForm(stab, arAbSyn));
	stabImportTForm(stab, tiGetTForm(stab, boolean));
	abPutUse(ab, AB_Use_Value);
	scopeBind(stab, ab);
	typeInfer(stab, ab);
	TForm tf = tiGetTForm(stab, ab);
	aprintf("Type: %s Cat: %d\n", tfPretty(tf), tfSatCat(tf));
	if (tfSatDom(tf)) {
		SymeList list = tfGetCatExports(tf);
		aprintf("Category\n");
		for (; list != listNil(Syme); list = cdr(list)) {
			Syme syme = car(list);
			aprintf("%5s %3d %s %pAbSynList\n", symeString(syme), symeHasDefault(syme),
				tfPretty(symeType(syme)), symeCondition(syme));
		}
	}
	else {
		aprintf(">>> Exports\n");
		SymeList list = tfStabGetDomImports(stab, tf);

		for (; list != listNil(Syme); list = cdr(list)) {
			Syme syme = car(list);
			aprintf("%s %d %d %s\n", symeString(syme),
				symeDefnNum(syme), symeConstNum(syme), tfPretty(symeType(syme)));
		}

		TQualList tqList;
		aprintf(">>> Cascades\n");
		tqList = tfGetDomCascades(tf);

		for (; tqList != listNil(TQual); tqList = cdr(tqList)) {
			TQual tq = car(tqList);
			aprintf("--> %pTForm\n", tfPretty(tqBase(tq)));
		}
	}

	scobindFiniFile();
	stabFiniFile();
	comsgFini();
	macexFiniFile();

}

local AbSyn
shexpParse(String txt)
{
	AbSyn ab;
	TokenList tl;
	SrcLineList sll;
	SrcLine srcLine = slineNew(sposNone, 0, txt);

	sll = listList(SrcLine)(1, srcLine);
	tl = scan(sll);
	tl = linearize(tl);
	ab = parse(&tl);
	ab = abNormal(ab, false);
	ab = macroExpand(ab);
	ab = abNormal(ab, true);

	return ab;
}

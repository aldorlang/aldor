#include "ablogic.h"
#include "abnorm.h"
#include "abquick.h"
#include "abuse.h"
#include "comsg.h"
#include "format.h"
#include "linear.h"
#include "macex.h"
#include "parseby.h"
#include "scan.h"
#include "scobind.h"
#include "sefo.h"
#include "srcline.h"
#include "stab.h"
#include "strops.h"
#include "symbol.h"
#include "testlib.h"
#include "tinfer.h"
#include "ti_sef.h"

local AbSyn abqParseSrcLines(SrcLineList sll);

ABQK_DEFINE0(sequence0, abNewSequence0);
ABQK_DEFINE1(sequence1, abNewSequence1);
ABQK_DEFINE2(sequence2, abNewSequence2);
ABQK_DEFINE0(comma0, abNewComma0);
ABQK_DEFINE1(comma1, abNewComma1);
ABQK_DEFINE2(comma2, abNewComma2);
ABQK_DEFINE0(nothing, abNewNothing);
ABQK_DEFINE2(define, abNewDefine);
ABQK_DEFINE2(declare, abNewDeclare);
ABQK_DEFINE2(with, abNewWith);
ABQK_DEFINE2(add, abNewAdd);
ABQK_DEFINE2(label, abNewLabel);
ABQK_DEFINE2(has, abNewHas);
ABQK_DEFINE2(apply1, abNewApply1);
ABQK_DEFINE3(apply2, abNewApply2);
ABQK_DEFINE3(lambda, abNewLambda);
ABQK_DEFINE3(_if0, abNewIf);
ABQK_DEFINE2(import, abNewImport);
ABQK_DEFINE2(qualify, abNewQualify);
ABQK_DEFINE2(pretend, abNewPretendTo);
ABQK_DEFINE2(restrictTo, abNewRestrictTo);
ABQK_DEFINE1(test, abNewTest);
ABQK_DEFINE2(and, abNewAnd);

ABQK_DEFINE1_Symbol(id, abNewId);

AbSyn
_if(AbSyn testPart, AbSyn thenPart, AbSyn elsePart)
{
	return _if0(test(testPart), thenPart, elsePart);
}


AbSyn emptyAdd() 
{
	return add(nothing(), nothing());
}

AbSyn emptyWith() 
{
	return with(nothing(), nothing());
}

AbSyn typeDecl(String name)
{
	return declare(id(name), id("Type"));
}

AbSyn
defineUnary(String name, AbSyn param, AbSyn retType, AbSyn rhs)
{
	AbSyn theLambda = lambda(comma1(abCopy(param)),
				 abCopy(retType),
				 label(id(name), rhs));
	AbSyn theDefine = define(declare(id(name),
					 apply2(id("->"), abCopy(param),
						abCopy(retType))),
			   theLambda);
	
	abFree(retType);
	abFree(param);

	return theDefine;
}


AbSyn
abqParse(String txt)
{
	SrcLine srcLine = slineNew(sposNone, 0, txt);

	return abqParseSrcLines(listList(SrcLine)(1, srcLine));
}

AbSyn
abqScopeBind(String txt)
{
	SrcLine srcLine = slineNew(sposNone, 0, txt);

	AbSyn ab = abqParseSrcLines(listList(SrcLine)(1, srcLine));
	scopeBind(stabFile(), ab);
	return ab;
}

local AbSyn
abqParseSrcLines(SrcLineList sll)
{
	AbSyn ab;
	TokenList tl;

	tl = scan(sll);
	tl = linearize(tl);
	ab = parse(&tl);
	ab = abNormal(ab, false);
	ab = macroExpand(ab);
	ab = abNormal(ab, true);

	return ab;
}

AbSyn
abqParseLinesAsSeq(StringList lines)
{
	SrcLineList sll = listNil(SrcLine);

	while (lines != listNil(String))
	{
		String tmp;
		SrcLine line;
		int indent = 0;
		char *p = car(lines);
		lines = cdr(lines);
		while (*p == ' ') { p++; indent++; }
		tmp = strConcat(p, "\n");
		line = slineNew(sposNone, indent, tmp);
		strFree(tmp);
		sll = listCons(SrcLine)(line, sll);
	}

	return abqParseSrcLines(listNReverse(SrcLine)(sll));
}

AbSynList
abqParseLines(StringList lines)
{
	AbSynList result = listNil(AbSyn);
	while (lines != listNil(String)) {
		result = listCons(AbSyn)(abqParse(car(lines)), result);
		lines = listFreeCons(String)(lines);
	}
	return listNReverse(AbSyn)(result);
}



AbSyn
stdtypes()
{
	String Type_txt = "Type: with == add";
	String Category_txt = "Category: with == add";
	String Tuple_txt = "Tuple(T: Type): with == add";
	String Cross_txt = "Cross(T: Tuple Type): with == add";
	String Generator_txt = "Generator(T: Type): with == add";
	String Map_txt = "(->)(A: Tuple Type, R: Tuple Type): with == add";
	String Boolean_txt = "Boolean: with == add";
	String Join_txt = "Join(T: Tuple Category): Category == with";
	String Record_txt = "Record(T: Tuple Type): with == add";
	String Enumeration_txt = "Enumeration(T: Tuple Type): with == add";

	StringList lines = listList(String)(10, Type_txt, Category_txt, Cross_txt,
					    Tuple_txt, Map_txt, Boolean_txt, Join_txt,
					    Generator_txt, Record_txt, Enumeration_txt);
	AbSynList code = abqParseLines(lines);
	AbSyn absyn = abNewSequenceL(sposNone, code);

	return absyn;
}

void
stdscope(Stab stab)
{
	String B_imp = "import from Boolean";
	StringList lines = listList(String)(1, B_imp);
	AbSyn absyn = abNewSequenceL(sposNone, listCons(AbSyn)(stdtypes(),
							       abqParseLines(lines)));
	abPutUse(absyn, AB_Use_NoValue);
	abPrintDb(absyn);
	scopeBind(stab, absyn);
	typeInfer(stab, absyn);

	testTrue("Declare is sefo", abIsSefo(absyn));
	testIntEqual("Error Count", 0, comsgErrorCount());
}

TForm
tfqTypeForm(Stab stab, String str)
{
	AbSyn absyn = abqParse(str);
	abPutUse(absyn, AB_Use_NoValue);
	abPrintDb(absyn);
	scopeBind(stab, absyn);
	typeInfer(stab, absyn);

	testTrue("Declare is sefo", abIsSefo(absyn));
	testIntEqual("Error Count", 0, comsgErrorCount());

	return tiGetTForm(stab, absyn);
}

Sefo
tfqTypeInfer(Stab stab, String str)
{
	int nErrors = comsgErrorCount();
	AbSyn absyn = abqParse(str);
	abPutUse(absyn, AB_Use_NoValue);

	scopeBind(stab, absyn);
	typeInfer(stab, absyn);

	testTrue("Declare is sefo", abIsSefo(absyn));
	testIntEqual("Error Count", nErrors, comsgErrorCount());

	return (Sefo) absyn;
}

void
tfqTypeInferFails(Stab stab, String str)
{
	AbSyn absyn = abqParse(str);
	int nErrors = comsgErrorCount();
	abPutUse(absyn, AB_Use_NoValue);

	scopeBind(stab, absyn);
	typeInfer(stab, absyn);

	testTrue("Error produced", comsgErrorCount() > nErrors);
}

Syme
uniqueMeaning(Stab stab, String s)
{
	SymeList symesForString = stabGetMeanings(stab, ablogTrue(), symIntern(s));
	Syme d;

	testIsNull("", cdr(symesForString));

	d = car(symesForString);

	return d;
}

Sefo
sefo(AbSyn absyn)
{
	tiSefo(stabFile(), absyn);
	return (Sefo) absyn;
}

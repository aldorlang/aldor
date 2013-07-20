#include "abnorm.h"
#include "abquick.h"
#include "linear.h"
#include "macex.h"
#include "parseby.h"
#include "scan.h"
#include "stab.h"
#include "ablogic.h"

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
ABQK_DEFINE1(test, abNewTest);

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
	
	StringList lines = listList(String)(8, Type_txt, Category_txt, Cross_txt,
					    Tuple_txt, Map_txt, Boolean_txt, Join_txt, 
					    Generator_txt);
	AbSynList code = abqParseLines(lines);
	AbSyn absyn = abNewSequenceL(sposNone, code);

	return absyn;
}

/* XXX: from test_ostream.c */
void testNull(void);

Syme
uniqueMeaning(Stab stab, String s)
{
	SymeList symesForString = stabGetMeanings(stab, ablogTrue(), symIntern(s));
        // FIXME: what is this supposed to do?
#if 0
	testNull("", cdr(symesForString));
#endif
	Syme d = car(symesForString);

	return d;
}

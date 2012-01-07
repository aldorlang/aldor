#include "abquick.h"
#include "scan.h"
#include "parseby.h"
#include "abnorm.h"
#include "macex.h"

ABQK_DEFINE0(sequence0, abNewSequence0);
ABQK_DEFINE1(sequence1, abNewSequence1);
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
	ab = parse(&tl);
	ab = abNormal(ab, false);
	ab = macroExpand(ab);
	ab = abNormal(ab, true);

	return ab;
}

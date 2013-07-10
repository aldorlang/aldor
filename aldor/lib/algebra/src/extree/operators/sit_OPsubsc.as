---------------------------   sit_OPsubsc.as   ------------------------------
-- Copyright (c) Manuel Bronstein 1999
-- Copyright (c) INRIA 1999, Version 0.1.12
-- Logiciel Sum^it ©INRIA 1999, dans sa version 0.1.12
-----------------------------------------------------------------------------

#include "algebra"
#include "algebrauid"

macro {
	TEXT == TextWriter;
	TREE == ExpressionTree;
	SI   == MachineInteger;
}

#if ALDOC
\thistype{ExpressionTreeSubscript}
\History{Manuel Bronstein}{5 November 99}{created}
\Usage{import from \this}
\Descr{\this~is the subscript operator for expression trees.}
\begin{exports}
\category{\astype{ExpressionTreeOperator}}\\
\end{exports}
#endif
   
ExpressionTreeSubscript: ExpressionTreeOperator == add
{
	import from String, TREE;

	name:Symbol             == -"__";
	arity:SI                == 2;
	uniqueId:SI             == UID__SUBSCRIPT;
	texParen?(p:SI):Boolean == false;

	aldor  (p:TEXT, l:List TREE): TEXT == INFIX2(aldor,p,l,"____");
	axiom   (p:TEXT, l:List TREE): TEXT == INFIX2(axiom,p,l,"__");
	C       (p:TEXT, l:List TREE): TEXT == INFIX2(C,p,l,"__");
	fortran (p:TEXT, l:List TREE): TEXT == INFIX2(fortran,p,l,"__");
	lisp	(p:TEXT, l:List TREE): TEXT == INFIX2(lisp,p,l,"__");

	maple   (p:TEXT, l:List TREE): TEXT == {
		maple(p, first l);
		maple(p << "[", first rest l);
		p << "]";
	}

	infix   (p:TEXT, l:List TREE): TEXT == {
		p := infix(p, first l) << "__";
		a := first rest l;
		notlf? := ~leaf? a;
		if notlf? then p := p << "(";
		p := infix(p, a);
		notlf? => p << ")";
		p;
	}

	tex     (p:TEXT, l:List TREE): TEXT == {
		tex(p, first l);
		tex(p << "__{", first rest l);
		p << "}";
	}
}


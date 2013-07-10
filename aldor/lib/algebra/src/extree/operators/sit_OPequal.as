------------------------------   sit_OPequal.as   ------------------------------
-- Copyright (c) Marco Codutti 1995
-- Copyright (c) INRIA 1999, Version 0.1.12
-- Logiciel Sum^it ©INRIA 1999, dans sa version 0.1.12
-- Copyright (c) Swiss Federal Polytechnic Institute Zurich, 1995-97
-----------------------------------------------------------------------------

#include "algebra"
#include "algebrauid"

macro {
	TEXT == TextWriter;
	TREE == ExpressionTree;
	SI   == MachineInteger;

	NOTYET(a,b) == p << a << " output for " << b << " not yet implemented.";
}

#if ALDOC
\thistype{ExpressionTreeEqual}
\History{Marco Codutti}{12 June 95}{created}
\Usage{import from \this}
\Descr{\this~is the {\em equal} operator for expression trees.}
\begin{exports}
\category{\astype{ExpressionTreeOperator}}\\
\end{exports}
\begin{remarks}
The following functions are not yet implemented and produce dummy results :
{\bf C}, {\bf fortran}.
\end{remarks}
#endif
   
ExpressionTreeEqual: ExpressionTreeOperator == add
{
	import from String, TREE;

	name:Symbol             == -"=";
	arity:SI                == 2;
	uniqueId:SI             == UID__EQUAL;
	texParen?(p:SI):Boolean == true;

	aldor   (p:TEXT, l:List TREE): TEXT == INFIX2(aldor,p,l,name);
	axiom   (p:TEXT, l:List TREE): TEXT == INFIX2(axiom,p,l,name);
	C       (p:TEXT, l:List TREE): TEXT == NOTYET("C",name);
	fortran (p:TEXT, l:List TREE): TEXT == NOTYET("fortran",name);
	maple   (p:TEXT, l:List TREE): TEXT == INFIX2(maple,p,l,name);
	tex     (p:TEXT, l:List TREE): TEXT == INFIX2(tex,p,l,name);
	infix   (p:TEXT, l:List TREE): TEXT == INFIX2(infix,p,l,name);

	lisp(p:TEXT, l:List TREE): TEXT == {
		import from ExpressionTreeOperatorTools;
		lisp(p, "equal", l);
	}
}

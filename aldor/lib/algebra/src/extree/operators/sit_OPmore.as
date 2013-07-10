------------------------------   sit_OPmore.as   ------------------------------
-- Copyright (c) Manuel Bronstein 1995
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
\thistype{ExpressionTreeGreaterEqual}
\History{Manuel Bronstein}{15 December 97}{created}
\Usage{import from \this}
\Descr{\this~is the {\em greater or equal} operator for expression trees.}
\begin{exports}
\category{\astype{ExpressionTreeOperator}}\\
\end{exports}
\begin{remarks}
The following functions are not yet implemented and produce dummy results :
{\bf C}, {\bf fortran}, {\bf lisp}.
\end{remarks}
#endif
   
ExpressionTreeGreaterEqual: ExpressionTreeOperator == add
{
	import from String, TREE;

	name:Symbol             == -">=";
	arity:SI                == 2;
	uniqueId:SI             == UID__MOREEQUAL;
	texParen?(p:SI):Boolean == true;

	aldor  (p:TEXT, l:List TREE): TEXT == INFIX2(aldor,p,l,">=");
	axiom   (p:TEXT, l:List TREE): TEXT == INFIX2(axiom,p,l,">=");
	C       (p:TEXT, l:List TREE): TEXT == NOTYET("C",">=");
	fortran (p:TEXT, l:List TREE): TEXT == NOTYET("fortran",">=");
	maple   (p:TEXT, l:List TREE): TEXT == INFIX2(maple,p,l,">=");
	tex     (p:TEXT, l:List TREE): TEXT == INFIX2(tex,p,l,"\ge ");
	lisp	(p:TEXT, l:List TREE): TEXT == NOTYET("lisp", ">=");
	infix   (p:TEXT, l:List TREE): TEXT == INFIX2(infix,p,l," >= ");
}

#if ALDOC
\thistype{ExpressionTreeGreaterThan}
\History{Manuel Bronstein}{15 December 97}{created}
\Usage{import from \this}
\Descr{\this~is the {\em greater than} operator for expression trees.}
\begin{exports}
\category{\astype{ExpressionTreeOperator}}\\
\end{exports}
\begin{remarks}
The following functions are not yet implemented and produce dummy results :
{\bf C}, {\bf fortran}, {\bf lisp}.
\end{remarks}
#endif

ExpressionTreeGreaterThan: ExpressionTreeOperator == add
{
	import from String, TREE;

	name:Symbol             == -">";
	arity:SI                == 2;
	uniqueId:SI             == UID__MORETHAN;
	texParen?(p:SI):Boolean == true;

	aldor  (p:TEXT, l:List TREE): TEXT == INFIX2(aldor,p,l,">");
	axiom   (p:TEXT, l:List TREE): TEXT == INFIX2(axiom,p,l,">");
	C       (p:TEXT, l:List TREE): TEXT == NOTYET("C",">");
	fortran (p:TEXT, l:List TREE): TEXT == NOTYET("fortran",">");
	maple   (p:TEXT, l:List TREE): TEXT == INFIX2(maple,p,l,">");
	tex     (p:TEXT, l:List TREE): TEXT == INFIX2(tex,p,l,">");
	lisp	(p:TEXT, l:List TREE): TEXT == NOTYET("lisp", ">");
}

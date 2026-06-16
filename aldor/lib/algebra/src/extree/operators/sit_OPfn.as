-----------------------------   sit_OPand.as   ------------------------------
-- Copyright (c) Marco Codutti 1995
-- Copyright (c) INRIA 1999, Version 0.1.12
-- Logiciel Sum^it �INRIA 1999, dans sa version 0.1.12
-- Copyright (c) Swiss Federal Polytechnic Institute Zurich, 1995-97
-----------------------------------------------------------------------------

#include "algebra"
#include "algebrauid"

macro {
	TEXT == TextWriter;
	TREE == ExpressionTree;
	SI   == MachineInteger;

	NOTYET(a,b) == p << a << " output for " << b << " not yet implemented.";
	FALSE == (t:TREE):Boolean +-> false;
	TRUE == (t:TREE):Boolean +-> true;
}

#if ALDOC
\thistype{ExpressionTreeApply}
\History{Peter Broadbery}{26 June 19}{created}
\Usage{import from \this}
\Descr{\this~is the function apply operator for expression trees.}
\begin{exports}
\category{\astype{ExpressionTreeOperator}}\\
\end{exports}
\begin{remarks}
The following functions are not yet implemented and produce dummy results :
{\bf C}, {\bf fortran}.
\end{remarks}
#endif

ExpressionTreeFunctionApply: ExpressionTreeOperator == add
{
	import from String, TREE, ExpressionTreeOperatorTools;
        import from TEXT;

	name:Symbol             == -"apply";
	arity:SI                == -1;
	uniqueId:SI             == UID__APPLY;
	texParen?(p:SI):Boolean == true;

	aldor   (p:TEXT, l:List TREE): TEXT == binary(p,l,leaf?, FALSE, aldor);
	axiom   (p:TEXT, l:List TREE): TEXT == binary(p,l,leaf?, FALSE, axiom);
	C       (p:TEXT, l:List TREE): TEXT == infix (p,l,TRUE,C, "&");
	fortran (p:TEXT, l:List TREE): TEXT == NOTYET("fortran",name);
	maple   (p:TEXT, l:List TREE): TEXT == binary(p,l,leaf?, FALSE, maple);
	tex     (p:TEXT, l:List TREE): TEXT == binary(p,l,leaf?, FALSE, tex);
	infix   (p:TEXT, l:List TREE): TEXT == binary(p,l,leaf?, FALSE, infix);

        binary(p: TEXT, l: List TREE, lparen?: TREE -> Boolean, rparen?: TREE -> Boolean, step:(TEXT,TREE) -> TEXT): TEXT == {
	    lp := false;
	    if lp then p := p << "(";
            p := step(p, first l);
            if lp then p := p << ")";
            p := p << "(";
            step(p, first rest l) << ")";
        }
}

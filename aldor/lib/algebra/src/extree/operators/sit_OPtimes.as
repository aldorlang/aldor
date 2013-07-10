-------------------------- sit_OPtimes.as --------------------------
-- Copyright (c) Manuel Bronstein 1995
-- Copyright (c) INRIA 1999, Version 0.1.12
-- Logiciel Sum^it ©INRIA 1999, dans sa version 0.1.12
-- Copyright (c) Swiss Federal Polytechnic Institute Zurich, 1995-97
-----------------------------------------------------------------------------

#include "algebra"
#include "algebrauid"

#if ALDOC
\thistype{ExpressionTreeTimes}
\History{Manuel Bronstein}{21/11/94}{created}
\Usage{import from \this}
\Descr{\this~is the multiplication operator for expression trees.}
\begin{exports}
\category{\astype{ExpressionTreeOperator}}\\
\end{exports}
#endif

macro {
	Z	== MachineInteger;
	TEXT	== TextWriter;
	TREE 	== ExpressionTree;
}

ExpressionTreeTimes:ExpressionTreeOperator == add {
        import from String, TREE, ExpressionTreeOperatorTools;

	name:Symbol			== -"*";
	arity:Z				== -1;
	uniqueId:Z			== UID__TIMES;
	texParen?(p:Z):Boolean		== p > TEXPREC__TIMES;
	axiom(p:TEXT, l:List TREE):TEXT	== trav(p, l, axiom);
	aldor(p:TEXT,l:List TREE):TEXT	== trav(p, l, aldor);
	maple(p:TEXT, l:List TREE):TEXT	== trav(p, l, maple);
	C(p:TEXT, l:List TREE):TEXT	== trav(p, l, C);
	fortran(p:TEXT,l:List TREE):TEXT== trav(p, l, fortran);

	infix(p:TEXT, l:List TREE):TEXT   ==
		infix(p, l, (t:TREE):Boolean +-> texParen?(TEXPREC__TIMES, t),
			infix, "*", "(", ")");

	tex(p:TEXT, l:List TREE):TEXT   ==
		infix(p, l, (t:TREE):Boolean +-> texParen?(TEXPREC__TIMES, t),
			tex, "\,", "\left(", "\right)");

	local trav(p:TEXT, l:List TREE, f:(TEXT, TREE) -> TEXT):TEXT ==
		infix(p, l, (t:TREE):Boolean +-> true, f, "*");

}

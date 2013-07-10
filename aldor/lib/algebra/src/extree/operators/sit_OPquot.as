-------------------------- sit_OPquot.as --------------------------
-- Copyright (c) Manuel Bronstein 1995
-- Copyright (c) INRIA 1999, Version 0.1.12
-- Logiciel Sum^it ©INRIA 1999, dans sa version 0.1.12
-- Copyright (c) Swiss Federal Polytechnic Institute Zurich, 1995-97
-----------------------------------------------------------------------------

#include "algebra"
#include "algebrauid"

#if ALDOC
\thistype{ExpressionTreeQuotient}
\History{Manuel Bronstein}{21/11/94}{created}
\Usage{import from \this}
\Descr{\this~is the division operator for expression trees.}
\begin{exports}
\category{\astype{ExpressionTreeOperator}}\\
\end{exports}
#endif

macro {
	Z	== MachineInteger;
	TEXT	== TextWriter;
	TREE 	== ExpressionTree;
}

ExpressionTreeQuotient:ExpressionTreeOperator == add {
	import from String, TREE, ExpressionTreeOperatorTools;

	name:Symbol			== -"/";
	arity:Z				== 2;
	uniqueId:Z			== UID__DIVIDE;
	texParen?(p:Z):Boolean		== p > TEXPREC__DIVIDE;
	axiom(p:TEXT, l:List TREE):TEXT	== trav(p, l, axiom);
	aldor(p:TEXT,l:List TREE):TEXT	== trav(p, l, aldor);
	maple(p:TEXT, l:List TREE):TEXT	== trav(p, l, maple);
	fortran(p:TEXT,l:List TREE):TEXT== trav(p, l, fortran);
	C(p:TEXT,l:List TREE):TEXT	== trav(p, l, C);

	infix(p:TEXT, l:List TREE):TEXT == {
		notlf? := ~leaf?(a := first l);
		if notlf? then p := p << "(";
		p := infix(p, a);
		if notlf? then p := p << ")";
		p := p << " / ";
		notlf? := ~leaf?(a := first rest l);
		if notlf? then p := p << "(";
		p := infix(p, a);
		notlf? => p := p << ")";
		p;
	}

	tex(p:TEXT, l:List TREE):TEXT == {
		--tex(tex(p << "\frac{", first l) << "}{", first rest l) << "}";
		tex(tex(p << "{{",first l) << "} \over {",first rest l) << "}}";
	}

	local trav(p:TEXT, l:List TREE, f:(TEXT, TREE) -> TEXT):TEXT ==
		infix(p, l, (t:TREE):Boolean +-> true, f, "/");

}

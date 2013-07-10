-------------------------- sit_OPexpt.as --------------------------
-- Copyright (c) Manuel Bronstein 1995
-- Copyright (c) INRIA 1999, Version 0.1.12
-- Logiciel Sum^it ©INRIA 1999, dans sa version 0.1.12
-- Copyright (c) Swiss Federal Polytechnic Institute Zurich, 1995-97
-----------------------------------------------------------------------------

#include "algebra"
#include "algebrauid"

#if ALDOC
\thistype{ExpressionTreeExpt}
\History{Manuel Bronstein}{21/11/94}{created}
\Usage{import from \this}
\Descr{\this~is the exponentiation operator for expression trees.}
\begin{exports}
\category{\astype{ExpressionTreeOperator}}\\
\end{exports}
#endif

macro {
	Z	== MachineInteger;
	TEXT	== TextWriter;
	TREE 	== ExpressionTree;
}

ExpressionTreeExpt:ExpressionTreeOperator == add {
	import from String, TREE, ExpressionTreeOperatorTools;

	name:Symbol			== -"^";
	arity:Z				== 2;
	uniqueId:Z			== UID__EXPT;
	texParen?(p:Z):Boolean		== p > TEXPREC__EXPT;
	axiom(p:TEXT, l:List TREE):TEXT	== trav(p, l, "^", axiom);
	aldor(p:TEXT,l:List TREE):TEXT	== trav(p, l, "^", aldor);
	maple(p:TEXT, l:List TREE):TEXT	== trav(p, l, "^", maple);
	fortran(p:TEXT,l:List TREE):TEXT== trav(p, l, "**", fortran);
	lisp(p:TEXT, l:List TREE):TEXT	== lisp(p, "expt", l);

	infix(p:TEXT, l:List TREE):TEXT == {
		paren? := texParen?(TEXPREC__EXPT, a := first l);
		if paren? then p := p << "(";
		p := infix(p, a);
		if paren? then p := p << ")";
		p := p << "^";
		exponent := first rest l;
		notlf? := ~leaf? exponent;
		if notlf? then p := p << "(";
		p := infix(p, exponent);
		notlf? => p << ")";
		p;
	}

	tex(p:TEXT, l:List TREE):TEXT == {
		paren? := texParen?(TEXPREC__EXPT, a := first l);
		if paren? then p := p << "\left(";
		p := tex(p, a);
		if paren? then p := p << "\right)";
		tex(p << "^{", first rest l) << "}";
	}

	local trav(p:TEXT, l:List TREE, s:String, f:(TEXT,TREE) -> TEXT):TEXT ==
		infix(p, l, (t:TREE):Boolean +-> true, f, s);


	C(p:TEXT, l:List TREE):TEXT ==
		C(C(p << "pow(", first l) << ",", first rest l) << ")";

}

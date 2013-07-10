-------------------------- sit_OPplus.as --------------------------
-- Copyright (c) Manuel Bronstein 1995
-- Copyright (c) INRIA 1999, Version 0.1.12
-- Logiciel Sum^it (c) INRIA 1999, dans sa version 0.1.12
-- Copyright (c) Swiss Federal Polytechnic Institute Zurich, 1995-97
-----------------------------------------------------------------------------

#include "algebra"
#include "algebrauid"

#if ALDOC
\thistype{ExpressionTreePlus}
\History{Manuel Bronstein}{21/11/94}{created}
\Usage{import from \this}
\Descr{\this~is the addition operator for expression trees.}
\begin{exports}
\category{\altype{ExpressionTreeOperator}}\\
\end{exports}
#endif

macro {
	Z	== MachineInteger;
	TEXT	== TextWriter;
	TREE 	== ExpressionTree;
}

ExpressionTreePlus:ExpressionTreeOperator == add {
	import from String, TREE, ExpressionTreeOperatorTools;

	name:Symbol			== -"+";
	arity:Z				== -1;
	uniqueId:Z			== UID__PLUS;
	texParen?(p:Z):Boolean		== p > TEXPREC__PLUS;
	axiom(p:TEXT, l:List TREE):TEXT	== trav(p, l, axiom);
	aldor(p:TEXT,l:List TREE):TEXT	== trav(p, l, aldor);
	maple(p:TEXT, l:List TREE):TEXT	== trav(p, l, maple);
	C(p:TEXT, l:List TREE):TEXT	== trav(p, l, C);
	fortran(p:TEXT,l:List TREE):TEXT== trav(p, l, fortran);
	tex(p:TEXT, l:List TREE):TEXT	== pretty(p, l, tex,"\left(","\right)");
	infix(p:TEXT, l:List TREE):TEXT	== pretty(p, l, infix, "(", ")");

	trav(p:TEXT, l:List TREE, f:(TEXT, TREE) -> TEXT):TEXT ==
		infix(p, l, (t:TREE):Boolean +-> true, f, "+");

	local pretty(p:TEXT, l:List TREE, f:(TEXT, TREE)->TEXT,
			lp:String, rp:String):TEXT == {
		empty? l => p;
		empty? rest l => f(p, first l);
		paren? := texParen?(TEXPREC__PLUS, first l);
		if paren? then p := p << lp;
		p := f(p, first l);
		for t in rest l repeat {
			if paren? then p := p << rp;
			if is?(t, ExpressionTreeMinus)
				and empty?(rest arguments t) then {	-- unary
					p := p << "-";
					tt := first arguments t;
			}
			else {
				p := p << "+";
				tt := t;
			}
			paren? := texParen?(TEXPREC__PLUS, tt);
			if paren? then p := p << lp;
			p := f(p, tt);
		}
		paren? => p << rp;
		p;
	}

}

-------------------------- sit_OPcplex.as --------------------------
-- Copyright (c) Manuel Bronstein 2001
-- Copyright (c) INRIA 2001, Version 0.1.13
-- Logiciel Sum^it (c) INRIA 2001, dans sa version 0.1.13
-----------------------------------------------------------------------------

#include "algebra"
#include "algebrauid"

#if ALDOC
\thistype{ExpressionTreeComplex}
\History{Manuel Bronstein}{21/11/94}{created}
\Usage{import from \this}
\Descr{\this~is the complex operator for expression trees.}
\begin{exports}
\category{\altype{ExpressionTreeOperator}}\\
\end{exports}
#endif

macro {
	Z	== MachineInteger;
	TEXT	== TextWriter;
	TREE 	== ExpressionTree;
}

ExpressionTreeComplex:ExpressionTreeOperator == add {
	import from String, TREE, ExpressionTreeOperatorTools;

	name:Symbol			== -"complex";
	arity:Z				== 2;
	uniqueId:Z			== UID__COMPLEX;
	texParen?(p:Z):Boolean		== true;
	axiom(p:TEXT, l:List TREE):TEXT	== prefix(p, l, axiom, "complex");
	aldor(p:TEXT, l:List TREE):TEXT	== prefix(p, l, aldor, "complex");
	lisp(p:TEXT, l:List TREE):TEXT	== lisp(p, "complex", l);
	infix(p:TEXT, l:List TREE):TEXT	== pretty(p, l, infix, "(", ")", " %i");

	maple(p:TEXT, l:List TREE):TEXT	== {
		p := maple(p << "(", first l) << ") + (";
		maple(p, first rest l) << ") * I";
	}

	fortran(p:TEXT, l:List TREE):TEXT == {
		p := fortran(p << "(", first l) << ",";
		fortran(p, first rest l) << ")";
	}

	C(p:TEXT, l:List TREE):TEXT == {
		p := C(p << "{", first l) << ",";
		C(p, first rest l) << "}";
	}

	tex(p:TEXT, l:List TREE):TEXT ==
		pretty(p, l, tex, "\left(", "\right)", "\,\imath");

	local pretty(p:TEXT, l:List TREE, f:(TEXT, TREE)->TEXT,
		lp:String, rp:String, i:String):TEXT == {
		p := f(p, first l);
		if negative?(a := first rest l) then {
			p := p << "-";
			a := negate a;
		}
		else p := p << "+";
		paren? := texParen?(TEXPREC__TIMES, a);
		if paren? then p := p << lp;
		p := f(p, a);
		if paren? then p := p << rp;
		p << i;
	}
}

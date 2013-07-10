-------------------------- sit_OPminus.as --------------------------
-- Copyright (c) Manuel Bronstein 1995
-- Copyright (c) INRIA 1999, Version 0.1.12
-- Logiciel Sum^it (c) INRIA 1999, dans sa version 0.1.12
-- Copyright (c) Swiss Federal Polytechnic Institute Zurich, 1995-97
-----------------------------------------------------------------------------

#include "algebra"
#include "algebrauid"

#if ALDOC
\thistype{ExpressionTreeMinus}
\History{Manuel Bronstein}{22/12/94}{created}
\Usage{import from \this}
\Descr{\this~is the unary/binary minus operator for expression trees.}
\begin{exports}
\category{\altype{ExpressionTreeOperator}}\\
\end{exports}
#endif

macro {
	Z	== MachineInteger;
	TEXT	== TextWriter;
	TREE 	== ExpressionTree;
}

ExpressionTreeMinus:ExpressionTreeOperator == add {
	import from String, TREE, ExpressionTreeOperatorTools;

	name:Symbol			== -"-";
	arity:Z				== -1;
	uniqueId:Z			== UID__MINUS;
	texParen?(p:Z):Boolean		== p > TEXPREC__MINUS;
	axiom(p:TEXT, l:List TREE):TEXT	== pref(p, l, axiom);
	aldor(p:TEXT,l:List TREE):TEXT	== pref(p, l, aldor);
	maple(p:TEXT, l:List TREE):TEXT	== pref(p, l, maple);
	C(p:TEXT, l:List TREE):TEXT	== pref(p, l, C);
	fortran(p:TEXT,l:List TREE):TEXT== pref(p, l, fortran);
	tex(p:TEXT, l:List TREE):TEXT	== pretty(p, l, tex,"\left(","\right)");
	infix(p:TEXT, l:List TREE):TEXT	== pretty(p, l, infix, "(", ")");

	local pretty(p:TEXT, l:List TREE, f:(TEXT, TREE)->TEXT,
		lp:String, rp:String):TEXT == {
		empty? rest l =>	-- unary case
			prefix(p, first l,
				(t:TREE):Boolean+-> texParen?(TEXPREC__MINUS,t),
					f, "-", lp, rp);
		assert(empty? rest rest l);
		paren? := texParen?(TEXPREC__PLUS, first l);
		if paren? then p := p << lp;
		p := f(p, first l);
		if paren? then p := p << rp;
		p := p << "-";
		paren? := texParen?(TEXPREC__MINUS, first rest l);
		if paren? then p := p << lp;
		f(p, first rest l);
		paren? => p << rp;
		p;
	}

	pref(p:TEXT, l:List TREE, f:(TEXT, TREE) -> TEXT):TEXT == {
		empty? rest l =>	-- unary case
			prefix(p, first l, (t:TREE):Boolean +-> ~leaf? t,f,"-");
		assert(empty? rest rest l);
		infix(p, l, (t:TREE):Boolean +-> true, f, "-");
	}
}

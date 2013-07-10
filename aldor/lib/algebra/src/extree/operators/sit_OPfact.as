-------------------------- sit_OPfact.as --------------------------
-- Copyright (c) Manuel Bronstein 2000
-- Copyright (c) INRIA 1999, Version 0.1.12
-- Logiciel Sum^it ©INRIA 1999, dans sa version 0.1.12
-----------------------------------------------------------------------------

#include "algebra"
#include "algebrauid"

#if ALDOC
\thistype{ExpressionTreeFactorial}
\History{Manuel Bronstein}{18/6/2000}{created}
\Usage{import from \this}
\Descr{\this~is the generalized factorial operator for expression trees.}
\begin{exports}
\category{\altype{ExpressionTreeOperator}}\\
\end{exports}
#endif

macro {
	Z	== MachineInteger;
	TEXT	== TextWriter;
	TREE 	== ExpressionTree;
}

ExpressionTreeFactorial:ExpressionTreeOperator == add {
	import from String, TREE, ExpressionTreeOperatorTools;

	name:Symbol			== -"factorial";
	arity:Z				== 3;
	uniqueId:Z			== UID__FACTORIAL;
	texParen?(p:Z):Boolean		== false;
	axiom(p:TEXT, l:List TREE):TEXT	== prefix(p, l, axiom, "factorial");
	aldor(p:TEXT,l:List TREE):TEXT	== prefix(p, l, aldor, "factorial");
	maple(p:TEXT, l:List TREE):TEXT	== prefix(p, l, maple, "factorial");
	fortran(p:TEXT,l:List TREE):TEXT== prefix(p, l, fortran, "factorial");
	C(p:TEXT, l:List TREE):TEXT	== prefix(p, l, C, "factorial");
	lisp(p:TEXT, l:List TREE):TEXT	== lisp(p, "factorial", l);

	tex(p:TEXT, l:List TREE):TEXT == {
		import from Integer, ExpressionTreeLeaf;
		a := first l; s := first rest l; n := first rest rest l;
		leaf? s and integer?(lf := leaf s)
			and (one?(st := integer lf) or st = -1) => {
			str := { st > 0 => "ov"; "und" };
			p << "{" << a << "}^{{\" <<str<< "erline " << n << "}}";
				
		}
		prefix(p, l, tex, "\factorial", "\left(", "\right)");
	}
}

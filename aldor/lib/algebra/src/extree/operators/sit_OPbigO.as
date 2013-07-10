-------------------------- sit_OPbigO.as --------------------------
-- Copyright (c) Manuel Bronstein 2000
-- Copyright (c) INRIA 1999, Version 0.1.12
-- Logiciel Sum^it ©INRIA 1999, dans sa version 0.1.12
-----------------------------------------------------------------------------

#include "algebra"
#include "algebrauid"

#if ALDOC
\thistype{ExpressionTreeBigO}
\History{Manuel Bronstein}{31/5/2000}{created}
\Usage{import from \this}
\Descr{\this~is the $\cal O$ operator for expression trees.}
\begin{exports}
\category{\astype{ExpressionTreeOperator}}\\
\end{exports}
#endif

macro {
	Z	== MachineInteger;
	TEXT	== TextWriter;
	TREE 	== ExpressionTree;
}

ExpressionTreeBigO:ExpressionTreeOperator == add {
	import from String, TREE, ExpressionTreeOperatorTools;

	name:Symbol			== -"O";
	arity:Z				== 1;
	uniqueId:Z			== UID__BIGO;
	texParen?(p:Z):Boolean		== false;
	axiom(p:TEXT, l:List TREE):TEXT	== pref(p, l, axiom);
	aldor(p:TEXT,l:List TREE):TEXT	== pref(p, l, aldor);
	maple(p:TEXT, l:List TREE):TEXT	== pref(p, l, maple);
	C(p:TEXT, l:List TREE):TEXT	== pref(p, l, C);
	fortran(p:TEXT,l:List TREE):TEXT== pref(p, l, fortran);

	tex(p:TEXT, l:List TREE):TEXT   ==
		prefix(p, first l, (t:TREE):Boolean +-> true,
				tex, "\mathcal{O}", "\left(", "\right)");

	pref(p:TEXT, l:List TREE, f:(TEXT, TREE) -> TEXT):TEXT ==
		prefix(p, first l, (t:TREE):Boolean +-> true, f, "O");
}

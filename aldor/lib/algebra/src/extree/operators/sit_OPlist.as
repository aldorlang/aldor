--------------------------   sit_OPlist.as   ------------------------
-- Copyright (c) Manuel Bronstein 1995
-- Copyright (c) INRIA 1999, Version 0.1.12
-- Logiciel Sum^it ©INRIA 1999, dans sa version 0.1.12
-- Copyright (c) Swiss Federal Polytechnic Institute Zurich, 1995-97
-----------------------------------------------------------------------------

#include "algebra"
#include "algebrauid"

#if ALDOC
\thistype{ExpressionTreeList}
\History{Manuel Bronstein}{15 May 96}{created}
\Usage{import from \this}
\Descr{\this~is the list operator for expression trees.}
\begin{exports}
\category{\astype{ExpressionTreeOperator}}\\
\end{exports}
\begin{remarks}
The following function is not yet implemented and produces dummy results :
{\bf fortran}.
\end{remarks}
#endif
	
macro {
	Z	== MachineInteger;
	TEXT	== TextWriter;
	TREE	== ExpressionTree;
}

ExpressionTreeList : ExpressionTreeOperator == add {
	import from String, TREE, ExpressionTreeOperatorTools;
	import from ExpressionTreeLeaf;

	name:Symbol			== -"list";
	arity:Z				== -1;
	uniqueId:Z			== UID__LIST;
	texParen?(p:Z):Boolean		== false;
	fortran(p:TEXT,l:List TREE):TEXT== p << "--- fortran list ---";
	aldor(p:TEXT,l:List TREE):TEXT	== trav(p, l, aldor, "[", "]");
	axiom(p:TEXT, l:List TREE):TEXT	== trav(p, l, axiom, "[", "]");
	maple(p:TEXT, l:List TREE):TEXT	== trav(p, l, maple, "[", "]");
	C(p:TEXT,l:List TREE):TEXT	== trav(p, l, C, "{", "}");
	lisp(p:TEXT,l:List TREE):TEXT	== trav(p, l, lisp, "(", ")");
	tex(p:TEXT, l:List TREE):TEXT	== trav(p, l, tex, "[ ", "]");
	infix(p:TEXT, l:List TREE):TEXT	== trav(p, l, infix, "[ ", "]");

	local trav ( p:TEXT, l:List TREE, f:(TEXT,TREE)->TEXT, 
	       begin:String, end:String ) : TEXT == {
		p << begin;
		infix(p, l, (t:TREE):Boolean +-> false, f, ",");
		p << end;
	}
}


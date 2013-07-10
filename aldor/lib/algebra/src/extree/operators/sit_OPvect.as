--------------------------   sit_OPvect.as   ------------------------
-- Copyright (c) Marco Codutti 1995
-- Copyright (c) INRIA 1999, Version 0.1.12
-- Logiciel Sum^it ©INRIA 1999, dans sa version 0.1.12
-- Copyright (c) Swiss Federal Polytechnic Institute Zurich, 1995-97
-----------------------------------------------------------------------------

#include "algebra"
#include "algebrauid"

#if ALDOC
\thistype{ExpressionTreeVector}
\History{Marco Codutti}{10 May 95}{created}
\Usage{import from \this}
\Descr{\this~is the vector operator for expression trees.}
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
	FALSE	== (t:TREE):Boolean +-> false;
}

ExpressionTreeVector : ExpressionTreeOperator == add {
	import from String, TREE, ExpressionTreeOperatorTools;
	import from ExpressionTreeLeaf;

	name:Symbol			== -"vector";
	arity:Z				== -1;
	uniqueId:Z			== UID__VECTOR;
	texParen?(p:Z):Boolean		== false;
	fortran(p:TEXT,l:List TREE):TEXT== p << "--- fortran vector ---";

	infix(p:TEXT,l:List TREE):TEXT	== 
		trav (p,l,infix, "vector [", ",", "]" );

	aldor(p:TEXT,l:List TREE):TEXT	== 
		trav (p,l,aldor, "vector [", ",", "]" );

	axiom(p:TEXT, l:List TREE):TEXT	==  
		trav (p,l,axiom, "vector [", ",", "]" );

	maple(p:TEXT, l:List TREE):TEXT	==   {
		p << "linalg[vector](" << #l;
		trav (p,l,maple, ",[", ",", "])" );
	}

	C(p:TEXT,l:List TREE):TEXT	== {
		p << "[" << #l;
		trav(p, l, C, "]{", ",", "}" );
	}

	tex(p:TEXT, l:List TREE):TEXT == trav(p, l, tex, "[ ", " , ", " ]");

	trav ( p:TEXT, l:List TREE, f:(TEXT,TREE)->TEXT, 
	       begin:String, sep:String, end:String ) : TEXT == {
		p << begin;
		infix( p, l, FALSE, f, sep );
		p << end;
	}
}


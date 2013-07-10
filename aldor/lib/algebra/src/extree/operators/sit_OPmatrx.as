--------------------------   sit_OPmatrx.as   ------------------------
-- Copyright (c) Marco Codutti 1995
-- Copyright (c) INRIA 1999, Version 0.1.12
-- Logiciel Sum^it ©INRIA 1999, dans sa version 0.1.12
-- Copyright (c) Swiss Federal Polytechnic Institute Zurich, 1995-97
-----------------------------------------------------------------------------

#include "algebra"
#include "algebrauid"

#if ALDOC
\thistype{ExpressionTreeMatrix}
\History{Marco Codutti}{10 May 95}{created}
\Usage{import from \this}
\Descr{\this~is the matrix operator for expression trees.}
\begin{exports}
\category{\astype{ExpressionTreeOperator}}\\
\end{exports}
\begin{remarks}
The following function is not yet implemented and produces dummy results :
{\bf fortran}.
\end{remarks}
#endif
	
macro {
	Z	      == MachineInteger;
	TEXT	      == TextWriter;
	TREE 	 == ExpressionTree;
	FALSE      == (t:TREE):Boolean +-> false;
	RET        == new(1,newline);
}

ExpressionTreeMatrix : ExpressionTreeOperator == add
{
	import from String, TREE, ExpressionTreeOperatorTools;

	name:Symbol			== -"matrix";
	arity:Z				== -1;
	uniqueId:Z			== UID__MATRIX;
	texParen?(p:Z):Boolean   == false;

	aldor(p:TEXT,l:List TREE):TEXT	== 
		trav (p,l,aldor, "matrix [[", ",", "],[", "]]" );

	axiom(p:TEXT, l:List TREE):TEXT	==  
		trav (p,l,axiom, "matrix [[", ",", "],[", "]]" );

	infix(p:TEXT, l:List TREE):TEXT	==  
		trav (p,l,infix, "matrix [[", ",", "],[", "]]" );

	lisp(p:TEXT, l:List TREE):TEXT	==  
		trav (p,l,lisp, "((", " ", ") (", "))" );

	local dimensions(l:List TREE):(Z, Z) == {
		import from TREE, ExpressionTreeLeaf;
		(machineInteger leaf first l, machineInteger leaf first rest l);
	}

	maple(p:TEXT, l:List TREE):TEXT	==   {
		import from Z;
		(n, m) := dimensions l;
		p << "linalg[matrix](" << n << "," << m;
		n = 0 or m = 0 => p << ",[])";
		trav (p,l,maple, ",[", ",", ",", "])" );
	}

	fortran(p:TEXT,l:List TREE):TEXT== p << "--- fortran matrix ---";

	C(p:TEXT,l:List TREE):TEXT	== {
		import from Z;
		(n, m) := dimensions l;
		p << "[" << n << "][" << m << "]{";
		infix( p, l, FALSE, C, "," );
		p << "}";
	}

	tex(p:TEXT, l:List TREE):TEXT == 
	{
		import from Character;
		b := "\pmatrix{" + RET;
		trav (p,l,tex, b, " & ", " \cr " + RET, "\cr } ");
	}

	-- return ([a_{n+1},...], [a_1,\dots,a_n]) where l = [a_1,...]
	local first(l:List TREE, n:Z):(List TREE, List TREE) == {
		import from Boolean;
		ll:List TREE := empty;
		for i in 1..n repeat {
			assert(~empty? l);
			ll := cons(first l, ll);
			l := rest l;
		}
		(l, reverse! ll);
	}

	local trav ( p:TEXT, l:List TREE, f:(TEXT,TREE)->TEXT, 
	       begin:String, sep1:String, sep2:String, end:String ) : TEXT ==
	{
		import from Z, Boolean;
		(n, m) := dimensions l;
		l := rest rest l;
		p << begin;
		for i in 1..prev n repeat {
			(l, lm) := first(l, m);
			infix( p, lm, FALSE, f, sep1 );
			p << sep2;
		}
		if ~empty?(l) then infix( p, l, FALSE, f, sep1 );
		p << end;
	}
}


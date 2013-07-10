------------------------------   sit_OPcase.as   ------------------------------
-- Copyright (c) Marco Codutti 1995
-- Copyright (c) INRIA 1999, Version 0.1.12
-- Logiciel Sum^it ©INRIA 1999, dans sa version 0.1.12
-- Copyright (c) Swiss Federal Polytechnic Institute Zurich, 1995-97
-----------------------------------------------------------------------------

#include "algebra"
#include "algebrauid"

macro {
	TEXT == TextWriter;
	TREE == ExpressionTree;
	SI   == MachineInteger;

	NOTYET(a,b) == p << a << " output for " << b << " not yet implemented.";
}

#if ALDOC
\thistype{ExpressionTreeCase}
\History{Marco Codutti}{12 June 95}{created}
\Usage{import from \this}
\Descr{\this~is the {\em multi conditional} operator for expression trees.}
\begin{exports}
\category{\astype{ExpressionTreeOperator}}\\
\end{exports}
\begin{remarks}
The following functions are not yet implemented and produce dummy results :
{\bf axiom}, {\bf C}, {\bf fortran}, {\bf maple}.
\end{remarks}
#endif

ExpressionTreeCase: ExpressionTreeOperator == add
{
	import from String, TREE, ExpressionTreeLeaf;

	name:Symbol             == -"case";
	arity:SI                == -1;
	uniqueId:SI             == UID__CASE;
	texParen?(p:SI):Boolean == true;

	aldor  (p:TEXT, l:List TREE): TEXT == {
		import from Boolean;
		empty? l => p << "case()";
		p := p << "case(";
		t := l;
		o := first t; t := rest  t;
		c := first t; t := rest  t;
		p := aldor (p, ExpressionTreeIf [ c, o ] );
		while ~empty? t repeat {
			o := first t; t := rest  t;
			c := first t; t := rest  t;
			p := p << ",";
			p := aldor (p, ExpressionTreeIf [ c, o ] );
		}
		p << ")";
	}

	axiom   (p:TEXT, l:List TREE): TEXT == NOTYET("axiom",name);
	C       (p:TEXT, l:List TREE): TEXT == NOTYET("C",name);
	fortran (p:TEXT, l:List TREE): TEXT == NOTYET("fortran",name);
	lisp   (p:TEXT, l:List TREE): TEXT == NOTYET("lisp",name);

	maple   (p:TEXT, l:List TREE): TEXT == {
		import from Boolean;
		t := l;
		empty? t => p << "()";
		leaf? (c:=first rest t) and string? (s := leaf(c)) 
			and string s = "always" => p << first t;
		p := p << "piecewise(";
		while ~empty? t repeat {
			o := first t; t := rest  t;
			c := first t; t := rest  t;
			p := maple(p, c); p := p << ","; p := maple(p, o);
			if ~empty? t then p := p << ",";
		}
		p << ")";
	}

	tex (p:TEXT, l:List TREE): TEXT == {
		import from Boolean;
		t := l;
		empty? t => p << "\left\{ \mbox{\em no case} \right.";
		leaf? (c:=first rest t) and string? (s := leaf(c)) 
			and string s = "always" => p << first t;
		p := p << "\left\{ \begin{array}{ll}";
		while ~empty? t repeat {
			o := first t; t := rest  t;
			c := first t; t := rest  t;
			p := tex (p,o); p := p << " & \mbox{if } ";
			p := tex (p,c); p := p << " \\";
		}
		p << "\end{array} \right.";
	}
}


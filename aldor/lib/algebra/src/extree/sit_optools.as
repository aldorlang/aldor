-------------------------- sit_optools.as --------------------------
--
-- Tools for generic operator methods
--
-- Copyright (c) Manuel Bronstein 1994
-- Copyright (c) INRIA 1999, Version 0.1.12
-- Logiciel Sum^it (c) INRIA 1999, dans sa version 0.1.12
-- Copyright (c) Swiss Federal Polytechnic Institute Zurich, 1994-97
-----------------------------------------------------------------------------

#include "algebra"

#if ALDOC
\thistype{ExpressionTreeOperatorTools}
\History{Manuel Bronstein}{21/11/94}{created}
\Usage{import from \this}
\Descr{\this~provides utilities that make it simpler to write new operator
types.}
\begin{exports}
\alexp{infix}: & (TEXT, \altype{List} TREE, TREE) $\to$ \altype{Boolean}, & \\
& ~~~(TEXT, TREE) $\to$ TEXT, & \\
& ~~~\altype{String}, \altype{String}, \altype{String}) $\to$ TEXT &
Write as infix\\
\alexp{prefix}: & (TEXT, \altype{List} TREE, & \\
& ~~~(TEXT, TREE) $\to$ TEXT, & \\
& ~~~\altype{String}, \altype{String}, \altype{String}) $\to$ TEXT &
Write as prefix\\
\end{exports}
\begin{aswhere}
TEXT &==& \altype{TextWriter}\\
TREE &==& \altype{ExpressionTree}\\
\end{aswhere}
#endif

macro {
	TEXT	== TextWriter;
	TREE 	== ExpressionTree;
}

ExpressionTreeOperatorTools: with {
	infix: (TEXT, List TREE, TREE -> Boolean, (TEXT, TREE) -> TEXT,
		String, lp:String == "(", rp:String == ")") -> TEXT;
#if ALDOC
\alpage{infix}
\Usage{\name(p, [$t_1,\dots,t_n$], paren?, farg, op, left, right)}
\Signatures{
\name: & (TEXT, \altype{List} TREE, TREE $\to$ \altype{Boolean},
(TEXT, TREE) $\to$ TEXT,\\
& ~~~\altype{String}, \altype{String}, \altype{String}) $\to$ TEXT\\ }
\Params{
{\em p} & TEXT & The port to write to\\
{\em [$t_1,\dots,t_n$]} & \altype{List} TREE & The arguments of the operator\\
{\em paren?} & TREE $\to$ \altype{Boolean} & The parenthetization function\\
{\em farg} & (TEXT, TREE) $\to$ TEXT & The function for the arguments\\
{\em op} & \altype{String} & The infix symbol\\
{\em left} & \altype{String} & The left parenthesis (optional)\\
{\em right} & \altype{String} & The right parenthesis (optional)\\
}
\begin{aswhere}
TEXT &==& \altype{TextWriter}\\
TREE &==& \altype{ExpressionTree}\\
\end{aswhere}
\Descr{Writes $farg(t_1)~op~\dots~op~farg(t_n)$ to $p$, calling $farg$
on each argument, and calling $paren?$ to decide whether to parenthetize
each argument. Uses $left$ and $right$, which default to ``('' and ``)''
when parenthetizing.}
\alseealso{\alexp{prefix}}
#endif
	lisp: (TEXT, String, List TREE) -> TEXT;
#if ALDOC
\alpage{lisp}
\Usage{\name(p, s, [$t_1,\dots,t_n$])}
\Signature{(TEXT, \altype{String}, \altype{List} TREE)}{TEXT}
\Params{
{\em p} & TEXT & The port to write to\\
{\em s} & \altype{String} & A Lisp operator name\\
{\em [$t_1,\dots,t_n$]} & \altype{List} TREE & The arguments of the operator\\
}
\Descr{Writes $(s t_1 \dots t_n)$ to $p$, where each $t_i$ is written
in Lisp format.}
#endif
	prefix: (TEXT, TREE, TREE -> Boolean, (TEXT, TREE) -> TEXT,
		String, lp:String == "(", rp:String == ")") -> TEXT;
	prefix: (TEXT, List TREE, (TEXT, TREE) -> TEXT,
		String, lp:String == "(", rp:String == ")") -> TEXT;
#if ALDOC
\alpage{prefix}
\Usage{
\name(p, t, paren?, farg, op, left, right)\\
\name(p, [$t_1,\dots,t_n$], farg, op, left, right)
}
\Signatures{
\name: & (TEXT, TREE, TREE $\to$ \altype{Boolean}, (TEXT, TREE) $\to$ TEXT,\\
 & \phantom{name:}~~~\altype{String}, \altype{String}, \altype{String})
$\to$ TEXT\\
\name: & (TEXT, \altype{List} TREE, (TEXT, TREE) $\to$ TEXT,\\
 & \phantom{name:}~~~\altype{String}, \altype{String}, \altype{String})
$\to$ TEXT\\
}
\Params{
{\em p} & TEXT & The port to write to\\
{\em [$t_1,\dots,t_n$]} & \altype{List} TREE & The arguments of the operator\\
{\em paren?} & TREE $\to$ \altype{Boolean} & The parenthetization function\\
{\em farg} & (TEXT, TREE) $\to$ TEXT & The function for the arguments\\
{\em op} & \altype{String} & The prefix symbol\\
{\em left} & \altype{String} & The left parenthesis (optional)\\
{\em right} & \altype{String} & The right parenthesis (optional)\\
}
\begin{aswhere}
TEXT &==& \altype{TextWriter}\\
TREE &==& \altype{ExpressionTree}\\
\end{aswhere}
\Descr{Writes $op(farg(t_1),\dots,farg(t_n))$ to $p$, calling $farg$
on each argument. Uses $left$ and $right$, which default to ``('' and ``)''
for parenthetizing. The unary version calls $paren?$ to decide whether
to parenthetize the argument.}
\alseealso{\alexp{infix}}
#endif
} == add {
	import from MachineInteger;

	local alwaysTrue(t:TREE):Boolean == true;

        prefix(p:TEXT, t:TREE, par?:TREE -> Boolean, f:(TEXT,TREE) -> TEXT,
		op:String, lp:String, rp:String):TEXT == {
			p := p << op;
			paren? := par? t;
			if paren? then p := p << lp;
			p := f(p, t);
			paren? => p << rp;
			p;
	}

        prefix(p:TEXT, l:List TREE, f:(TEXT,TREE) -> TEXT,
		op:String, lp:String, rp:String):TEXT == {
			p := p << op << lp;
			for t in l for i in #l.. by -1 repeat {
				p := f(p, t);
				if i > 1 then p := p << ",";
			}
			p << rp;
	}

        infix(p:TEXT, l:List TREE, par?:TREE -> Boolean, f:(TEXT,TREE) -> TEXT,
		op:String, lp:String, rp:String):TEXT == {
			empty? l => p;
			empty? rest l => f(p, first l);
			paren? := par? first l;
			if paren? then p := p << lp;
			p := f(p, first l);
			for t in rest l repeat {
				if paren? then p := p << rp;
				p := p << op;
				paren? := par? t;
				if paren? then p := p << lp;
				p := f(p, t);
			}
			paren? => p << rp;
			p;
	}

	lisp(p:TEXT, s:String, l:List TREE):TEXT == {
		import from TREE;
		p := p << "(" << s;
		for arg in l repeat p := lisp(p << " ", arg);
		p << ")";
	}
}

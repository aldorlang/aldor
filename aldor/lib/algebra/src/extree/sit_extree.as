------------------------------- sit_extree.as ----------------------------------
--
-- Inert Expression Trees
--
-- Copyright (c) Manuel Bronstein 1994
-- Copyright (c) INRIA 1999, Version 0.1.12
-- Logiciel Sum^it (c) INRIA 1999, dans sa version 0.1.12
-- Copyright (c) Swiss Federal Polytechnic Institute Zurich, 1994-97
-----------------------------------------------------------------------------

#include "algebra"
#include "algebrauid"

#include "alg_op"
#include "alg_leaf"

#if ALDOC
\thistype{ExpressionTree}
\History{Manuel Bronstein}{24/11/94}{created}
\History{Manuel Bronstein}{23/6/2003}{added infix}
\Usage{import from \this}
\Descr{\this~is a type whose elements are expression trees.}
\begin{exports}
\category{\altype{OutputType}}\\
\category{\altype{PrimitiveType}}\\
\alexp{aldor}: & (TEXT, \%) $\to$ TEXT & Conversion to \asharp code\\
\alexp{apply}:
& (OP, \altype{List} \%) $\to$ \% & Apply an operator to arguments\\
\alexp{apply}:
& (OP, \builtin{Tuple} \%) $\to$ \% & Apply an operator to arguments\\
\alexp{arguments}:
& \% $\to$ \altype{List} \% & Take the arguments of the root\\
\alexp{axiom}: & (TEXT, \%) $\to$ TEXT & Conversion to Axiom code\\
\alexp{C}: & (TEXT, \%) $\to$ TEXT & Conversion to C code\\
\alexp{extree}: & \altype{ExpressionTreeLeaf} $\to$ \% & Conversion to a tree\\
\alexp{fortran}: & TEXT, \%) $\to$ TEXT & Conversion to FORTRAN code\\
\alexp{infix}: & (TEXT, \%) $\to$ TEXT & Conversion to one-dim infix output\\
\alexp{is?}: & (\%, OP) $\to$ \altype{Boolean} & Test for a specific operator\\
\alexp{leaf}: & \% $\to$ \altype{ExpressionTreeLeaf} & Conversion to a leaf\\
\alexp{leaf?}: & \% $\to$ \altype{Boolean} & Test whether tree is a leaf\\
\alexp{lisp}: & TEXT, \%) $\to$ TEXT & Conversion to Lisp code\\
\alexp{maple}: & (TEXT, \%) $\to$ TEXT & Conversion to Maple code\\
\alexp{operator}: & \% $\to$ OP & Take the root operator\\
\alexp{tex}: & (TEXT, \%) $\to$ TEXT & Conversion to \LaTeX\\
\alexp{texParen?}:
& (\altype{MachineInteger}, \%) $\to$ \altype{Boolean} &
Check whether to parenthetize\\
\end{exports}
\begin{alwhere}
TEXT &==& \altype{TextWriter}\\
OP &==& \altype{ExpressionTreeOperator}\\
\end{alwhere}
#endif

ExpressionTree: Join(OutputType, PrimitiveType) with {
	aldor:		(TEXT, %) -> TEXT;
	axiom:		(TEXT, %) -> TEXT;
	C:		(TEXT, %) -> TEXT;
	fortran:	(TEXT, %) -> TEXT;
	infix:		(TEXT, %) -> TEXT;
	lisp:		(TEXT, %) -> TEXT;
	maple:		(TEXT, %) -> TEXT;
	tex:		(TEXT, %) -> TEXT;
#if ALDOC
\alpage{aldor,axiom,C,fortran,infix,lisp,maple,tex}
\altarget{aldor}
\altarget{axiom}
\altarget{C}
\altarget{fortran}
\altarget{infix}
\altarget{lisp}
\altarget{maple}
\altarget{tex}
\Usage{{\em format}(p, t)}
\Signature{(\altype{TextWriter}, \%)}{\altype{TextWriter}}
\Params{
{\em p} & \altype{TextWriter} & The port to write to\\
{\em t} & \% & An expression tree\\
}
\Descr{Writes to $p$ the expression corresponding to the tree $t$
in the requested format.}
#endif
	--apply:		(OP, Tuple %) -> %;   -- compiler bug
	apply:		(OP, List %) -> %;
#if ALDOC
\alpage{apply}
\Usage{
% \name(op, $t_1,\dots,t_n$)\\  % compiler bug
\name(op, [$t_1,\dots,t_n$])\\
% op($t_1,\dots,t_n$)\\         % compiler bug
op~[$t_1,\dots,t_n$]
}
\Signature{(\altype{ExpressionTreeOperator}, \altype{List} \%)}{\%}
% Compiler bug
% \Signatures{
% \name: & (\altype{ExpressionTreeOperator}, \altype{List} \%) $\to$ \%\\
% \name: & (\altype{ExpressionTreeOperator}, \builtin{Tuple} \%) $\to$ \%\\
% }
\Params{
{\em op} & \altype{ExpressionTreeOperator} & An operator\\
{\em $t_i$} & \% & Expression trees\\
}
\Retval{Returns the tree whose root is $op$, with arguments $t_1,\dots,t_n$.}
#endif
	arguments:	% -> List %;
#if ALDOC
\alpage{arguments}
\Usage{\name~t}
\Signature{\%}{\altype{List} \%}
\Params{ {\em t} & \% & An expression tree\\ }
\Retval{Returns the list of arguments of the root operator of $t$, which must
not be a leaf.}
\alseealso{\alexp{operator}}
#endif
	extree:		ExpressionTreeLeaf -> %;
#if ALDOC
\alpage{extree}
\Usage{\name~a}
\Signature{\altype{ExpressionTreeLeaf}}{\%}
\Params{ {\em a} & \altype{ExpressionTreeLeaf} & A leaf\\ }
\Retval{\name~a returns $a$ as an expression tree.}
#endif
	is?:		(%, OP) -> Boolean;
#if ALDOC
\alpage{is?}
\Usage{\name(t, op)}
\Signature{(\%, \altype{ExpressionTreeOperator})}{\altype{Boolean}}
\Params{
{\em t} & \% & An expression tree\\
{\em op} & \altype{ExpressionTreeOperator} & An operator\\
}
\Retval{\name(t, op) returns \true~if t is of the form op(args),
\false~otherwise.}
#endif
	leaf:		% -> ExpressionTreeLeaf;
	leaf?:		% -> Boolean;
#if ALDOC
\alpage{leaf}
\altarget{\name?}
\Usage{ \name~t\\ \name?~t }
\Signatures{
\name: & \% $\to$ \altype{ExpressionTreeLeaf}\\
\name?: & \% $\to$ \altype{Boolean}\\
}
\Params{ {\em t} & \% & An expression tree\\ }
\Retval{
\name~a returns $a$ as a leaf is $a$ is a leaf.
\name?~a returns \true~if a is a leaf, \false~otherwise.
}
#endif
	negate:		% -> %;
#if ALDOC
\alpage{negate}
\Usage{\name~t}
\Signature{\%}{\%}
\Params{ {\em t} & \% & An expression tree\\ }
\Retval{Returns the leaf $-t$ if $t$ is a numerical leaf with $t < 0$,
and returns $s$ if $t$ is of the form $(- s)$ for some tree $s$.
$t$ must be of one of the above 2 forms.}
\alseealso{\alexp{negative?}}
#endif
	negative?:	% -> Boolean;
#if ALDOC
\alpage{negative?}
\Usage{\name~t}
\Signature{\%}{\altype{Boolean}}
\Params{ {\em t} & \% & An expression tree\\ }
\Retval{Returns \true~if either $t$ is a numerical leaf and $t < 0$,
or if $t$ is of the form $(- s)$ for some tree $s$, \false~otherwise.}
\alseealso{\alexp{negate}}
#endif
	operator:	% -> OP;
#if ALDOC
\alpage{operator}
\Usage{\name~t}
\Signature{\%}{\altype{ExpressionTreeOperator}}
\Params{ {\em t} & \% & An expression tree\\ }
\Retval{Returns the root operator of $t$, which must not be a leaf.}
\alseealso{\alexp{arguments}}
#endif
	texParen?:	(Z, %) -> Boolean;
#if ALDOC
\alpage{texParen?}
\Usage{\name(prec, t)}
\Signature{(\altype{MachineInteger}, \%)}{\altype{Boolean}}
\Params{
{\em prec} & \altype{MachineInteger} & An operator precendence.\\
{\em t} & \% & An expression tree\\
}
\Retval{Returns \true~if $t$ should be parenthetized when appearing as
argument of an operator of precedence {\em prec}, \false~otherwise.}
#endif
} == add {
	macro {
		Tree == Record(oper:OP, argum: List %);
		Leaf == ExpressionTreeLeaf;
		Rep == Union(uleaf: Leaf, utree: Tree);
	}

	import from Rep;

	-- sample:%			== per [sample$Leaf];
	extree(l:Leaf):%		== per [l];
	local tree(r:Tree):%		== per [r];
	--apply(op:OP, t:Tuple %):%	== tree [op, list t];
	leaf?(t:%):Boolean		== rep(t) case uleaf;
	leaf(t:%):Leaf			== { assert(leaf? t); rep(t).uleaf; }
	operator(t:%):OP		== operator tree t;
	arguments(t:%):List %		== arguments tree t;
	local operator(t:Tree):OP	== t.oper;
	local arguments(t:Tree):List %	== t.argum;
	texParen?(p:Z, t:Tree):Boolean	== texParen?(p)$operator(t);
	infix(p:TEXT, t:%):TEXT		== switchon(p, t, infix, infix);
	tex(p:TEXT, t:%):TEXT		== switchon(p, t, tex, tex);
	axiom(p:TEXT, t:%):TEXT		== switchon(p, t, axiom, axiom);
	maple(p:TEXT, t:%):TEXT		== switchon(p, t, maple, maple);
	C(p:TEXT, t:%):TEXT		== switchon(p, t, C, C);
	fortran(p:TEXT, t:%):TEXT	== switchon(p, t, fortran, fortran);
	lisp(p:TEXT, t:%):TEXT		== switchon(p, t, lisp, lisp);
	aldor(p:TEXT, t:%):TEXT		== switchon(p, t, aldor, aldor);
	is?(t:%, op:OP):Boolean		== (~leaf? t) and is?(tree t, op);
	local infix(p:TEXT,t:Tree):TEXT	== infix(p, arguments t)$operator(t);
	local tex(p:TEXT, t:Tree):TEXT	== tex(p, arguments t)$operator(t);
	local axiom(p:TEXT,t:Tree):TEXT	== axiom(p, arguments t)$operator(t);
	local maple(p:TEXT,t:Tree):TEXT	== maple(p, arguments t)$operator(t);
	local aldor(p:TEXT,t:Tree):TEXT== aldor(p, arguments t)$operator(t);
	local fortran(p:TEXT,t:Tree):TEXT== fortran(p, arguments t)$operator(t);
	local lisp(p:TEXT,t:Tree):TEXT	== lisp(p, arguments t)$operator(t);
	local C(p:TEXT, t:Tree):TEXT	== C(p, arguments t)$operator(t);
	local negate(t:Tree):%		== first arguments t;

	apply(op:OP, l:List %):% == {
		import from Z;
		assert((arity$op < 0) or (arity$op = #l));
		tree [op, l];
	}

	local tree(t:%):Tree == {
		import from Boolean;
		assert(~leaf? t);
		rep(t).utree;
	}

	local is?(t:Tree, op:OP):Boolean == {
		import from Z;
		uniqueId$operator(t) = uniqueId$op;
	}

	local negative?(t:Tree):Boolean == {
		import from Z;
		(uniqueId$operator(t)=UID__MINUS) and empty?(rest arguments t);
	}

	local opeq(x:%, y:%):Boolean == {
		import from Z;
		uniqueId$operator(x) = uniqueId$operator(y);
	}

	negative?(t:%):Boolean == {
		leaf? t => negative? leaf t;
		negative? tree t;
	}

	negate(t:%):% == {
		assert(negative? t);
		leaf? t => extree negate leaf t;
		negate tree t;
	}

	(p:TEXT) << (t:%):TEXT == {
		leaf? t => p << leaf t;
		stream(p, tree t); -- cannot use operator(t) in this function
	}

	local stream(p:TEXT, t:Tree):TEXT == {
		import from String, Symbol;
		p := p << "(" << name$operator(t);
		for a in arguments t repeat p := p << " " << a;
		p << ")";
	}

	(x:%) = (y:%):Boolean == {
		leaf? x => leaf? y and leaf x = leaf y;
		-- cannot use operator(t) in this function
		(~leaf? y) and arguments x = arguments y and opeq(x, y)
	}

	local switchon(p:TEXT, t:%, f:(TEXT, Leaf) -> TEXT,
		g:(TEXT, Tree) -> TEXT):TEXT == {
			leaf? t => f(p, leaf t);
			g(p, tree t);
	}

	texParen?(p:Z, t:%):Boolean == {
		leaf? t => texParen? leaf t;
		texParen?(p, tree t);
	}
}

#if ALDORTEST
---------------------- test extree.as --------------------------
#include "algebra"
#include "aldortest"

macro {
	Z		== MachineInteger;
	Plus		== ExpressionTreePlus;
	Minus		== ExpressionTreeMinus;
	Times		== ExpressionTreeTimes;
	Expt		== ExpressionTreeExpt;
	Quotient	== ExpressionTreeQuotient;
	Tree		== ExpressionTree;
}

height(t:Tree):Z == {
	import from List Tree;
	m:Z := 1;
	leaf? t => m;
	for a in arguments t repeat {
		h := height a;
		if h > m then m := h;
	}
	m + 1;
}

extree0(t:Tree):Boolean == {
	import from Z, String, Symbol, List Tree;
	2 = arity$operator(t) and name$operator(first arguments t) = -"+";
}

extree():Boolean == {
	import from Z, String, Symbol, SingleFloat;
	import from Tree, List Tree, ExpressionTreeLeaf;

	x := extree leaf(-"x");
	y := extree leaf(-"y");

	t2 := extree leaf 2;
	t3 := extree leaf 3;

	-- t is the fraction ((x-1.5)y^3 + 2x^2y^2 + (x+2)^3 y+x+1) / (y-2)

	t := Plus [Times [Plus [x, Minus [extree leaf 1.5]], Expt [y, t3]],_
		Times [t2, Expt [x, t2], Expt [y, t2]],_
		Times [Expt [Plus [x, t2], t3], y], x, extree leaf(1@Z)];

	t := Quotient [t, Plus [y, Minus [t2]]];

	6 = height t and extree0 t;
}

stdout << "Testing sit__extree..." << endnl;
aldorTest("extree", extree);
stdout << endnl;
#endif


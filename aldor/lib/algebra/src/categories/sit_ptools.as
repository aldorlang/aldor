----------------------------- sit_ptools.as ----------------------------------
--
-- This file provides help-functions for parsing expression trees
--
-- Copyright (c) Manuel Bronstein 1999
-- Copyright (c) INRIA 1999, Version 0.1.12
-- Logiciel Sum^it (c) INRIA 1999, dans sa version 0.1.12
-----------------------------------------------------------------------------

#include "algebra"
#include "algebrauid"

macro {
	TREE == ExpressionTree;
	LEAF == ExpressionTreeLeaf;
}

#if ALDOC
\thistype{ParsingTools}
\History{Manuel Bronstein}{30/08/99}{created}
\Usage{import from \this~R}
\Params{
{\em R} & \altype{Parsable} & A parsable arithmetic system\\
        & \altype{ArithmeticType} &\\
}
\Descr{\this~R provides tools for converting
expression trees into elements of R.}
\begin{exports}
\alexp{evalArith}: & (Z, \altype{List}~TREE) $\to$ \builtin{Partial} R &
Interpret an arithmetic expression\\
\alexp{evalInt}: & TREE $\to$ \builtin{Partial} \altype{Integer} &
Interpret an integer\\
\end{exports}
\begin{alwhere}
TREE &==& \altype{ExpressionTree}\\
Z &==& \altype{MachineInteger}\\
\end{alwhere}
#endif

ParsingTools(R:Join(Parsable, ArithmeticType)): with {
	evalArith: (MachineInteger, List TREE) -> Partial R;
#if ALDOC
\alpage{evalArith}
\Usage{\name(op,[$e_1,\dots,e_n$])}
\Signature{(\altype{MachineInteger}, \altype{List} \altype{ExpressionTree})}
{\altype{Partial} R}
\Params{
{\em op} & \altype{MachineInteger} & Code for an operator\\
$e_i$ & \altype{ExpressionTree} & Expression trees\
}
\Retval{Returns the result of evaluating $op(e_1,\dots,e_n)$ where
$op$ is a code from {\tt include/algebrauid.as}. Provides support for
the evaluation of the operators $+$, $-$, $\ast$ and $\land$,
as well as $/$ when {\em R} has \altype{CommutativeRing} or \altype{FloatType}.}
#endif
	evalInt: TREE -> Partial Integer;
#if ALDOC
\alpage{evalInt}
\Usage{\name~e}
\Signature{\altype{ExpressionTree}}{\altype{Partial} \altype{Integer}}
\Params{ {\em e} & \altype{ExpressionTree} & An expression tree\\ }
\Retval{Returns the value of $e$ as an integer if it is an
integer--valued leaf, \failed otherwise.}
#endif
} == add {
	macro intdom?	== R has IntegralDomain;
	macro float?	== R has FloatType;
	local maxint:Integer	== (max$MachineInteger)::Integer;

	evalInt(t:TREE):Partial Integer == {
		import from Integer, LEAF;
		leaf? t => {
			l := leaf t;
			machineInteger? l => [machineInteger(l)::Integer];
			integer? l => [integer l];
			failed;
		}
		failed;
	}

	evalArith(op:MachineInteger, l:List TREE):Partial R == {
		import from Boolean, Partial Integer;
		TRACE("parsable::eval:op = ", op);
		TRACE("parsable::eval:l = ", l);
		op = UID__PLUS => {
			ans:R := 0;
			for arg in l repeat {
				u := eval(arg)$R;
				failed? u => return failed;
				ans := ans + retract u;
			}
			[ans];
		}
		op = UID__TIMES => {
			ans:R := 1;
			for arg in l repeat {
				u := eval(arg)$R;
				failed? u => return failed;
				ans := ans * retract u;
			}
			[ans];
		}
		op = UID__MINUS => {
			assert(#l = 1 or #l = 2);
			failed?(u := eval(first l)$R) => failed;
			empty? rest l => [- retract u];
			failed?(v := eval(first rest l)$R) => failed;
			[retract(u) - retract(v)];
		}
		op = UID__EXPT => {
			assert(#l = 2);
			failed?(u := eval(first l)$R)
				or failed?(w := evalInt first rest l)
				or (e := retract w) < 0 or e > maxint => failed;
			[retract(u)^machine(e)];
		}
		intdom? and op = UID__DIVIDE => {
			assert(#l = 2);
			failed?(u := eval(first l)$R)
				or failed?(v := eval(first rest l)$R) => failed;
			zero?(vv := retract v) => throw SyntaxException;
			exactQuotient(retract u, vv)$R;
		}
		float? and op = UID__DIVIDE => {
			assert(#l = 2);
			failed?(u := eval(first l)$R)
				or failed?(v := eval(first rest l)$R) => failed;
			zero?(vv := retract v) => throw SyntaxException;
			[retract(u) / vv];
		}
		failed;
	}
}


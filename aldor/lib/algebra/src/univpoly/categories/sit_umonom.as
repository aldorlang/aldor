----------------------------- sit_umonom.as ----------------------------------
-- Copyright (c) Manuel Bronstein 1994
-- Copyright (c) INRIA 1999, Version 0.1.12
-- Logiciel Sum^it (c) INRIA 1999, dans sa version 0.1.12
-- Copyright (c) Swiss Federal Polytechnic Institute Zurich, 1994-97
-----------------------------------------------------------------------------

#include "algebra"

#if ALDOC
\thistype{UnivariateMonomial}
\History{Manuel Bronstein}{20/5/94}{created}
\Usage{import from \this~R\\ import from \this(R, x)}
\Params{
{\em R} & \altype{ExpressionType} & Coefficients of the monomials\\
        & \altype{ArithmeticType} &\\
{\em x} & \altype{Symbol} & The variable name (optional)\\
}
\Descr{This type implements univariate monomials with coefficients in R.}
\begin{exports}
%\category{\altype{Order}}\\
\category{\altype{ExpressionType}}\\
\alexp{apply}: & (\%, TREE) $\to$ TREE & Applies a monomial to a tree\\
\alexp{coefficient}: & \% $\to$ R & Extraction of the coefficient\\
\alexp{degree}: & \% $\to$ \altype{Integer} & The degree of a monomial\\
\alexp{map}: & (R $\to$ R) $\to$ \% $\to$ \% & Lift a map\\
\alexp{map!}: & (R $\to$ R) $\to$ \% $\to$ \% & Lift a map\\
\alexp{monomial}: & (R, \altype{Integer}) $\to$ \% & Creation of a monomial\\
\alexp{setCoefficient!}:
& (\%, R) $\to$ R & In--place coefficient modification\\
\alexp{setDegree!}: & (\%, Z) $\to$ Z & In-place degree modification\\
\end{exports}
\begin{exports}[if $R$ has \altype{FiniteCharacteristic} then]
\alexp{pthPower}: & \% $\to$ \% & Exponentiation to the characteristic\\
\alexp{pthPower!}:
& \% $\to$ \% & In--place exponentiation to the characteristic\\
\end{exports}
\begin{aswhere}
TREE &==& \altype{ExpressionTree}\\
\end{aswhere}
#endif

macro {
	Z == Integer;
	TREE == ExpressionTree;
}

UnivariateMonomial(R:Join(ArithmeticType, ExpressionType), var:Symbol == new()):
	ExpressionType with {
	apply: (%, TREE) -> TREE;
#if ALDOC
\alpage{apply}
\Usage{\name(p, t)}
\Signature{(\%, \altype{ExpressionTree})}{\altype{ExpressionTree}}
\Params{
{\em p} & \% & A monomial\\
{\em t} & \altype{ExpressionTree} & An expression tree\\
}
\Retval{Returns p as an expression tree using t as root variable name.}
#endif
	coefficient: % -> R;
#if ALDOC
\alpage{coefficient}
\Usage{\name~p}
\Signature{\%}{R}
\Params{ {\em p} & \% & A monomial\\ }
\Retval{Returns the coefficient of $p$, \ie $c$ where $p = c\; x^n$.}
\alseealso{\alexp{degree}, \alexp{setCoefficient!}}
#endif
	degree: % -> Z;
#if ALDOC
\alpage{degree}
\Usage{\name~p}
\Signature{\%}{\altype{Integer}}
\Params{ {\em p} & \% & A monomial\\ }
\Retval{Returns the degree of $p$, \ie $n$ where $p = c\; x^n$.}
\alseealso{\alexp{coefficient}}
#endif
	map: (R -> R) -> % -> %;
	map!: (R -> R) -> % -> %;
#if ALDOC
\alpage{map}
\altarget{\name!}
\Usage{\name~f\\\name!~f\\\name(f)(p)\\\name!(f)(p)}
\Signature{(R $\to$ R) $\to$ \%}{\%}
\Params{
{\em f} & R $\to$ R & A map\\
{\em p} & \% & A monomial\\
}
\Descr{\name(f)(p) returns $f(a) x^n$ where $p = a x^n$,
while \name(f) returns the mapping $p \to f(p)$.
In both cases, \name!~does not make a copy of $p$ but modifies it in place.}
#endif
	monomial: (R, Z) -> %;
#if ALDOC
\alpage{monomial}
\Usage{\name(c, n)}
\Signature{(R, \altype{Integer})}{\%}
\Params{
{\em c} & R & A scalar\\
{\em n} & \altype{Integer} & An exponent\\
}
\Retval{Returns the monomial $c\; x^n$.}
#endif
	if R has FiniteCharacteristic then {
        	pthPower: % -> %;
        	pthPower!: % -> %;
#if ALDOC
\alpage{pthPower}
\altarget{\name!}
\Usage{ \name~p\\ \name!~p }
\Signature{\%}{\%}
\Params{ {\em p} & \% & A monomial\\ }
\Retval{Returns $p^{\mbox{characteristic}}$.}
\Remarks{\name!~does not make a copy of $p$, which is therefore
modified after the call. It is unsafe to use the variable $p$
after the call, unless it has been assigned to the result
of the call, as in {\tt p := pthPower!~p}.}
#endif
	}
	setCoefficient!: (%, R) -> R;
#if ALDOC
\alpage{setCoefficient!}
\Usage{\name(p, c)}
\Signature{(\%, R)}{R}
\Params{
{\em p} & \% & A monomial\\
{\em c} & R & A scalar\\
}
\Descr{Sets the coefficient of $p$ to $c$,\ie changes $p = d\; x^n$ into
$c\; x^n$}
\Retval{Returns the new coefficient $c$.}
\alseealso{\alexp{coefficient}}
#endif
	setDegree!: (%, Z) -> Z;
#if ALDOC
\alpage{setDegree!}
\Usage{\name(p, c)}
\Signature{(\%, \altype{Integer})}{\altype{Integer}}
\Params{
{\em p} & \% & A monomial\\
{\em n} & \altype{Integer} & An exponent\\
}
\Descr{Sets the degree of $p$ to $n$,\ie changes $p = c\; x^m$ into
$c\; x^n$}
\Retval{Returns the new degree $n$.}
\alseealso{\alexp{degree}}
#endif
} == add {
	Rep == Record(coef:R, expt:Z);
	import from Rep;

	local evar:TREE				== extree var;
	monomial(c:R, n:Z):%			== { assert(n>=0); per [c, n]; }
	coefficient(t:%):R     			== rep(t).coef;
	degree(t:%):Z    	  		== rep(t).expt;
	setDegree!(t:%, n:Z):Z			== rep(t).expt := n;
	setCoefficient!(t:%, c:R):R		== rep(t).coef := c;
	extree(p:%):TREE			== p evar;
	map(f:R -> R)(p:%):%	== monomial(f coefficient p, degree p);
	map!(f:R -> R)(p:%):%	== { setCoefficient!(p, f coefficient p); p }

	apply(p:%, t:TREE):TREE == {
		import from R, List TREE;
		zero?(c := coefficient p) => extree c;
		d := degree p;
		negative?(tc := extree c) =>
			ExpressionTreeMinus [tree(c ~= -1, negate tc, d, t)];
		tree(c ~= 1, tc, d, t);
	}

	local tree(tims?:Boolean, c:TREE, n:Z, t:TREE):TREE == {
		import from R, List TREE;
		zero? n => c;
		if n > 1 then t := ExpressionTreeExpt [t, extree n];
		tims? => ExpressionTreeTimes [c, t];
		t;
	}

	(u:%) = (v:%):Boolean == {
		degree u = degree v and coefficient u = coefficient v;
	}

	if R has FiniteCharacteristic then {
		pthPower(p:%):% ==
			monomial(pthPower(coefficient p)$R,
					characteristic$R * degree p);

		pthPower!(p:%):% == {
			rec := rep p;
			rec.coef := pthPower(rec.coef)$R;
			rec.expt := characteristic$R * rec.expt;
			p;
		}
	}
}

#if ALDORTEST
---------------------- test umonom.as --------------------------
#include "algebra"
#include "aldortest"

macro {
        Z == Integer;
	F == SmallPrimeField 11;
        M == UnivariateMonomial;
}

local double(a:Z):Z == a + a;

local degree():Boolean == {
        import from Z, M Z;
        p := monomial(2, 3);
	q := map(double)(p);
        degree p = 3 and coefficient p = 2 and
		degree q = degree p and coefficient q = 4;
}

local pthpower():Boolean == {
        import from MachineInteger, Z, F, M F;

        p := monomial(2*1, 3);
	q := pthPower p;
	degree p = 3 and coefficient p = 2*1 and
		degree q = characteristic$F * degree p
			and coefficient q = coefficient p;
}

stdout << "Testing umonom..." << endnl;
aldorTest("degree", degree);
aldorTest("pthpower", pthpower);
stdout << endnl;
#endif


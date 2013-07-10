----------------------------- sit_upolalg.as ----------------------------------
-- Copyright (c) Manuel Bronstein 2000
-- Copyright (c) INRIA 1999, Version 0.1.12
-- Logiciel Sum^it (c) INRIA 1999, dans sa version 0.1.12
-----------------------------------------------------------------------------

#include "algebra"

macro {
	I	== MachineInteger;
	Z	== Integer;
}

#if ALDOC
\thistype{UnivariatePolynomialRing}
\History{Manuel Bronstein}{16/6/2000}{separated from sit_uffalg.as}
\Usage{\this~R: Category}
\Params{
{\em R} & \altype{ExpressionType} & The coefficient domain\\
        & \altype{ArithmeticType} &\\
}
\Descr{\this~is a common category for commutative and noncommutative
univariate polynomials with coefficients in an arbitrary arithmetic
system R and with respect to the power basis $(x^n)_{n \ge 0}$.}
\begin{exports}
\category{\altype{UnivariateFreeRing} R}\\
\alexp{add!}: & (\%,R,Z,\%, Z, Z) $\to$ \% & In-place partial product and sum\\
\alexp{compose}: & (\%, \%) $\to$ \% & Compose polynomials\\
\alexp{translate}: & (\%, R) $\to$ \% & Translate a polynomial\\
\end{exports}
\begin{exports}[if $R$ has \altype{Parsable} then]
\category{\altype{Parsable}}\\
\end{exports}
\begin{aswhere}
Z &==& \altype{Integer}\\
\end{aswhere}
#endif

define UnivariatePolynomialRing(R:Join(ArithmeticType, ExpressionType)):
	Category == UnivariateFreeRing R with {
	if R has Parsable then Parsable;
	add!: (%, R, Z, %, Z, Z) -> %;
#if ALDOC
\alpage{add!}
\Usage{\name(p, c, m, q, n, N)}
\Signature{(\%,R,\altype{Integer},\%,\altype{Integer},\altype{Integer})}{\%}
\Params{
{\em p} & \% & A polynomial (to be destroyed)\\
{\em c} & R & A scalar\\
{\em m} & \altype{Integer} & The degree of the monomial to add\\
{\em q} & \% & A polynomial to be multiplied by $c x^m$ and added to p\\
{\em n} & \altype{Integer} & A lower threshold\\
{\em N} & \altype{Integer} & An upper treshold\\
}
\Descr{\name(p, c, m, q, n, N) computes all the terms of degree at least $n$
and at most $N$ of
$$
p + c x^m q = \sum_{i=0}^{d+m} (a_i + c b_{i-m}) x^i\,,
$$
where $p = \sum_{i=0}^d a_i x^i$ and $q = \sum_{i=0}^d b_i x^i$.
Note that $m$ is allowed to be negative.
For efficiency reasons it is sometimes sufficient to compute some terms of
that sum only. All other coefficients of $p$ are not changed.}
\Remarks{The storage used by p is allowed to be destroyed or reused, so p
is lost after this call. This may cause p to be destroyed, so do not use
this unless p has been locally allocated, and is thus guaranteed not to
share space with other polynomials. Some functions, like \alexp{reductum} are
not necessarily copying their arguments and can thus create memory aliases.}
#endif
	compose: (%, %) -> %;
	translate: (%, R) -> %;
#if ALDOC
\alpage{compose,translate}
\altarget{compose}
\altarget{translate}
\Usage{compose(p, q)\\ translate(p, r)}
\Params{
{\em p, q} & \% & Polynomials\\
{\em r} & R & Amount to translate\\
}
\Retval{compose(p, q) returns
$$
p(q) = \sum_{i=0}^n a_i q^i
$$
where $p = \sum_{i=0}^n a_i x^i$, while translate(p, r) returns $p(x - r)$.}
#endif
	default {
		local dummy:Symbol == { import from String; -"dummy"; }

		translate(p:%, r:R):% == {
			zero? r => p;
			compose(p, monom - r::%);
		}

		add!(p:%, c:R, n:Z, q:%):% ==
			add!(p, c, n, q, 0, max(degree p, degree q + n));

		apply(p:%, x:ExpressionTree):ExpressionTree == {
			import from Boolean, R, List ExpressionTree;
			import from UnivariateMonomial(R, dummy);
			zero? p => extree(0@R);
			l:List(ExpressionTree) := empty;
			for term in p repeat {
				m := monomial(term)@UnivariateMonomial(R,dummy);
				l := cons(m x, l);
			}
			assert(~empty? l);
			empty? rest l => first l;
			ExpressionTreePlus reverse! l;
		}

		compose(p:%, q:%):% == {
			import from Z;
			q = monom => p;
			pq:% := 0;
			zero? q => pq;
			qn:% := 1;
			d:Z := 0;
			for term in terms p repeat {	-- low to high
				(c, n) := term;
				for i in 1..n - d repeat
					zero?(qn := q * qn) => return pq;
				pq := add!(pq, c * qn);
				d := n;
			}
			pq;
		}

		if R has Parsable then {
			import from ExpressionTree;
			-- TEMPORARY: CONSTANT CAUSE RUNTIME BOMB (1173)
			-- local xtree:ExpressionTree	== extree monom;
			-- local xtree?:Boolean		== leaf? xtree;
			local xtree():ExpressionTree	== extree monom;
			local xtree?():Boolean		== leaf? xtree();

			eval(l:ExpressionTreeLeaf):Partial % == {
				import from R, Partial R;
				-- xtree? and l = leaf xtree => [monom];
				xtree?() and l = leaf xtree() => [monom];
				failed?(u := eval(l)$R) => failed;
				[retract(u)::%];
			}

			eval(p:MachineInteger,l:List ExpressionTree):Partial %==
			{
				import from R, Partial R, ParsingTools %;
				failed?(u := evalArith(p, l)) => {
					failed?(v := eval(p, l)$R) => failed;
					[retract(v)::%];
				}
				u;
			}
		}
	}
}

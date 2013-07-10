----------------------------- alg_polydio.as -----------------------------
--
-- Diophantine equation solving in R[X]
--
-- Copyright (c) Manuel Bronstein 2004
-- Copyright (c) INRIA 2004, Version 1.0.3
-- Logiciel Sum^it (c) INRIA 2004, dans sa version 1.0.3
-----------------------------------------------------------------------------

#include "algebra"

#if ALDOC
\thistype{UnivariatePolynomialDiophantineSolver}
\History{Manuel Bronstein}{29/7/2004}{created}
\Usage{import from \this(R, RX)}
\Params{
\emph{R} & \alexttype{IntegralDomain}{} & the coefficient ring\\
\emph{RX}
& \alexttype{UnivariatePolynomialAlgebra}{} R & polynomials over {\em R}\\
}
\Descr{\this(R, RX) implements diophantine equation solving in \emph{RX}.}
\begin{exports}
\alexp{pseudoDiophantine}: &
(RX, RX, RX) $\to$ \alexttype{Partial}{} \builtin{Cross}(R, RX, RX) &
equation solver\\
\end{exports}
#endif

UnivariatePolynomialDiophantineSolver(R:IntegralDomain,
				RX:UnivariatePolynomialAlgebra0 R): with {
	pseudoDiophantine: (RX, RX, RX) -> Partial Cross(R, RX, RX);
#if ALDOC
\alpage{pseudoDiophantine}
\Usage{\name(a, b, c)}
\Signature{(RX, RX, RX)}{\alexttype{Partial}{} \builtin{Cross}(R, RX, RX)}
\Params{ {\em a,b,c} & RX & Polynomials\\ }
\Retval{
Returns either $(r, x, y)$ such that $a x + b y = r c$ and
$x=0$ or $\deg(x) < \deg(b)$, or \failed.}
\Remarks{If \name{} returns \failed,
then the equation $a x + b y = r c$ has no solution
whenever \emph{R} is a field or $\gcd(a,b) \in R$.
If those two conditions are not met, then
a solution could have been missed.
Therefore, \name{} is
complete only when \emph{R} is a field or when
\emph{a} and \emph{b} have no common root.}
#endif
} == add {
	local gcd?:Boolean == R has GcdDomain;

	if R has GcdDomain then {
		local gcdsimp(r:R, u:RX, v:RX):(R, RX, RX) == {
			import from List R;
			(cu, pu) := primitive u;
			(cv, pv) := primitive v;
			(g, l) := gcdquo [r, cu, cv];
			unit? g => (r, u, v);
			(first l, times!(first rest l, pu),
						times!(first rest rest l, pv));
		}
	}

	-- returns either (r <> 0, u, v) s.t. r c = a u + b v or failed
	-- degree(u) < degree(b) if b is monic
	-- tries to take out the most of r out of u and v when possible
	if R has Field then {
		pseudoDiophantine(a:RX, b:RX, c:RX):Partial Cross(R,RX,RX) == {
			import from R, Partial Cross(RX, RX);
			failed?(ans := extendedEuclidean(a, b, c)) => failed;
			(u, v) := retract ans;
			[(1, u, v)];
		}
	}
	else {
		pseudoDiophantine(a:RX, b:RX, c:RX):Partial Cross(R,RX,RX) == {
			import from R, Partial RX;
			failed?(ans := pseudoDiophantine0(a, b, c)) => failed;
			(r, u, v) := retract ans;
			rr := r::RX;
			failed?(uu := exactQuotient(u, rr))
				or failed?(vv := exactQuotient(v, rr)) => {
					-- TEMPORARY:WANT LIKE TO CACHE THE TEST
					-- gcd? => [gcdsimp(r, u, v)];
					R has GcdDomain => [gcdsimp(r, u, v)];
					ans;
			}
			[(1, retract uu, retract vv)];
		}
	}

	-- returns either (r <> 0, u, v) s.t. r c = a u + b v or failed
	-- degree(u) < degree(b) if b is monic
	local pseudoDiophantine0(a:RX, b:RX, c:RX):Partial Cross(R, RX, RX) == {
		import from R;
		failed?(ans := pseudoDiophantine1(a, b, c)) => failed;
		(s, u, v) := retract ans;
		zero? u or ~unit?(leadingCoefficient b) => ans;
		(q, r) := monicDivide!(u, b);
		[(s, r, v + a * q)];
	}

	-- returns either (r <> 0, u, v) s.t. r c = a u + b v or failed
	local pseudoDiophantine1(a:RX, b:RX, c:RX):Partial Cross(R, RX, RX) == {
		import from Partial Cross(R, RX);
		zero? a => {
			failed?(pm := pseudomultiple(b, c)) => failed;
			(r, v) := retract pm;		-- r c = b v
			[(r, 0, v)];
		}
		zero? b => {
			failed?(pm := pseudomultiple(a, c)) => failed;
			(r, u) := retract pm;		-- r c = a u
			[(r, u, 0)];
		}
		(p, u, v) := lastsubresultant(a, b);	-- p = a u + b v
		failed?(pm := pseudomultiple(p, c)) => failed;
		(r, q) := retract pm;			-- r c = p q
		[(r, u * q, v * q)];
	}

	-- returns either (r <> 0, q) s.t. r c = p q or failed
	local pseudomultiple(p:RX, c:RX):Partial Cross(R, RX) == {
		import from Integer, R, Partial R;
		zero? p => {
			zero? c => [(1, 1)];
			failed;
		}
		lcpinv := reciprocal(lcp := leadingCoefficient p);
		monic? := ~failed? lcpinv;
		zero? degree p => {
			monic? => [(1, retract(lcpinv) * c)];
			[(lcp, c)];
		}
		(q, r) := {
			monic? => monicDivide(c, p);
			pseudoDivide(c, p);
		}
		zero? r => {
			monic? => [(1, q)];
			[(lcp^next(degree c - degree p), q)];
		}
		failed;
	}

	-- returns (p, u, v) s.t. p = a u + b v and p is the last subsresultant
	local lastsubresultant(a:RX, b:RX):(RX, RX, RX) == {
		import from Integer, Resultant(R, RX);
		degree a < degree b => {
			(p, v, u) := extendedLastSPRS(b, a);
			(p, u, v);
		}
		extendedLastSPRS(a, b);
	}
}

------------------------------- alg_serpoly.as ------------------------------
-- Copyright (c) Manuel Bronstein 2002
-- Copyright (c) INRIA 2002, Version 1.0
-- Logiciel Algebra (c) INRIA 2002, dans sa version 1.0
-----------------------------------------------------------------------------

#include "algebra"

macro {
	Z == Integer;
	DRX == DenseUnivariatePolynomial R;
	UPC == UnivariatePolynomialAlgebra;
	UTSC == UnivariateTaylorSeriesType;
}

#if ALDOC
\thistype{UnivariateTaylorSeriesNewtonSolver}
\History{Manuel Bronstein}{21/1/2002}{created}
\Usage{ import from \this(R, Rx, RxY)\\ import from \this(R, Rx, RX, RXY) }
\Params{
{\em R} & \altype{ExpressionType} & The coefficient domain\\
        & \altype{ArithmeticType} &\\
{\em Rx} & \altype{UnivariateTaylorSeriesType} R & Series over \emph{R}\\
{\em RxY}&\altype{UnivariatePolynomialAlgebra} Rx& Polynomials over \emph{Rx}\\
{\em RX} & \altype{UnivariatePolynomialAlgebra} R & Polynomials over \emph{R}\\
{\em RXY}&\altype{UnivariatePolynomialAlgebra} RX& Polynomials over \emph{RX}\\
}
\Descr{\this{} provides a Newton solver for computing roots in $R[[x]]$ of
polynomials in either $R[x,y]$ or $R[[x]][y]$.}
\begin{exports}[if \emph{R} has \altype{CommutativeRing} then]
\alexp{differentiate}: & RxY $\to$ RxY & Differentiation\\
\alexp{root}: & (RXY, R) $\to$ Rx & Root of a polynomial\\
               & (RxY, R) $\to$ Rx & \\
\end{exports}
\begin{exports}[if \emph{R} has \altype{FloatType} then]
\alexp{root}: & (RXY, RXY, R) $\to$ Rx & Root of a polynomial\\
              & (RxY, RxY, R) $\to$ Rx & \\
\end{exports}
#endif

UnivariateTaylorSeriesNewtonSolver(R:Join(ArithmeticType, ExpressionType),
					RXX:UTSC R, RXXY:UPC RXX): with {
#if ALDOC
\alpage{differentiate}
\Usage{\name~p}
\Signature{RxY}{RxY}
\Params{\emph{p} & RxY & a polynomial\\}
\Retval{Returns $\frac{dp}{dy}$.}

\alpage{root}
\Usage{\name($p, s_0$)\\ \name($p, p', s_0$)}
\Signatures{
\name: & (RXY, R) $\to$ Rx\\
\name: & (RxY, R) $\to$ Rx\\
\name: & (RXY, RXY, R) $\to$ Rx\\
\name: & (RxY, RxY, R) $\to$ Rx\\
}
\Params{
\emph{p} & RXY & a nonzero polynomial\\
         & RxY & \\
$p'$ & RXY & the derivative of \emph{p} with respect to \emph{y}\\
     & RxY & \\
$s_0$ & R & a simple root of $p(0,y)$\\
}
\Descr{Returns a series $s(x)$ such that $p(x,s(x)) = 0$ and $s(0) = s_0$.
The initial value $s_0$ must satisfy $p(0,s_0) = 0$, and in addition,
$\frac{dp}{dy}(0, s_0)$ must be a unit in \emph{R}.}
\Remarks{The parameter $p'$ must be given only when \emph{R} has
\altype{FloatType}, since differentiation is not available for
polynomials over such rings.}
#endif
	if R has CommutativeRing then {
		differentiate: RXXY -> RXXY;
		root: (RXXY, R) -> RXX;
	}
	if R has FloatType then {
		root: (RXXY, RXXY, R) -> RXX;
	}
} == add {

	if R has CommutativeRing then {
		differentiate(p:RXXY):RXXY == {
			import from Z, R, RXX;
			h:RXXY := 0;
			for term in p repeat {
				(c, n) := term;
				if n > 0 then h := add!(h, n::R::RXX*c, prev n);
			}
			h;
		}

		root(p:RXXY, g0:R):RXX == {
			import from Boolean, Z, RXX, Partial R;
			-- TEMPO: COMPILER BUG: 1.0 THINKS THAT RXXY HAS
			--        differentiate, WHICH IS NOT THE CASE
			pp := differentiate(p)$%;
			u := reciprocal coefficient(pp(g0::RXX), 0);
			assert(~failed? u);
			root(p, pp, g0, retract u);
		}
	}

	if R has FloatType then {
		root(p:RXXY, pp:RXXY, g0:R):RXX == {
			import from Boolean, Z, RXX;
			ppg0 := coefficient(pp(g0::RXX), 0);
			assert(~zero? ppg0);
			root(p, pp, g0, 1 / ppg0);
		}
	}

	-- computes s such that p(s) = 0 and s0 = g0
	-- must have pp = p', p(g0) = 0 mod x, and s0 p'(g0) = 1 mod x
	local root(p:RXXY, pp:RXXY, g0:R, s0:R):RXX == {
		import from Z, Stream R, Sequence R;
		assert(zero? coefficient(p(g0::RXX), 0));
		assert(one?(s0 * coefficient(pp(g0::RXX), 0)));
		series sequence [rootGen(p, pp, g0, s0)];
	}

	-- PROBABLY CAUSES TROUBLE IN CHARACTERISTIC 2
	-- computes a series g such that p(g) = 0 and g(0) = g0
	-- must have pp = p', p(g0) = 0 mod x, and s0 p'(g0) = 1 mod x
	local rootGen(p:RXXY, pp:RXXY, g0:R, s0:R):Generator R == generate {
		import from Z, DRX,
			UnivariateTaylorSeriesType2Poly(R, RXX, DRX);
		yield g0;
		g := g0::DRX;
		s := s0::DRX;
		n:Z := 1;	-- p(g) = 0 mod x^n
		two:R := 1+1;
		repeat {
			m := 2 * n;
			pgs := minus! truncate!(truncate(p, g, m) * s, m);
			g := add!(pgs, g);
			gs2 := minus! truncate!(truncate(pp, g, m) * s^2, m);
			s := add!(gs2, two, s);
			for i in n..prev m repeat yield coefficient(g, i);
			n := m;
		}
	}

	-- returns p(g) mod x^n
	-- TEMPORARY: INEFFICIENT FOR NOW, SHOULD TRUNCATE DURING HORNER
	local truncate(p:RXXY, g:DRX, n:Z):DRX == {
		macro DRXY == DenseUnivariatePolynomial DRX;
		import from DRXY, UnivariateFreeRing2(RXX, RXXY, DRX, DRXY),
				UnivariateTaylorSeriesType2Poly(R, RXX,DRX);
		truncate!((map((s:RXX):DRX +-> truncate(s, n))(p)) g, n);
	}
}
 
UnivariateTaylorSeriesNewtonSolver(R:Join(ArithmeticType, ExpressionType),
					RXX:UTSC R, RX:UPC R, RXY:UPC RX):with {
	-- root(p, g0) returns a series g st p(g) = 0 and g(0) = g0
	-- provided that p(g0) = 0 mod x and p'(g0) invertible mod x
	if R has CommutativeRing then {
		root: (RXY, R) -> RXX;
	}
	if R has FloatType then {
		root: (RXY, RXY, R) -> RXX;
	}
} == add {
	if R has CommutativeRing then {
		root(p:RXY, g0:R):RXX == {
			import from Boolean, Z, RX, RXX, Partial R;
			pp := differentiate p;
			u := reciprocal coefficient(pp(g0::RX), 0);
			assert(~failed? u);
			root(p, pp, g0, retract u);
		}
	}

	if R has FloatType then {
		root(p:RXY, pp:RXY, g0:R):RXX == {
			import from Boolean, Z, RX, RXX;
			ppg0 := coefficient(pp(g0::RX), 0);
			assert(~zero? ppg0);
			root(p, pp, g0, 1 / ppg0);
		}
	}

	-- computes s such that p(s) = 0 and s0 = g0
	-- must have pp = p', p(g0) = 0 mod x, and s0 p'(g0) = 1 mod x
	local root(p:RXY, pp:RXY, g0:R, s0:R):RXX == {
		import from Z, RX, Stream R, Sequence R;
		assert(zero? coefficient(p(g0::RX), 0));
		assert(one?(s0 * coefficient(pp(g0::RX), 0)));
		series sequence [rootGen(p, pp, g0, s0)];
	}

	-- PROBABLY CAUSES TROUBLE IN CHARACTERISTIC 2
	-- computes a series g such that p(g) = 0 and g(0) = g0
	-- must have pp = p', p(g0) = 0 mod x, and s0 p'(g0) = 1 mod x
	local rootGen(p:RXY, pp:RXY, g0:R, s0:R):Generator R == generate {
		import from Z,RX,UnivariateTaylorSeriesType2Poly(R,RXX,RX);
		yield g0;
		g := g0::RX;
		s := s0::RX;
		n:Z := 1;	-- p(g) = 0 mod x^n
		two:R := 1+1;
		repeat {
			m := 2 * n;
			pgs := minus! truncate!(truncate(p g, m) * s, m);
			g := add!(pgs, g);
			gs2 := minus! truncate!(truncate(pp g, m) * s^2, m);
			s := add!(gs2, two, s);
			for i in n..prev m repeat yield coefficient(g, i);
			n := m;
		}
	}
}
 

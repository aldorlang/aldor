------------------------ sit_uflgqot.as ---------------------------
-- Copyright (c) Manuel Bronstein 1996
-- Copyright (c) INRIA 1999, Version 0.1.12
-- Logiciel Sum^it ©INRIA 1999, dans sa version 0.1.12
-- Copyright (c) Swiss Federal Polytechnic Institute Zurich, 1996
-----------------------------------------------------------------------------

#include "algebra"

#if ALDOC
\thistype{UnivariateFreeRingOverFraction}
\History{Manuel Bronstein}{25/4/96}{created}
\Usage{import from \this(R, PR, Q, PQ)}
\Params{
{\em R} & \astype{IntegralDomain} & an integral domain\\
{\em PR} & \astype{UnivariateFreeRing} R &
a univariate free finite algebra type over R\\
{\em Q} & \astype{FractionCategory} R & a fraction domain of R\\
{\em PQ} & \astype{UnivariateFreeRing} R &
a univariate free finite algebra type over Q\\
}
\Descr{\this(R, PR, Q, PQ) provides useful conversions between
polynomials with integral and rational coefficients.}
\begin{exports}
\category{\astype{LinearCombinationFraction}(R, PR, Q, PQ)}\\
\end{exports}
#endif

UnivariateFreeRingOverFraction(R:IntegralDomain,
	PR:UnivariateFreeRing R,
	Q:FractionCategory R, PQ:UnivariateFreeRing Q):
		LinearCombinationFraction(R, PR, Q, PQ) == add {
	-- All those functions use the fact that IntegralDomain's are
        -- commutative (if fractions of noncommutative domains are used
        -- one day, those functions must be recoded, as they will give
	-- incorrect results if R is not commutative).

	(x:Q) * (p:PR):PQ == {
		q:PQ := 0;
		for term in p repeat {
			(c, n) := term;
			q := add!(q, c * x, n);
		}
		q;
	}

	makeRational(p:PR):PQ == {
		import from Q;
		q:PQ := 0;
		for term in p repeat {
			(c, n) := term;
			q := add!(q, c::Q, n);
		}
		q;
	}

	if Q has FractionFieldCategory0 R then {
		normalize(p:PR):(R, PQ) == {
			import from Q;
			zero? p => (1, 0);
			a := leadingCoefficient p;
			q:PQ := monomial degree p;
			for term in reductum p repeat {
				(c, n) := term;
				q := add!(q, c / a, n);
			}
			(a, q);
		}
	}

	macro gcd? == R has GcdDomain;
	local denom(c:Q, n:Integer):R	== denominator c;

	local prod(l:List R):R == {
		r:R := 1;
		while ~empty? l repeat {
			r := times!(r, first l);
			l := rest l;
		}
		r;
	}

	makeIntegral(p:PQ):(R, PR) == {
		import from R, Q, List R, List Q;
		denoms := [denom term for term in p];
		d := { gcd? => lcm(denoms)$R; prod denoms; }
		q:PR := 0;
		for term in p repeat {
			(c, n) := term;
			q := add!(q, numerator(d * c), n);
		}
		(d, q);
	}

	if Q has FractionByCategory0 R then {
		makeIntegralBy(p:PQ):(Integer, PR) == {
			import from R, Q, List R, List Q;
			mu:Integer := 0;
			for term in p repeat {
				(c, n) := term;
				m:Integer := order(c)$Q;
				if m < mu then mu := m;
			}
			assert(mu <= 0);
			q:PR := 0;
			for term in p repeat {
				(c, n) := term;
				q := add!(q, numerator shift(c, -mu), n);
			}
			(mu, q);
		}
	}
}

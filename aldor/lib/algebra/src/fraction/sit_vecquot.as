------------------------ sit_vecquot.as ---------------------------
-- Copyright (c) Manuel Bronstein 1998
-- Copyright (c) INRIA 1999, Version 0.1.12
-- Logiciel Sum^it ©INRIA 1999, dans sa version 0.1.12
-----------------------------------------------------------------------------

#include "algebra"

macro {
	I == MachineInteger;
	V == Vector;
}

#if ALDOC
\thistype{VectorOverFraction}
\History{Manuel Bronstein}{13/7/98}{created}
\Usage{import from \this(R, Q)}
\Params{
{\em R} & \astype{IntegralDomain} & an integral domain\\
{\em Q} & \astype{FractionCategory} R & a fraction domain over R\\
}
\Descr{\this(R, Q) provides useful conversions between
vectors with integral and rational coefficients.}
\begin{exports}
\category{
\astype{LinearCombinationFraction}(R,\astype{Vector} R,Q,\astype{Vector} Q)}\\
\end{exports}
#endif

VectorOverFraction(R:IntegralDomain, Q:FractionCategory R):
	LinearCombinationFraction(R, V R, Q, V Q) == add {
	macro gcd? == R has GcdDomain;

	-- Most of those functions use the fact that IntegralDomain's are
        -- commutative (if fractions of noncommutative domains are used
        -- one day, those functions must be recoded, as they will give
	-- incorrect results if R is not commutative).

	(x:Q) * (v:V R):V Q == {
		import from I;
		w:V Q := zero(n := #v);
		for i in 1..n repeat w.i := v.i * x;
		w;
	}

	makeRational(v:V R):V Q == {
		import from I, Q;
		w:V Q := zero(n := #v);
		for i in 1..n repeat w.i := (v.i)::Q;
		w;
	}

	if Q has FractionFieldCategory0 R then {
		normalize(v:V R):(R, V Q) == {
			import from I, Q;
			w:V Q := zero(n := #v);
			zero? n => (1, w);
			a := v.1;
			for i in 1..n repeat w.i := v.i / a;
			(a, w);
		}
	}

	local prod(l:List R):R == {
		r:R := 1;
		while ~empty? l repeat {
			r := times!(r, first l);
			l := rest l;
		}
		r;
	}

	makeIntegral(w:V Q):(R, V R) == {
		import from I, R, Q, List R;
		v:V R := zero(n := #w);
		l:List R := [denominator(w.i) for i in 1..n];
		d := { gcd? => lcm(l); prod l; }
		for i in 1..n repeat v.i := numerator(d * w.i);
		(d, v);
	}

	if Q has FractionByCategory0 R then {
		makeIntegralBy(w:V Q):(Integer, V R) == {
			import from I, Boolean, Q;
			v:V R := zero(n := #w);
			mu:Integer := 0;
			for j in 1..n | ~zero?(b := w.j) repeat {
				m:Integer := order(b);
				if m < mu then mu := m;
			}
			assert(mu <= 0);
			for i in 1..n repeat v.i := numerator shift(w.i, -mu);
			(mu, v);
		}
	}
}

------------------------ sit_qotient.as ---------------------------
-- Copyright (c) Laurent Bernardin 1994
-- Copyright (c) INRIA 1999, Version 0.1.12
-- Logiciel Sum^it (c) INRIA 1999, dans sa version 0.1.12
-- Copyright (c) Swiss Federal Polytechnic Institute Zurich, 1994
-----------------------------------------------------------------------------

#include "algebra"

macro Z == Integer;

#if ALDOC
\thistype{Fraction}
\History{Laurent Bernardin}{1/12/94}{created}
\History{Manuel Bronstein}{6/6/96}{made auto-normalizing, algos from Knuth}
\Usage{import from \this~R}
\Params{ {\em R} & \altype{GcdDomain} & a gcd domain\\ }
\Descr{
\this~R forms the fraction field of the gcd domain {\em R}. Fractions
are automatically normalized in this field.
}
\begin{exports}
\category{\altype{FractionFieldCategory} R}\\
\end{exports}
#endif

Fraction(R:GcdDomain): FractionFieldCategory R == add {
	Rep == Record(Numer:R, Denom:R);

	local canon?:Boolean		== canonicalUnitNormal?$R;
	local mkquot(a:R, b:R):%	== { import from Rep; per [a, b]; }
	numerator(x:%):R		== { import from Rep; rep(x).Numer; }
	denominator(x:%):R		== { import from Rep; rep(x).Denom; }
	local numden(x:%):(R, R)	== { import from Rep; explode rep x; }
	0:%				== { import from R; mkquot(0, 1); }
	1:%				== { import from R; mkquot(1, 1); }
	coerce(a:R):%			== mkquot(a, 1);
	coerce(a:Z):%			== a::R::%;
	normalize(x:%):%		== x;
	characteristic:Z		== characteristic$R;

	extree(a:%):ExpressionTree == {
		import from R, List ExpressionTree;
		zero? a => extree(0@R);
		tnum := extree numerator a;
		one? denominator a => tnum;
		tden := extree denominator a;
		-- move the minus sign out of the numerator to the front
		negative? tnum => {
			t := ExpressionTreeQuotient [negate tnum, tden];
			ExpressionTreeMinus [t];
		}
		ExpressionTreeQuotient [tnum, tden];
	}

	local canon(n:R, d:R):% == {
		TRACE("qotient::canon: n = ", n);
		TRACE("qotient::canon: d = ", d);
		assert(~zero? d);
		TRACE("qotient::canon: gcd(n, d) = ", gcd(n, d));
		assert(unit? gcd(n, d));
		canon? => {
			-- TEMPORARY: CANNOT OVERLOAD (BUG 1272)
			-- (newd, newn) :=  unitNormal(d, n);
			(newd, newn) :=  unitNormalize(d, n);
			TRACE("qotient::canon: newn = ", newn);
			TRACE("qotient::canon: newd = ", newd);
			TRACE("", "");
			mkquot(newn, newd);
		}
		mkquot(n, d);
	}

	-(x:%):% == {
		import from R;
		(n, d) := numden x;
		zero? n => x;
		mkquot(- n, d);
	}

	(x:%)^(n:Z):% == {
		import from R;
		one? x or zero? n => 1;
		zero? x or one? n => x;
		(u, v) := numden x;
		n < 0 => canon(v^(-n), u^(-n));
		canon(u^n, v^n);
	}

	(x:%) = (y:%):Boolean == {
		import from R;
		(u, v) := numden x;
		(w, t) := numden y;
		u = w and v = t => true;
		canon? => false;
		u * t = v * w;
	}

	(x:%) * (y:%):% == {
		(u, v) := numden x;
		(w, t) := numden y;
		crossgcd(u, v, w, t);
	}

	(x:%) / (y:%):% == {
		(u, v) := numden x;
		TRACE("qotient::/: u = ", u);
		TRACE("qotient::/: v = ", v);
		(w, t) := numden y;
		TRACE("qotient::/: w = ", w);
		TRACE("qotient::/: t = ", t);
		crossgcd(u, v, t, w);
	}

	-- computes (u/v) * (w/t) by crossing gcd's
	local crossgcd(u:R, v:R, w:R, t:R):% == {
		zero? u or zero? w => 0;
		assert(unit? gcd(u, v));
		assert(unit? gcd(w, t));
		unit? v => {
			unit? t => canon(u * w, v * t);
			(d, u1, t1) := gcdquo(u, t);
			canon(u1 * w, v * t1);
		}
		(e, v1, w1) := gcdquo(v, w);
		unit? t => canon(u * w1, v1 * t);
		(d, u1, t1) := gcdquo(u, t);
		canon(u1 * w1, v1 * t1);
	}

	inv(a:%):% == {
		(n, d) := numden a;
		canon(d, n);
	}

	(a:R) / (b:R):%	== {
		assert(~zero? b);
		zero? a => 0;
		one? b => a::%;
		(g, a1, b1) := gcdquo(a, b);
		canon(a1, b1);
	}

	(c:R) * (x:%):% == {
		zero? c or zero? x => 0;
		one? c => x;
		(a, b) := numden x;
		(g, c1, b1) := gcdquo(c, b);
		mkquot(c1 * a, b1);
	}

	(x:%) - (y:%):% == {
		zero? x => - y; zero? y => x;
		addsub(x, y, false);
	}

	(x:%) + (y:%):% == {
		zero? x => y; zero? y => x;
		addsub(x, y, true);
	}

	local addsub(x:%, y:%, add?:Boolean):% == {
		(u, v) := numden x;
		(w, t) := numden y;
		unit? v or unit? t => {
			s := { add? => u * t + v * w; u * t - v * w; }
			zero? s => 0;
			canon(s, v * t);
		}
		(d, v1, t1) := gcdquo(v, t);
		s := { add? => u * t1 + v1 * w; u * t1 - v1 * w; }
		zero? s => 0;
		one? d => canon(s, v * t);
		(e, s1, d1) := gcdquo(s, d);
		canon(s1, v1 * t1 * d1);
	}

	lift(D:Derivation R):Derivation(%) == {
		derivation {(f:%):% +-> {
			(n, d) := numden f;
			dn := D n;
			zero?(dd := D d) => dn / d;
			(g, u, v) := gcdquo(D d, d);	-- d'/d = u/v
			a := v * dn - n * u;		-- f' = a / (d v)
			one? g => mkquot(a, v * d);
			(e, s, t) := gcdquo(a, g);
			canon(s, v * v * t);
		} }
	}
}		

#if ALDORTEST
---------------------- test sit_qotient.as --------------------------
#include "algebra"
#include "aldortest"

macro {
	Z   == Integer;
	Q   == Fraction Z;
}

rational():Boolean == {
	import from Z, Q;

	b := h := 0@Z;
	while zero? h repeat h := random()$Z;
	while zero? b repeat b := h * random()$Z;
	a := h * random()$Z;
	g := gcd(a, b);
	x := a / b;
	g * numerator x = a and g * denominator x = b;
}

stdout << "Testing sit__qotient..." << newline;
aldorTest("rational", rational);
stdout << newline;

#endif


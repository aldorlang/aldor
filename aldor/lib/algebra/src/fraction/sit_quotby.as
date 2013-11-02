---------------------------- sit_quotby.as -------------------------------
-- Copyright (c) Manuel Bronstein 1996
-- Copyright (c) INRIA 1999, Version 0.1.12
-- Logiciel Sum^it ©INRIA 1999, dans sa version 0.1.12
-- Copyright (c) Swiss Federal Polytechnic Institute Zurich, 1996
-----------------------------------------------------------------------------

#include "algebra"

macro Z == Integer;

#if ALDOC
\thistype{FractionBy}
\History{Manuel Bronstein}{7/6/96}{created}
\Usage{import from \this(R, p, irr?)}
\Params{
{\em R} & \astype{IntegralDomain} & an integral domain\\
{\em p} & R & a nonzero nonunit of R\\
{\em irr?} & \astype{Boolean} &
Indicates whether p is known to be irreducible in R\\
}
\Descr{
\this(R, p, irr?) forms the fractions of the integral domain {\em R} by the
nonzero nonunit {\em p}, \ie the set of all fractions whose denominator is a
power of {\em p}. Fractions are normalized in the sense that {\em p}
does not divide the numerators. Indicating whether {\em p} is
irreducible if for efficiency purposes only, always use \false~when
it is unknown.
}
\begin{exports}
\category{\astype{FractionByCategory} R}\\
\end{exports}
#endif

FractionBy(R: IntegralDomain, p:R, irr?:Boolean): FractionByCategory R == {
	-- sanity checks on the parameters
	assert(~zero? p);
	assert(~unit? p);

	add {
	-- Keeps normalized, ie exactQuotient(Numerator, p) = failed
	-- value is Numer p^Order
	Rep == Record(Numer:R, Order:Z);
	import from String;

	local gcd?:Boolean		== R has GcdDomain;
	local mkquot(a:R, n:Z):%	== { import from Rep; per [a, n]; }
	order(x:%):Z			== { import from Rep; rep(x).Order; }
	local numord(x:%):(R, Z)	== { import from Rep; explode rep x; }
	0:%				== { import from Z, R; mkquot(0, 0); }
	1:%				== { import from Z, R; mkquot(1, 0); }
	coerce(n:Z):%			== { import from R; canon(n::R, 0); }
	coerce(a:R):%			== { import from Z; canon(a, 0); }
	local orderpquo: R -> (Z, R)	== orderquo p;

	random():% == {
		import from Z;
		canon(random()$R, (random()$Z rem 101) - 50);
	}

	numerator(x:%):R == {
		import from Z;
		(a, n) := numord x;
		n < 0 => a;
		assert(n >= 0);
		a * p^n;
	}

	denominator(x:%):R == {
		import from Z;
		(a, n) := numord x;
		n < 0 => p^(-n);
		1;
	}

	-- returns (c, k) s.t. b divides a q^k,
	-- returns k = -1 if there are no such nonnegative k
	-- if q and b are coprime (in particular whenever q is irreducible),
	-- this is equivalent to b divides a
	-- over a gcd domain if q and b are not coprime, this is
	-- equivalent to b/g divides a g^k for some k >= 0 where g = gcd(b, q)
	local powexquo(b:R, a:R, q:R, irrq?:Boolean):(R, Z) == {
		import from Partial R;
		gcd? => gcdpowexquo(b, a, q, irrq?);
		~failed?(u := exactQuotient(a, b)) => (retract u, 0);
		irrq? => (0, -1);
		-- TEMPORARY HACK: tries only whether b divides a p
		~failed?(u := exactQuotient(b, a * p)) => (retract u, 1);
		-- TEMPORARY: THIS IS FALSE!
		(0, -1);
	}

	-- b p^m divides a p^n if and only if b divides a p^k for some k >= 0
	exactQuotient(x:%, y:%):Partial % == {
		TRACE("fractionby::exactQuotient, p = ", p);
		import from Z, Partial R;
		(a, n) := numord x;
		TRACE("a = ", a); TRACE("n = ", n);
		(b, m) := numord y;
		TRACE("b = ", b); TRACE("m = ", m);
		e := n - m;
		TRACE("e = ", e);
		(c, k) := powexquo(b, a, p, irr?);
		k < 0 => {
			gcd? => failed;
			-- TEMPORARY HACK: tries whether b divides a p^|e|
			(ee := abs e) > 2 and
				~failed?(u := exactQuotient(a * p^ee, b)) => {
					e > 0 => [retract(u)::%];
					(c, q) := numord(retract(u)::%);
					[mkquot(c, e + e + q)];
			}
			error "FractionBy::exactQuotient - cannot conclude"
		}
		[mkquot(c, e - k)];
	}

	if R has GcdDomain then {
		normalize(x:%):% == x;

		local gcdpowexquo(b:R, a:R, q:R, irrq?:Boolean):(R, Z) == {
			import from Partial R;
			~failed?(u := exactQuotient(a, b)) => (retract u, 0);
			irrq? => (0, -1);
			(g, bb, qq) := gcdquo(b, q)$R;	-- TEMPO: 1.1.11e BUG
			unit? g => (0, -1);
			(h, k) := powexquo(bb, a, g, false);
			k < 0 => (0, -1);
			(h * qq^(next k), next k);
		}
	}
	else {
		normalize(x:%): % == error "Normalize on non GCD Domain";
	}

	shift(x:%, n:Z):% == {
		(a, m) := numord x;
		zero? a or zero? n => x;
		mkquot(a, n + m);
	}

	-(x:%):% == {
		(a, n) := numord x;
		zero? a => 0;
		mkquot(- a, n);
	}

	(x:%)^(n:Z):% == {
		zero? x or one? x or zero? n => 1;
		one? n => x;
		assert(n > 1);
		(a, m) := numord x;
		mkquot(a^n, n * m);
	}

	(x:%) = (y:%):Boolean == {
		(a, n) := numord x;
		(b, m) := numord y;
		n = m and a = b;
	}

	(x:%) * (y:%):% == {
		zero? x or zero? y => 0;
		(a, n) := numord x;
		(b, m) := numord y;
		irr? => mkquot(a * b, n + m);
		(c, q) := numord((a * b)::%);
		mkquot(c, n + m + q);
	}

	(c:R) * (x:%):% == {
		zero? c or zero? x => 0;
		one? c => x;
		(a, n) := numord x;
		irr? => {
			(b, m) := numord canon(c, n);
			mkquot(a * b, m);
		}
		canon(c * a, n);
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
		import from Z;
		(a, n) := numord x;
		(b, m) := numord y;
		n < m => {
			bp := b * p^(m-n);
			s := { add? => a + bp; a - bp; }
			mkquot(s, n);
		}
		n > m => {
			ap := a * p^(n-m);
			s := { add? => ap + b; ap - b; }
			mkquot(s, m);
		}
		s := { add? => a + b; a - b; }
		canon(s, n);
	}

	local canon(a:R, n:Z):% == {
		TRACE("FractionBy::canon: ", a);
		TRACE("p^", n);
		zero? a => 0;
		(m, a) := orderpquo a;
		TRACE("FractionBy::canon: returning ", a);
		TRACE("p^", n + m);
		mkquot(a, n + m);
	}

	local liftPconstant(D:Derivation R):Derivation(%) == {
		derivation {(f:%):% +-> {
			(a, n) := numord f;
			canon(D a, n);
		}}
	}

	lift(D:Derivation R):Derivation(%) == {
		zero?(dp := D p) => liftPconstant D;
		-- p'/p = dppa p^dppn
		(dppa, dppn) := numord canon(dp, -1);
		derivation {(f:%):% +-> {
			(a, n) := numord f;
			da := D a;
			zero? n => da::%;
			assert(~zero? dppa);
			local b:R; local m:Z;
			-- compute b,m s.t. a p'/p = b p^m,  p \nodiv b
			if irr? then {
				b := a * dppa;
				m := dppn;
			}
			else (b, m) := numord canon(a * dppa, dppn);
			m >= 0 => canon(da + n * b * p^m, n);
			mkquot(da * p^(-m) + n * b, n + m);
		} }
	}

--	if R has OrderedRing then {
--		(x:%) > (y:%):Boolean == {
--			(a, n) := numord x;
--			(b, m) := numord y;
--			n > m or (n = m and a >$R b);	-- TEMPO: 1.1.11e BUG
--		}
--	}

	if R has FiniteCharacteristic then {
		pthPower(x:%):% == {
			(a, n) := numord x;
			ap := pthPower(a)$R;	-- TEMPO: 1.1.11e BUG
			np := characteristic$R * n;
			irr? => mkquot(ap, np);
			canon(ap, np);
		}
	}
	}		
}

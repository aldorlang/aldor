----------------------------- sit_spread.as ----------------------------------
-- Copyright (c) Manuel Bronstein 1994
-- Copyright (c) INRIA 1999, Version 0.1.12
-- Logiciel Sum^it (c) INRIA 1999, dans sa version 0.1.12
-- Copyright (c) Swiss Federal Polytechnic Institute Zurich, 1994-97
-----------------------------------------------------------------------------

#include "algebra"

macro {
	Z == Integer;
	ORBIT == Cross(RX, List Z);
}

UnivariatePolynomialSpread(R:Join(RationalRootRing, GcdDomain),
				RX:UnivariatePolynomialAlgebra R,
				RXY:UnivariatePolynomialAlgebra RX): with {
	integerDistances: RX -> List Z;
	integerDistances: (RX, RX) -> List Z;
} == add {
	-- the elements of l must all be irreducible
	local orbitalComp(l:List RX):(List RX, RX, List Z) == {
		import from Z, Partial Z;
		p := first l;
		d := degree p;
		assert(d > 0);
		orb:List Z := [0];
		l := rest l;
		ll:List RX := empty;
		for q in l repeat {
			if failed?(s := irrSpread(p, q)) then ll := cons(q, ll);
			else {
				m := retract s;		-- p(n) ~ q(n+m)
				assert(m ~= 0);
				orb := cons(m, orb);
				if m < 0 then {
					p := q;
					orb := [e - m for e in orb];
				}
			}
		}
		(ll, p, orb);
	}

	if R has FactorizationRing then {
		local first(p:RX, e:Z):RX == p;

		-- p must be squarefree already
		local orbitalDecomposition(p:RX):List ORBIT == {
			import from Boolean, Product RX;
			orb:List ORBIT := empty;
			(c, prod) := factor p;
			l:List RX := [first term for term in prod];
			while ~empty? l repeat {
				(l, q, pr) := orbitalComp l;
				orb := cons((q, pr), orb);
			}
			orb;
		}

		integerDistances(p:RX):List Z ==
			spread orbitalDecomposition squareFreePart p;

		integerDistances(p:RX, q:RX):List Z ==
			spread(orbitalDecomposition squareFreePart p,
				orbitalDecomposition squareFreePart q);
	}
	else {
		integerDistances(p:RX):List Z == {
			pp := squareFreePart p;
			spread(pp, pp);
		}

		integerDistances(p:RX, q:RX):List Z ==
			spread(squareFreePart p, squareFreePart q);
	}

	local searchOrbit(b:List ORBIT, p:RX):(MachineInteger, List Z, Z) == {
		import from Boolean, Partial Z, List Z;
		i:MachineInteger := 0;
		for term in b repeat {
			i := next i;
			(q, qorb) := term;
			~failed?(s := irrSpread(p, q)) => {
				m := retract s;		-- p(n) ~ q(n+m)
				return(i, qorb, m);
			}
		}
		(0, empty, 0);
	}

	local spread(l:List Z):Set Z == {
		import from Z;
		spr:Set Z := empty;
		for r in l repeat for rr in l repeat spr := union!(spr, r - rr);
		spr;
	}

	local spread(a:List ORBIT):List Z == {
		import from MachineInteger;
		spr:Set Z := empty;
		for term in a repeat {
			(p, porb) := term;
			spr := union!(spr, spread porb);
		}
		[n for n in spr];
	}

	local spread(la:List Z, lb:List Z, e:Z):Set Z == {
		spr:Set Z := empty;
		for a in la repeat for b in lb repeat
			spr := union!(spr, a - b + e);
		spr;
	}

	local spread(a:List ORBIT, b:List ORBIT):List Z == {
		import from MachineInteger;
		spr:Set Z := empty;
		for term in a repeat {
			(p, porb) := term;
			(bpos, qorb, e) := searchOrbit(b, p);
			if bpos > 0 then {
				spr := union!(spr, spread(porb, qorb, e));
				empty?(b := delete!(b, bpos)) =>
					return [n for n in spr];
			}
		}
		[n for n in spr];
	}

	-- one-candidate version, use when p and q are known to be irreducible
	local irrSpread(p:RX, q:RX):Partial Z == {
		import from Boolean, Z, R, Partial R;
		assert(~zero? p); assert(~zero? q);
		assert(degree p > 0); assert(degree q > 0);
		(d := degree p) ~= degree q => failed;
		d1 := prev d;
		lp := leadingCoefficient p;
		lq := leadingCoefficient q;
		x:RX := monom;
		failed?(u := exactQuotient(lq, lp)) => {
			failed?(u := exactQuotient(lp, lq)) => failed;
			alpha := retract u;
			failed?(u := exactQuotient(coefficient(p, d1)
				- alpha * coefficient(q,d1), d::R * alpha * lq))
					=> failed;
			failed?(mz := integer(RX)(m := retract u)) => failed;
			zero?(alpha * q(x + m::RX) - p) => mz;
			failed;
		}
		alpha := retract u;
		failed?(u := exactQuotient(alpha * coefficient(p, d1)
				- coefficient(q, d1), d::R * lq)) => failed;
		failed?(mz := integer(RX)(m := retract u)) => failed;
		zero?(q(x + m::RX) - alpha * p) => mz;
		failed;
	}

	-- resultant version, use when p and q are not known to be irreducible
	local spread(p:RX, q:RX):List Z == {
		import from Boolean, Z, RXY;
		import from FractionalRoot Z, List FractionalRoot Z;
		import from UnivariateFreeRing2(R, RX, RX, RXY);
		assert(~zero? p); assert(~zero? q);
		f:RX -> RXY := map((c:R):RX +-> c::RX);
		xpm := monom$RXY + (monom$RX)::RXY;
		r := resultant(f p, compose(f q, xpm));
		[integralValue rt for rt in integerRoots(r)$RX]
	}
}

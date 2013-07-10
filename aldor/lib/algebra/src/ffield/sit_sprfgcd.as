--------------------------- sit_sprfgcd.as ------------------------------
-- Copyright (c) Manuel Bronstein 2000
-- Copyright (c) INRIA 2000, Version 0.1.12
-- Logiciel Sum^it ©INRIA 2000, dans sa version 0.1.12
-----------------------------------------------------------------------------

#include "algebra"

macro {
	I == MachineInteger;
	Z == Integer;
	A == PrimitiveArray;
}

SmallPrimeFieldCategoryGcd(F:SmallPrimeFieldCategory0,
				FX:UnivariatePolynomialAlgebra0 F): with {
	gcdSPF: (FX, FX) -> FX;
	gcdquoSPF: (FX, FX) -> (FX, FX, FX);
} == add {
	local charac:Z	== characteristic$F;
	-- TEMPORARY: TERRIBLE 1.1.11e/12 COMPILER BUG (1181?)
	-- local schar:I	== machine charac;

	local array(p:FX, d:I):A I == {
		import from Z, F;
		a := new(next d, 0);
		for term in p repeat {
			(c, e) := term;
			a(d - machine e) := machine c;
		}
		a;
	}

	gcdSPF(p:FX, q:FX):FX == {
		import from I, A I, Z, F, ModulopUnivariateGcd;
		zero? p => monic q;
		zero? q => monic p;
		zero?(dp := degree p) or zero?(dq := degree q) => 1;
		-- TEMPORARY: TERRIBLE 1.1.11e/12 COMPILER BUG (1181?)
		if dp < dq then {	-- swap p,q
			ap := array(q, ep := machine dq);
			aq := array(p, eq := machine dp);
		}
		else {
			ap := array(p, ep := machine dp);
			aq := array(q, eq := machine dq);
		}
		assert(ep >= eq);
		schar:I := machine charac;
		(log, exp, log?) := discreteLogTable$F;
		(v, d, s) := {
			log? => gcd!(ap, ep, aq, eq, 1, schar, log, exp);
			gcd!(ap, ep, aq, eq, 1, schar);
		}
		-- monic gcd is now in v(s),v(s+1),...
		g:FX := 0;
		for i in 0..d repeat g := add!(g, v(s+i)::F, (d-i)::Z);
		g;
	}

	gcdquoSPF(p:FX, q:FX):(FX, FX, FX) == {
		g := gcdSPF(p, q);
		(g, quotient(p, g), quotient(q, g));
	}
}

------------------------------- optbug.as -----------------------------------
--
-- Yet another optimization bug that occurs when specialize! becomes inlined
--
-- aldor -q1 -fx -lalgebra -laldor optbug.as
-- optbug
-- Calling rank()...T
--
-- aldor -q2 -fx -lalgebra -laldor optbug.as
-- optbug
-- Segmentation fault
--
-- This bug also illustrates a second compile-time seg fault:
-- aldor -q1 -wdebug optbug.as
-- Program fault (segmentation violation).
--

#include "algebra"
#include "aldorio"

macro {
	I == MachineInteger;
	Z == Integer;
	M == DenseMatrix;
	A == PrimitiveArray;
}

Special(R:Join(CommutativeRing, Specializable), M:MatrixCategory R): with {
	rankLowerBound: M -> Partial Cross(Boolean, I);
} == add {
	rankLowerBound(m:M):Partial Cross(Boolean, I) == {
		import from Boolean, I, A I, A A I, LazyHalfWordSizePrimes;
		(r, c) := dimensions m;
		mp:A A I := new r;
		for i in 0..prev r repeat mp.i := new c;
		for i in 1..2 repeat {
			p := randomPrime();
			~failed?(u := rankLowerBound(m, mp, p)) => return u;
		}
		failed;
	}

	local rankLowerBound(m:M, mp:A A I, p:I):Partial Cross(Boolean, I) == {
		import from I, ModulopGaussElimination;
		(r, c) := dimensions m;
		maxrank := min(r, c);
		for i in 1..2 repeat {
			specialize!(mp, m, p) => {
				rk := rank!(mp, r, c, p);
				return [(rk = maxrank, rk)];
			}
		}
		failed;
	}

	local specialize!(mp:A A I, m:M, p:I):Boolean == {
		macro F == SmallPrimeField0 p;
		import from I, A I, A A I, F, Partial F, PartialFunction(R, F);
		(r, c) := dimensions m;
		sigma := partialMapping(specialization(F)$R);
		for i in 1..r repeat {
			rowi := mp(prev i);
			for j in 1..c repeat {
				failed?(u := sigma m(i, j)) => return false;
				rowi(prev j) := machine retract u;
			}
		}
		true;
	}
}

local rank():Boolean == {
	import from I, Z, M Z, Partial Cross(Boolean, I);
	import from Special(Z, M Z);
	m:M Z := zero(4, 4);
	m(1, 2) := -1;
	m(1, 4) := 1;
	m(3, 4) := 1;
	m(4, 4) := -1;
	u := rankLowerBound m;
	failed? u => false;
	(b?, r) := retract u;
	r = 2 and ~b?;
}

stdout << "Calling rank()..." << rank() << newline;
stdout << endnl;


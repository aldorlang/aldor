---------------------------- alg_ffupla.as ---------------------------------
-- Copyright (c) Manuel Bronstein 2002
-- Copyright (c) INRIA 2002, Version 1.0.1
-- Logiciel Algebra (c) INRIA 2002, dans sa version 1.0.1
-----------------------------------------------------------------------------

#include "algebra"

macro {
	I == MachineInteger;
	V == Vector FX;
	ARR == PrimitiveArray;
	ELIM == TwoStepFractionFreeGaussElimination(FX, M);
	POPOV == UnivariatePolynomialPopovLinearAlgebra(F, FX, M);
	MF == DenseMatrix F;
}

FiniteFieldUnivariatePolynomialLinearAlgebra(
	F: FiniteField,
	FX:UnivariatePolynomialAlgebra0 F,
	M: MatrixCategory FX): with {
		determinant: M -> FX;
		inverse: M -> (M, V);
		kernel: M -> M;
		linearDependence: (Generator V, I) -> V;
		linearDependenceViaKernel: (Generator V, I) -> V;
		maxInvertibleSubmatrix: M -> (Array I, Array I);
		particularSolution: (M, M) -> (M, V);
		rank: M -> I;
		solve: (M, M) -> (M, M, V);
		span: M -> Array I;
} == add {
	-- TEMPORARY: Only do span, rank, det, maxInvertibleSubmatrix
	-- and kernel from Popov for now,
	-- this type is a placeholder for eventual efficient implementations
	-- redispatch the other funcs everything to FF2GE in the meantime
	span(m:M):Array I			== span(m)$POPOV;
	determinant(m:M):FX			== determinant(m)$POPOV;

	local eval0(m:M, r:I, c:I):MF == {
		import from Integer, FX;
		m0:MF := zero(r, c);
		for i in 1..r repeat for j in 1..c repeat
			m0(i, j) := coefficient(m(i, j), 0);
		m0;
	}

	maxInvertibleSubmatrix(m:M):(Array I, Array I) == {
		import from LinearAlgebra(F, MF), I;
		(r, c) := dimensions m;
		m0 := eval0(m, r, c);
		(r0, c0) := maxInvertibleSubmatrix m0;
		r = #r0 or c = #c0 => (r0, c0);
		maxInvertibleSubmatrix(m)$POPOV;
	}

	rank(m:M):I == {
		import from LinearAlgebra(F, MF);
		(r, c) := dimensions m;
		m0 := eval0(m, r, c);
		(r0 := rank(m0)) = min(r, c) => r0;
		rank(m)$POPOV;
	}

	kernel(m:M):M == {
		import from I, OverdeterminedLinearSystemSolver(FX, M);
		(r, c) := dimensions m;
		zero? c => zero(0, 0);
		if zero? r then m := zero(1, c);
		kernel!(copy m, nullspace!);
	}

	local nullspace!(m:M):M == {
		import from I, LinearAlgebra(F, MF);
		(r, c) := dimensions m;
		if r >= c then {	-- check for full rank at 0
			m0 := eval0(m, r, c);
			(r0 := rank(m0)) = c => return zero(c, 0);
		}
		kernel(m)$POPOV;
	}

	local firstcols(a:M, n:I, s:I, gen:Generator V):M == {
		assert(s <= next n);
		s = next n => {
			l:List V := append!([v for v in columns a], next! gen);
			[generator l];
		}
		a(1,1,n,s);
	}

	-- for benchmarking purposes
	-- will replace linearDependence if found better
	linearDependenceViaKernel(gen:Generator V, n:I):V == {
		import from LinearAlgebra(FX, M);
		a:M := [v for v in gen for i in 1..n];
		(rank?, r) := rankLowerBound a;
		b := firstcols(a, n, s := next r, gen);
		k: M := kernel(b)$%;
		while zero?(numberOfColumns k) and s <= next n repeat {
			b := firstcols(a, n, s := next s, gen);
			k := kernel(b)$%;
		}
		assert(one?(numberOfColumns k) and s <= next n);
		column(k, 1);
	}

	-- Code from here on is just copied from LinearAlgebra
	inverse(m:M):(M, V)			== inverse! copy m;
	particularSolution(a:M, b:M):(M, V)	== psol!(copy a, copy b);

	linearDependence(gen:Generator V, n:I):V == {
		import from ARR I, Backsolve(FX, M);
		(a,p,r,d) := dependence(gen, n)$ELIM;
		st:ARR I:= new r;       -- ignore st(0)
		for i in 1..prev r repeat st(i) := i;
		first(backsolve(a,p,st,prev r,r), r);
	}

	local first(w:V, n:I):V == {
		assert(n <= #w);
		n = #w => w;
		v := zero n;
		for i in 1..n repeat v.i := w.i;
		v;
	}

	local inverse!(a:M):(M, V) == {
		import from Backsolve(FX, M);
		assert(square? a);
		(p,r,st,d,w) := extendedRowEchelon!(a)$ELIM;
		backsolve(a,p,st,r,w);
	}

	solve(a:M, b:M):(M, M, V) == {
		import from I;
		(ra, ca) := dimensions a;
		(rb, cb) := dimensions b;
		assert(ra = rb);
		if zero? ra then { a := zero(1, ca); b := zero(1, cb) }
		solve!(copy a, copy b);
	}

	local solve!(a:M, b:M):(M, M, V) == {
		import from Backsolve(FX, M);
		(p,r,st,d) := rowEchelon!(a, b)$ELIM;
		(psol, den) := backsolve(a,p,st,r,b);
		(null!(a, p, r, st), psol, den);
	}

	local null!(a:M, p:I->I, r:I, st:ARR I):M == {
		import from Backsolve(FX, M);
		zero? r => one numberOfColumns a;
		k:List V := empty;
		(n, m) := dimensions a;
		for j in 1..prev(st 1) repeat
			k := cons(backsolve(a,p,st,0,j),k);
		for i in 1..prev r repeat
			for j in next(st i)..prev st(next i) repeat
				k:=cons(backsolve(a,p,st,i,j),k);
		if r > 0 then for j in next(st r)..m repeat
					k:=cons(backsolve(a,p,st,r,j),k);
		[v for v in k];
	}

	local psol!(a:M, b:M):(M, V) == {
		import from Backsolve(FX, M);
		(p,r,st,d) := rowEchelon!(a, b)$ELIM;
		backsolve(a,p,st,r,b);
	}

}

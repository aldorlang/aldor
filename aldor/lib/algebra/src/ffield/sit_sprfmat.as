--------------------------- sit_sprfmat.as ------------------------------------
-- Copyright (c) Manuel Bronstein 2000
-- Copyright (c) INRIA 2000, Version 0.1.12
-- Logiciel Sum^it ©INRIA 2000, dans sa version 0.1.12
-----------------------------------------------------------------------------

#include "algebra"

macro {
	Z == MachineInteger;
	A == PrimitiveArray Z;
	M == PrimitiveArray A;
	V == Vector F;
}

-- Do not use the log table for small prime, since the lazy strategy is better
SmallPrimeFieldCategoryLinearAlgebra(F:SmallPrimeFieldCategory0,
					MF:MatrixCategory F): with {
	determinant: MF -> F;
	inverse: MF -> (MF, V);
	kernel: MF -> MF;
	linearDependence: (Generator V, Z) -> V;
	maxInvertibleSubmatrix: MF -> (Array Z, Array Z);
	particularSolution: (MF, MF) -> (MF, V);
	rank: MF -> Z;
	solve: (MF, MF) -> (MF, MF, V);
	span: MF -> Array Z;
} == add {
	local charac:Integer	== characteristic$F;
	-- TEMPORARY: TERRIBLE 1.1.11e/12 COMPILER BUG (1181?)
	-- local schar:Z	== machine charac;

	local array(m:MF, r:Z, c:Z):M == {
		import from A, F;
		a := array(r, c);
		c1 := prev c;
		for i in 0..prev r repeat for j in 0..c1 repeat
			a.i.j := machine m(next i, next j);
		a;
	}

	local array(r:Z, c:Z):M == {
		import from A;
		a:M := new r;
		for i in 0..prev r repeat a.i := new(c, 0);
		a;
	}

	local matrix(a:M, r:Z, c:Z):MF == {
		import from A, F;
		m:MF := zero(r, c);
		for i in 1..r repeat for j in 1..c repeat
			m(i, j) := a(prev i)(prev j)::F;
		m;
	}

	local vector(a:A, n:Z):V == {
		import from F;
		v:V := zero n;
		for i in 1..n repeat v.i := a(prev i)::F;
		v;
	}

	local genp(g:Generator V, n:Z, a:A):Generator A == generate {
		import from F, V;
		for v in g repeat {
			assert(n = #v);
			for i in 1..n repeat a(prev i) := machine(v.i);
			yield a;
		}
	}

	linearDependence(g:Generator V, n:Z):V == {
		import from A, M, F, ModulopGaussElimination;
		-- TEMPORARY: TERRIBLE 1.1.11e/12 COMPILER BUG (1181?)
		schar := machine charac;
		a:A := new n;
		work := array(n, n1 := next n);
		ans := array(n1, 1);
		r := firstDependence!(genp(g, n, a), n, schar, work, ans);
		assert(r <= n1);
		v:V := zero r;
		for i in 1..r repeat v.i := ans(prev i)(0)::F;
		v;
	}

	determinant(m:MF):F == {
		import from Z, ModulopGaussElimination;
		assert(square? m);
		n := numberOfRows m;
		a := array(m, n, n);
		-- TEMPORARY: TERRIBLE 1.1.11e/12 COMPILER BUG (1181?)
		schar := machine charac;
		determinant!(a, n, schar)::F;
	}

	inverse(m:MF):(MF, V) == {
		import from Z, A, ModulopGaussElimination;
		assert(square? m);
		n := numberOfRows m;
		a := array(m, n, n);
		b := array(n, n);
		d:A := new n;
		-- TEMPORARY: TERRIBLE 1.1.11e/12 COMPILER BUG (1181?)
		schar := machine charac;
		inverse!(a, n, schar, b, d);
		(matrix(b, n, n), vector(d, n));
	}

	kernel(m:MF):MF == {
		import from Z, ModulopGaussElimination;
		TRACE("sprfmat::kernel: m = ", m);
		(r, c) := dimensions m;
		TRACE("sprfmat::kernel: r = ", r);
		TRACE("sprfmat::kernel: c = ", c);
		a := array(m, r, c);
		b := array(c, c);
		-- TEMPORARY: TERRIBLE 1.1.11e/12 COMPILER BUG (1181?)
		schar := machine charac;
		TRACE("sprfmat::kernel: schar = ", schar);
		k := kernel!(a, r, c, schar, b);
		matrix(b, c, k);
	}

	maxInvertibleSubmatrix(m:MF):(Array Z, Array Z) == {
		import from Z, ModulopGaussElimination;
		(r, c) := dimensions m;
		a := array(m, r, c);
		-- TEMPORARY: TERRIBLE 1.1.11e/12 COMPILER BUG (1181?)
		schar := machine charac;
		maxInvertibleSubmatrix!(a, r, c, schar);
	}

	particularSolution(m:MF, q:MF):(MF, V) == {
		import from Z, A, ModulopGaussElimination;
		(r, c) := dimensions m;
		a := array(m, r, c);
		assert(r = numberOfRows q);
		cq := numberOfColumns q;
		b := array(q, r, cq);
		w := array(c, cq);
		d:A := new cq;
		-- TEMPORARY: TERRIBLE 1.1.11e/12 COMPILER BUG (1181?)
		schar := machine charac;
		particularSolution!(a, r, c, b, cq, schar, w, d);
		(matrix(w, c, cq), vector(d, cq));
	}

	rank(m:MF):Z == {
		import from Z, ModulopGaussElimination;
		(r, c) := dimensions m;
		a := array(m, r, c);
		-- TEMPORARY: TERRIBLE 1.1.11e/12 COMPILER BUG (1181?)
		schar := machine charac;
		rank!(a, r, c, schar);
	}

	span(m:MF):Array Z == {
		import from Z, ModulopGaussElimination;
		(r, c) := dimensions m;
		a := array(m, r, c);
		-- TEMPORARY: TERRIBLE 1.1.11e/12 COMPILER BUG (1181?)
		schar := machine charac;
		span!(a, r, c, schar);
	}

	solve(m:MF, q:MF):(MF, MF, V) == {
		import from Z, A, ModulopGaussElimination;
		(r, c) := dimensions m;
		a := array(m, r, c);
		kern := array(c, c);
		assert(r = numberOfRows q);
		cq := numberOfColumns q;
		b := array(q, r, cq);
		w := array(c, cq);
		d:A := new cq;
		-- TEMPORARY: TERRIBLE 1.1.11e/12 COMPILER BUG (1181?)
		schar := machine charac;
		k := solve!(a, r, c, b, cq, schar, w, d, kern);
		(matrix(kern, c, k), matrix(w, c, cq), vector(d, cq));
	}
}

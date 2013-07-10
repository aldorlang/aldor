------------------------- sit_overdet.as --------------------------
-- Copyright (c) Manuel Bronstein 1998
-- Copyright (c) INRIA 1999, Version 0.1.12
-- Logiciel Sum^it ©INRIA 1999, dans sa version 0.1.12
-----------------------------------------------------------------------------

#include "algebra"

#if ALDOC
\thistype{OverdeterminedLinearSystemSolver}
\History{Manuel Bronstein}{8/6/98}{created}
\Usage{import from \this(R, M)}
\Params{
{\em R} & \astype{IntegralDomain} & A domain\\
{\em M} & \astype{MatrixCategory} R & A matrix type over R\\
}
\Descr{\this(R, M) provides a solver for overdetermined linear algebraic
equations with coefficients in R.}
\begin{exports}
\asexp{kernel!} & (M, M $\to$ M) $\to$ M & Solve an overdetermined system\\
\end{exports}
#endif

macro {
	I	== MachineInteger;
	V	== Vector;
}

OverdeterminedLinearSystemSolver(R:IntegralDomain, M: MatrixCategory R): with {
		kernel!: (M, M -> M) -> M;
#if ALDOC
\aspage{kernel!}
\Usage{\name(m, f)}
\Signature{(M, M $\to$ M)}{M}
\Params{
{\em m} & M & A matrix\\
{\em f} & M $\to$ M & Computes a kernel\\
}
\Retval{Returns a basis for the kernel of m,
uses a specialized algorithm if m is overdetermined,
uses f when the system is no longer overdetermined.}
\Remarks{Can destroy m.}
#endif
} == add {
	local gcd?:Boolean == R has GcdDomain;

	local prim!(m:M):M == {
		gcd? => gcdprim! m;
		m;
	}

	if R has GcdDomain then {
		local gcdprim!(m:M):M == {
			import from I;
			for i in 1..numberOfColumns m repeat prim!(m, i);
			m;
		}

		-- makes the j-th column of m primitive
		local prim!(m:M, j:I):() == {
			import from R;
			n := numberOfRows m;
			assert(n > 0);
			g := m(1,j);
			for i in 2..n repeat {
				unit? g => return;
				g := gcd(g, m(i,j));
			}
			if ~unit? g then for i in 1..n repeat
				m(i,j) := quotient(m(i,j), g);
		}
	}

	-- split into odd and even rows
	local split(a:M):(M, M) == {
		import from I;
		(r, c) := dimensions a;
		assert(r >= c + c); assert(c > 0);
		r2 := r quo 2;
		m := { odd? r => next r2; r2 }
		o:M := zero(m, c);
		e:M := zero(r2, c);
		for i in 1..r2 repeat {
			i2 := i + i;
			for j in 1..c repeat {
				o(i, j) := a(prev i2, j);
				e(i, j) := a(i2, j);
			}
		}
		if odd? r then for j in 1..c repeat o(m, j) := a(r, j);
		(o, e);
	}

	kernel!(a:M, nullsp!:M -> M):M == {
		import from I;
		TIMESTART;
		(r, c) := dimensions a;
		TRACE("overdet::kernel!: # of equations = ", r);
		TRACE("overdet::kernel!: # of unknowns = ", c);
		assert(c > 0);
		r < c + c => nullsp! a;
		(a, b) := split a;
		TIME("overdet::kernel!: split at ");
		k := kernel!(a, nullsp!);
		TIME("overdet::kernel!: half-kernel at ");
		zero? numberOfColumns k or zero?(b := b * k) => k;
		-- at this point k is not the true kernel,
		-- but a x = 0 is equivalent to x = k y and b y = 0
		kb := kernel!(b, nullsp!);
		TIME("overdet::kernel!: y-kernel at ");
		zero? numberOfColumns kb => kb;
		k := k * kb;
		TIME("overdet::kernel!: x-kernel at ");
		prim! k;
	}
}

---------------------------- sit_hensela.as ------------------------------------
-- Copyright (c) Helene Prieto 2000
-- Copyright (c) INRIA 2000, Version 0.1.12
-- Logiciel Sum^it (c) INRIA 2000, dans sa version 0.1.12
-----------------------------------------------------------------------------

#include "algebra"

macro {
	I == MachineInteger;
	Z == Integer;
	V == Vector FX;
	Fx == Fraction FX;
	VU == Vector Fx;
}

UnivariatePolynomialHenselLinearAlgebra(F:Field,
		FX:UnivariatePolynomialAlgebra F,
		M:MatrixCategory FX): with {
	determinant: M -> Partial FX;
	particularSolution: (M, V) -> Partial VU;
} == add {
	local divideByX(p:FX):FX	== { import from Z; shift(p,-1); }
	local multiplyByX(p:FX):FX	== { import from Z; shift(p, 1); }
	local multiplyByX!(p:FX):FX	== { import from Z; shift!(p,1); }
	local val0(p:FX):F		== { import from Z; coefficient(p,0); }

	determinant(A:M):Partial FX == {
		import from I, Z, FX, Fx, Partial VU;
		import from VectorOverFraction(FX, Fx);
		import from UnivariatePolynomialCRTLinearAlgebra(F, FX, M);
		n := numberOfRows A;
		assert(n = numberOfColumns A); assert(n > 0);
		b:V := zero n;
		for i in 1..n repeat b.i := random 1;	-- try linear rhs
		b.n := 1;				-- make sure b <> 0
		failed?(u := particularSolution(A, b)) => failed;
		y := retract u;
		(d, ignore) := makeIntegral y;
		determinant(A, d);
	}

	particularSolution(A:M, b:V):Partial VU == {
		import from I, Z, F, FX, Fx, Partial Fx;
		TRACE("hensela::particularSolution: A = ", A);
		TRACE("hensela::particularSolution: b = ", b);
		(n, m) := dimensions A;
		(sol?, s, k, l, N, D, w, Q) := padicSolution(A, b, n, m);
		TRACE("hensela::particularSolution: s = ", s);
		TRACE("hensela::particularSolution: k = ", k);
		TRACE("hensela::particularSolution: l = ", l);
		TRACE("hensela::particularSolution: w = ", w);
		TRACE("hensela::particularSolution: N = ", N);
		TRACE("hensela::particularSolution: D = ", D);
		sol? => {
			v:VU := zero m;
			xk := monomial(k::Z);
			xml := inv(monomial(l::Z)::Fx);
			for j in 1..s repeat {
				res := ratrecon(w.j, xk, N, D);
				failed? res => return failed;
				v(Q j) := xml * retract res;
			}
			[v];
		}
		failed;
	}

	local padicSolution(A:M, b:V, n:I, m:I):(Boolean,I,I,I,I,I,V,I->I) == {
		import from F, FX, Z;

		local D:I; local N:I;

		dA := degree A;
		db := degree b;

		B:M := zero(n, m);
		c := copy b;
		w:V := zero m;
		P:Permutation n := 1;
		Q:Permutation m := 1;
		s := k := l := 0@I;

		repeat {
			for j in 1..s repeat {
				t := val0(c.j);
				w.j := add!(w.j, t, k::Z);
				-- substract constant multiples of the first s
				-- colums of B from c, to make the first s
				-- entries of c divisible by x.
				for r in 1..n repeat c.r := c.r - t * B(r,j);
			}

			i := next s;
			while i <= n and zero? val0(c.i) repeat i := next i;
			-- if all the entries of c equal 0 mod x we can
			-- improve the approximation by dividing c by x and
			-- incrementing k.  Then restart from the beginning.
			if i > n then {
				c := map!(divideByX)(c);
				k := next k;
				zero?(s) or k > N+D =>
					return(true, s, k, l, N, D,w,mapping Q);
			}
			-- otherwise we switch rows s+1 and s+i (cols 1 to s)
			-- with i the index of the first non zero entry of c;
			else {
				sb := next s;
				if i > sb then {
					rowSwap!(B, sb, i, 1, s);
					{ tmp:=c.sb; c.sb:=c.i; c.i:=tmp; }
					P := transpose!(P, sb, i);
				}
				for j in sb..m repeat B(sb,j) := A(P sb, Q j);
				cont?:Boolean := true;
				while cont? repeat {
					-- Here we substract constant multiples
					-- of the first s rows from (s+1)^{th}
					-- row to make its first s entries
					-- divisible by x.
					for ii in 1..s repeat {
						t := val0 B(sb,ii);
						c.sb := c.sb - t * c.ii;
						rowCombine!(B,1,sb, -t::FX, ii);
					}
					j := sb;
					while j <= m and zero? val0 B(sb,j)
							repeat j := next j;					
					cont? := j > m;
					-- if all entries in row s+1 of B are
					-- divisible by x, we multiply c and w
					-- by x, divide the (s+1)^{th} row by x
					-- and increment l.
					if cont? then {
						c := map!(multiplyByX)(c);
						w := map!(multiplyByX!)(w);
						c.sb := divideByX(c.sb);
						for r in 1..m repeat
						    B(sb,r):=divideByX B(sb,r);
						l := next l;
						l > sb * dA =>
							return(false, s, k,l,N,
								D, w,mapping Q);
					}
				}
				-- switch columns j and (s+1) of B (rows 1 to s)
				if j > sb then {
					Q := transpose!(Q, sb, j);
					colSwap!(B, sb, j, 1, s);
				}
				for ii in sb+1..n repeat B(ii,sb):=A(P ii,Q sb);
				-- Multiply the (s+1)^{th} row by a constant to
				-- make B(sb,sb) equal to 1 mod x. 
				t := val0 B(sb,sb);
				assert(~zero? t);
				it := inv t;
				c.sb := it * c.sb;
				for r in 1..m repeat B(sb,r) := it * B(sb,r);
				-- Substract constant multiples of row (s+1)
				-- from the first s rows to make the first s
				-- entries in column (s+1) divisible by x.
				for ii in 1..s repeat {
					t := val0 B(ii,sb);
					c.ii := c.ii - t * c.sb;
					rowCombine!(B, 1, ii, -t::FX, sb);
				}
				-- increment s and restart from the top
				s := sb;
				N := (s-1) * dA + db;
				D := s * dA - l;
			}
		}
		never;
	}

	local ratrecon(u:FX, p:FX,N:I,D:I): Partial Fx == {
		import from Z, Fx, Partial Cross(FX, FX);
		failed?(v := rationalReconstruction(u,p,N::Z,D::Z)) => failed;
		(t1, t0) := retract v;
		[t1/t0];
	}

	local degree(a:M):I == {
		import from Boolean, Z, FX;
		maxdeg:I := 0;
		(n, m) := dimensions a;
		for i in 1..n repeat for j in 1..m repeat {
			if ~zero?(b := a(i, j))
				and (d := machine degree b) > maxdeg then
					maxdeg := d;
		}
		maxdeg;
	}

	local degree(v:V):I == {
		import from Boolean, Z, FX;
		maxdeg:I := 0;
		for b in v | ~zero? b repeat {
			if (d := machine degree b) > maxdeg then maxdeg := d;
		}
		maxdeg;
	}
}


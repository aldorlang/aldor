---------------------------- sit_oge.as ------------------------------------
-- Copyright (c) Thom Mulders 1996
-- Copyright (c) INRIA 1999, Version 0.1.12
-- Logiciel Sum^it ©INRIA 1999, dans sa version 0.1.12
-- Copyright (c) Swiss Federal Polytechnic Institute Zurich, 1996
-----------------------------------------------------------------------------

#include "algebra"

macro {
	I == MachineInteger;
	ARR == PrimitiveArray;
	Sn == Permutation n;
}

#if ALDOC
\thistype{OrdinaryGaussElimination}
\History{Thom Mulders}{8 July 96}{created}
\Usage{import from \this(F,M)}
\Params{
{\em F} & \astype{Field} & A coefficient field\\
{\em M} & \astype{MatrixCategory} F & A matrix type over F\\
}
\begin{exports}
\category{\astype{LinearEliminationCategory}(R,M)}\\
\end{exports}
\Descr{
This domain implements ordinary Gaussian elimination on matrices.
}
#endif

OrdinaryGaussElimination(F:Field, M:MatrixCategory F):
	LinearEliminationCategory(F, M) == add {

	rowEchelon!(a:M,b:M):(I->I,I,ARR I,I) == rowEch!(a,b,numberOfRows a);

	local rowEch!(a:M,b:M,n:I): (I->I,I,ARR I,I) == {
		import from F, Sn;

		TRACE("OGE::rowEch!: a = ", a);
		TRACE("OGE::rowEch!: b = ", b);
		TRACE("OGE::rowEch!: n = ", n);
		ma := numberOfColumns a; mb := numberOfColumns b;
		assert(n=numberOfRows b);
		p:Sn := 1;
		st:ARR I := new(next n, next ma);	-- ignore st(0)

		r:I := 1;
		while r<=n for c in 1..ma repeat {
			l := pivot(a, mapping p, c, r);
			if l<=n then {
				st(r) := c;
				p := transpose!(p, l, r);
				arcinv := inv(a(p(r),c));
				for i in next(r)..n repeat {
					f := a(p(i),c)*arcinv;
					for j in c+1..ma repeat
						a(p(i),j) := _
							a(p(i),j)-f*a(p(r),j);
					for j in 1..mb repeat
						b(p(i),j) := _
							 b(p(i),j)-f*b(p(r),j);
				}
				r := next r;
			}
		}
		(mapping p,prev r,st,sign p);
	}

	denominators(a:M,p:I->I,r:I,st:ARR I): ARR F == {
		import from F;
		den:ARR F := new(next r,1);	-- ignore den.0
		den;
	}

	deter(a:M,p:I->I,r:I,d:I): F == {
		n := numberOfRows a;
		assert(n=numberOfColumns a);
		r<n => 0;
		det:F := { d=1 => 1; -1 }
		for i in 1..r repeat det := det*a(p(i),i);
		det;
	}

	dependence(gen:Generator Vector F,n:I): (M,I->I,I,F) == {
		import from Boolean, Vector F, Sn;
		a:M := zero(n,n+1);
		p:Sn := 1;
		r:I := 1;
		independent := true;
		while independent for v in gen repeat {
			for i in 1..n repeat a(i,r) := v(i);
			for j in 1..prev r repeat {
				for i in next(j)..n repeat {
				       a(p(i),r):=a(p(i),r)-a(p(i),j)*a(p(j),r);
				}
			}
			l := pivot(a, mapping p, r, r);
			independent := l <= n;
			if independent then {
				p := transpose!(p, l, r);
				inv := inv(a(p(r),r));
				for i in next r..n repeat
					a(p(i),r) := a(p(i),r)*inv;
				r := next r;
			}
		}
		(a,mapping p,r,1@F);
	}
}


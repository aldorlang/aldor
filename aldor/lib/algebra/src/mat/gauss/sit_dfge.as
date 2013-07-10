------------------------------ sit_dfge.as ------------------------------------
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
\thistype{DivisionFreeGaussElimination}
\History{Thom Mulders}{8 July 96}{created}
\Usage{import from \this(R,M)}
\Params{
{\em R} & \altype{CommutativeRing} & A coefficient ring\\
{\em M} & \altype{MatrixCategory} R & A matrix type over R\\
}
\begin{exports}
\category{\altype{LinearEliminationCategory}(R,M)}\\
\end{exports}
\Descr{
This domain implements division--free Gaussian elimination on matrices.
}
#endif

DivisionFreeGaussElimination(R:CommutativeRing, M:MatrixCategory R):
	LinearEliminationCategory(R,M) == add {

	rowEchelon!(a:M,b:M):(I->I,I,ARR I,I) == rowEch!(a,b,numberOfRows a);

	local rowEch!(a:M,b:M,n:I): (I->I,I,ARR I,I) == {

		import from R, Sn;

		ma := numberOfColumns a; mb := numberOfColumns b;
		assert(n=numberOfRows b);
		p:Sn := 1;
		st:ARR I := new(next n, next ma);       -- ignore st(0)

		r:I := 1;
		while r<=n for c in 1..ma repeat {
			l := pivot(a, mapping p, c, r);
			if l<=n then {
				st(r) := c;
				p := transpose!(p, l, r);
				arc:=a(p(r),c);
				for i in next(r)..n repeat {
					aic := a(p(i),c);
					for j in next(c)..ma repeat
						a(p(i),j):= arc*a(p(i),j) _
								-aic*a(p(r),j);
					for j in 1..mb repeat
						b(p(i),j):=arc*b(p(i),j) _
								-aic*b(p(r),j);
				}
				r := next r;
			}
		}
		(mapping p, prev r, st, sign p);
	}

	if R has IntegralDomain then {
		denominators(a:M,p:I->I,r:I,st:ARR I): ARR R == {
			den:ARR R := new(next r);	-- ignore den.0
			q1:R := 1;
			q2:R := 1;
			for i in 1..min(2,r) repeat den(i) := a(p(i),st(i));
			for i in min(2,r)+1..r repeat {
				q1 := q1*a(p(i-2),st(i-2));
				q2 := q2*q1;
				den(i) := quotient(a(p(i),st(i)),q2);
			}
			den;
		}

		deter(a:M,p:I->I,r:I,d:I): R == {
			n := numberOfRows a;
			assert(n=numberOfColumns a);
			r<n => 0;
			det := { d=1 => a(p(r),r); -a(p(r),r); }
			q1:R := 1;
			q2:R := 1;
			for i in 1..r-2 repeat {
				q1 := q1*a(p(i),i);
				q2 := q2*q1;
			}
			quotient(det,q2);
		}

		dependence(gen:Generator Vector R, n:I): (M,I->I,I,R) == {
			import from Boolean, Vector R, Sn;
			a:M := zero(n,n+1);
			p:Sn := 1;
			r:I := 1;
			independent := true;
			while independent for v in gen repeat {
				for i in 1..n repeat a(i,r) := v(i);
				for j in 1..prev r repeat {
					ajj := a(p(j),j);
					for i in next j..n repeat {
						a(p(i),r):=ajj*a(p(i),r) _
							-a(p(i),j)*a(p(j),r);
					}
				}
				l := pivot(a, mapping p, r, r);
				independent := l <= n;
				if independent then {
					p := transpose!(p, l, r);
					r := next r;
				}
			}
			q1:R := 1;
			q2:R := 1;
			for i in 1..r-3 repeat {
				q1 := q1*a(p(i),i);
				q2 := q2*q1;
			}
			(a,mapping p,r,quotient(a(p(r-1),r-1),q2));
		}
	}
}

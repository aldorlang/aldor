---------------------------- sit_ffge.as ------------------------------------
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
\thistype{FractionFreeGaussElimination}
\History{Thom Mulders}{8 July 96}{created}
\Usage{import from \this(R,M)}
\Params{
{\em R} & \astype{IntegralDomain} & A coefficient ring\\
{\em M} & \astype{MatrixCategory} R & A matrix type over R\\
}
\begin{exports}
\category{\astype{LinearEliminationCategory}(R,M)}\\
\end{exports}
\Descr{
This domain implements fraction--free Gaussian elimination on matrices.
}
#endif

FractionFreeGaussElimination(R:IntegralDomain, M:MatrixCategory R):
	LinearEliminationCategory(R, M) == add {

	rowEchelon!(a:M,b:M):(I->I,I,ARR I,I) == rowEch!(a,b,numberOfRows a);

	local rowEch!(a:M, b:M, n:I): (I->I,I,ARR I,I) == {

		import from R, Sn;

		TRACE("FFGE::rowEch!, n = ", n);

		ma := numberOfColumns a; mb := numberOfColumns b;
		assert(n=numberOfRows b);
		p:Sn := 1;
		st:ARR I := new(next n, next ma);       -- ignore st(0)

		r:I := 1;
		divisor:R := 1;
		while r<=n for c in 1..ma repeat {
			l := pivot(a, mapping p, c, r);
			TRACE("FFGE::rowEch!, pivot = ", l);
			if l<=n then {
				st(r) := c;
				p := transpose!(p, l, r);
				arc:=a(p(r),c);
				for i in next r..n repeat {
					aic := a(p(i),c);
					for j in next c..ma repeat
						a(p(i),j) := divDiffProd(arc, _
							a(p(i),j), aic, a(p(r),_
							j), divisor);
					for j in 1..mb repeat
						b(p(i),j) := divDiffProd(arc, _
							b(p(i),j), aic, b(p(r),_
							j), divisor);
				}
				r := next r;
				divisor := arc;
			}
		}
		TRACE("FFGE::rowEch!, rank = ", r - 1);
		TRACE("FFGE::rowEch!, sign p = ", sign p);
		(mapping p, prev r,st,sign p);
	}

	denominators(a:M,p:I->I,r:I,st:ARR I): ARR R == {
		den:ARR R := new(next r);	-- ignore den.0
		for i in 1..r repeat den(i) := a(p(i),st(i));
		den;
	}

	deter(a:M,p:I->I,r:I,d:I): R == {
		n := numberOfRows a;
		assert(n=numberOfColumns a);
		r<n => 0;
		d=1 => a(p(r),r);
		-a(p(r),r);
	}

	dependence(gen:Generator Vector R,n:I): (M,I->I,I,R) == {
		import from Boolean, Vector R, Sn;
		TRACE("FFGE::dependence, n = ", n);
		a:M := zero(n,n+1);
		p:Sn := 1;
		r:I := 1;
		independent := true;
		while independent for v in gen repeat {
			TRACE("FFGE::dependence, r = ", r);
			for i in 1..n repeat a(i,r) := v(i);
			divisor:R := 1;
			for j in 1..prev r repeat {
				ajj := a(p(j),j);
				for i in next(j)..n repeat {
					TRACE("FFGE::dependence, i = ", i);
					a(p(i),r) := divDiffProd(ajj,a(p(i),r),_
						a(p(i),j), a(p(j),r), divisor);
				};
				divisor := ajj;
			}
			l := pivot(a, mapping p, r, r);
			independent := l <= n;
			if independent then {
				p := transpose!(p, l, r);
				r := next r;
			}
		}
		TRACE("FFGE::dependence, r = ", r);
		(a, mapping p, r, a(p(prev r), prev r));
	}
}


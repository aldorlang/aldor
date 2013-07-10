------------------------------ sit_ff2ge.as ------------------------------------
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
\thistype{TwoStepFractionFreeGaussElimination}
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
This domain implements two-step fraction--free Gaussian elimination on matrices.
}
#endif

TwoStepFractionFreeGaussElimination(R:IntegralDomain, M:MatrixCategory R):
	LinearEliminationCategory(R,M) == add {

	rowEchelon!(a:M,b:M):(I->I,I,ARR I,I) == rowEch!(a,b,numberOfRows a);

	local rowEch!(a:M,b:M,n:I): (I->I,I,ARR I,I) == {

		TRACE("ff2ge::rowEch!:a = ", a);
		TRACE("ff2ge::rowEch!:n = ", n);
		import from R, Sn;

		ma := numberOfColumns a; mb := numberOfColumns b;
		assert(n=numberOfRows b);
		p:Sn := 1;
		st:ARR I := new(next n, next ma);       -- ignore st(0)

		c1:I := 1;
		r:I := 1;
		divisor:R := 1;

		while r<=n and c1<=ma repeat {
		    TRACE("ff2ge::rowEch!:a = ", a);
		    TRACE("ff2ge::rowEch!:r = ", r);
		    TRACE("ff2ge::rowEch!:c1 = ", c1);
		    l := pivot(a, mapping p, c1, r);
		    TRACE("ff2ge::rowEch!:l = ", l);
		    if l<=n then {
			if l~=r then p := transpose!(p, l, r);
			st(r) := c1;
			if r<n then {
			    c2 := next c1;
			    while c2<=ma repeat {
				TRACE("ff2ge::rowEch!:c2 = ", c2);
				k := pivot(a,n,mapping p,c1,c2,r);
				TRACE("ff2ge::rowEch!:k = ", k);
				if k<=n then {
				    p := transpose!(p, k, r1 := next r);
				    TRACE("ff2ge::rowEch!:", "p transposed");
				    st.r1 := c2;
				    TRACE("ff2ge::rowEch!:st.r1 = ", st.r1);
				    cc0 := divDiffProd(a(p(r),c1),a(p(r1),c2),_
						a(p(r),c2),a(p(r1),c1),divisor);
				    TRACE("ff2ge::rowEch!:cc0 = ", cc0);
				    for i in next(r1)..n repeat {
					cc1 := divDiffProd(a(p(i),c1),_
						a(p(r),c2), a(p(r),c1),_
						a(p(i),c2), divisor);
					cc2 := divDiffProd(a(p(r1),c1),_
						a(p(i),c2), a(p(i),c1),_
						a(p(r1),c2),divisor);
					for j in next(c2)..ma repeat
						a(p(i),j):=divSumProd(cc0,_
							a(p(i),j),cc1,_
							a(p(r1),j),cc2,_
							a(p(r),j), divisor);
					for j in 1..mb repeat
						b(p(i),j) := divSumProd(cc0,_
							b(p(i),j), cc1,_
							b(p(r1),j), cc2,_
							b(p(r),j), divisor);
				    }
				    for j in next(c2)..ma repeat
					a(p(r1),j) := divDiffProd(a(p(r),c1),_
							a(p(r1),j),a(p(r1),c1),_
							a(p(r),j), divisor);
				    for j in 1..mb repeat
					b(p(r1),j) := divDiffProd(a(p(r),c1),_
							b(p(r1),j),a(p(r1),c1),_
							b(p(r),j), divisor);
				    a(p(r1),c2) := cc0;
				    divisor := a(p(r1),c2);
				    r := next r1;
				    c1 := next c2;
				    break;
				}
				else c2 := next c2;
			    }
			    if c2>ma then {
				for i in next(r)..n repeat {
					for j in 1..mb repeat
						b(p(i),j) := divDiffProd(_
							a(p(r),c1), b(p(i),j),_
							a(p(i),c1), b(p(r),j),_
							divisor);
				}
				r := next r;
				c1 := next ma;
			    }
			}
			else {	-- r >= n
				r := next r;
				c1 := next c1;
			}
		    }
		    else c1 := next c1;	-- l > n
		}
		(mapping p, prev r, st, sign p);
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

	dependence(gen:Generator Vector R, n:I): (M,I->I,I,R) == {
		import from Boolean, Vector R, Sn;
		a:M := zero(n,n+1);
		p:Sn := 1;
		r:I := 1;
		independent := true;
		TRACE("ff2ge::dependence:n = ", n);
		while independent for v in gen repeat {
			TRACE("ff2ge::dependence:new v = ", v);
			for i in 1..n repeat a(i,r) := v(i);
			divisor:R := 1;
			for j in 1..prev(r) by 2 repeat {
				j1 := next j;
				c0 := a(p(j1), j1);
				for i in next(j1)..n repeat {
					a(p(i),r) := divSumProd(c0, a(p(i),r), _
							a(p(i),j), a(p(j1),r), _
							a(p(i),j1), a(p(j),r), _
							divisor);
				}
				a(p(j1),r) := divDiffProd(a(p(j),j),a(p(j1),r),_
						a(p(j1),j), a(p(j),r), divisor);
				divisor := a(p(j1),j1);
			}
			l := pivot(a, mapping p, r, r);
			independent := l <= n;
			if independent then {
				p := transpose!(p, l, r);
				w := next! gen;
				TRACE("ff2ge::dependence:w = ", w);
				r1 := next r;
				for i in 1..n repeat a(i, r1) := w(i);
				divisor := 1;
				for j in 1..prev r by 2 repeat {
					j1 := next j;
					c0 := a(p(j1),j1);
					for i in next(j1)..n repeat {
						a(p(i),r1) := divSumProd(c0,_
							a(p(i),r1), a(p(i),j),_
							a(p(j1),r1),a(p(i),j1),_
							a(p(j),r1), divisor);
					}
					a(p(j1),r1) := divDiffProd(a(p(j),j),_
							a(p(j1),r1),a(p(j1),j),_
							a(p(j),r1), divisor);
					divisor := a(p(j1),j1);
				}
				k := pivot(a, n, mapping p, r, r1, r);
				independent := k <= n;
				if independent then {
					p := transpose!(p, k, r1);
					c0:= divDiffProd(a(p(r),r),a(p(r1),r1),_
						a(p(r1),r),a(p(r),r1),divisor);
					for j in next(r1)..n repeat {
						c1 := divDiffProd(a(p(j),r),_
							a(p(r),r1), a(p(r),r),_
							a(p(j),r1), divisor);
						c2:=divDiffProd(a(p(r1),r),_
							a(p(j),r1), a(p(j),r),_
							a(p(r1),r1), divisor);
						a(p(j),r) := c1;
						a(p(j),r1) := c2;
					}
					a(p(r1),r1) := c0;
					r := next r1;
				}
				else r := r1;
			}
		}
		TRACE("ff2ge::dependence:r = ", r);
		(a, mapping p, r, a(p(prev r), prev r));
	}

	local pivot(a:M,n:I,p:I->I,c1:I,c2:I,r:I): I == {
		import from Boolean, R;
		assert(n = numberOfRows a);
		k := r;
		d:R := 0;
		while k < n and zero? d repeat {
			k := next k;
			d := a(p(r),c1)*a(p(k),c2)-a(p(k),c1)*a(p(r),c2);
		}
		zero? d => next n;
		minsize := relativeSize d;
		mini := k;
		for i in next(k)..prev(n) repeat {
			d := a(p(r),c1)*a(p(i),c2)-a(p(i),c1)*a(p(r),c2);
			if (~zero? d) and (s := relativeSize d) <minsize then {
				mini := i;
				minsize := s
			}
		}
		mini;
	}
}


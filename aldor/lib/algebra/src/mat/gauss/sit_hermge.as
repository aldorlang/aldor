---------------------------- sit_hermge.as ----------------------------------
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
\thistype{HermiteGaussElimination}
\History{Thom Mulders}{8 July 96}{created}
\Usage{import from \this(R,M)}
\Params{
{\em R} & \astype{EuclideanDomain} & A coefficient ring\\
{\em M} & \astype{MatrixCategory} R & A matrix type over R\\
}
\begin{exports}
\category{\astype{LinearEliminationCategory}(R,M)}\\
\end{exports}
\Descr{
This domain implements Hermite reduction on matrices.
}
#endif

HermiteGaussElimination(R:EuclideanDomain, M:MatrixCategory R):
	LinearEliminationCategory(R, M) == add {

	rowEchelon!(a:M,b:M):(I->I,I,ARR I,I) == rowEch!(a,b,numberOfRows a);

	local rowEch!(a:M, b:M, n:I): (I->I,I,ARR I,I) == {

		import from Boolean, R, Sn;

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
				g := a(p(r),c);
				for i in next(r)..n|~zero?(ai:=a(p i,c)) repeat{
					(g,x,y) := extendedEuclidean(g, ai);
					a1 := quotient(a(p(r),c),g);
					b1 := quotient(ai,g);
					a(p(r),c) := g;
					for j in next(c)..ma repeat {
						temp1 := a(p(r),j);
						a(p(r),j):= _
							x*a(p(r),j)+y*a(p(i),j);
						a(p(i),j) := _
							-b1*temp1+a1*a(p(i),j);
					}
					for j in 1..mb repeat {
						temp1 := b(p(r),j);
						b(p(r),j) := _
							x*b(p(r),j)+y*b(p(i),j);
						b(p(i),j) := _
							-b1*temp1+a1*b(p(i),j);
					}
				}
				q:R := 0;
				for i in 1..prev r repeat {
					(q,rr) := divide!(a(p(i),c),g,q);
					a(p(i),c) := rr;
					for j in next(c)..ma repeat
						a(p(i),j) := _
							a(p(i),j)-q*a(p(r),j);
					for j in 1..mb repeat
						b(p(i),j) := _
							b(p(i),j)-q*b(p(r),j);
				}
				r := next r;
			}
		}
		(mapping p, prev r, st, sign p);
	}

	denominators(a:M,p:I->I,r:I,st:ARR I): ARR R == {
		den:ARR R := new(next r);	-- ignore d.0
		det:R := 1;
		for i in 1..r repeat {
			det := det*a(p(i),st(i));
			den(i) := det;
		}
		den;
	}

	deter(a:M,p:I->I,r:I,d:I): R == {
		n := numberOfRows a;
		assert(n=numberOfColumns a);
		r<n => 0;
		det:R := { d = 1 => 1; -1 }
		for i in 1..r repeat det := det*a(p(i),i);
		det;
	}

	dependence(gen:Generator Vector R, n:I): (M,I->I,I,R) == {
		import from Boolean, Vector R, Sn;
		a:M := zero(n,n+1);
		t:M := one n;
		p:Sn := 1;
		r:I := 1;
		independent := true;
		while independent for v in gen repeat {
			for i in 1..n repeat {
				e:R := 0;
				for k in 1..n repeat e := e+t(i,k)*v(k);
				a(i,r) := e;
			}
			l := pivot(a, mapping p, r, r);
			independent := l <= n;
			if independent then {
				p := transpose!(p, l, r);
				g := a(p(r),r);
				for i in next(r)..n|~zero?(ai:=a(p i,r)) repeat{
					(g,x,y) := extendedEuclidean(g,ai);
					a1 := quotient(a(p(r),r),g);
					b1 := quotient(ai,g);
					a(p(r),r) := g;
					a(p(i),r) := 0@R;
					for j in 1..n repeat {
						temp1 := t(p(r),j);
						t(p(r),j) := _
							x*t(p(r),j)+y*t(p(i),j);
						t(p(i),j) := _
							-b1*temp1+a1*t(p(i),j);
					}
				}
				q:R := 0;
				for i in 1..r-1 repeat {
					(q,rr) := divide!(a(p(i),r),g,q);
					a(p(i),r) := rr;
					for j in 1..n repeat
						t(p(i),j) := _
							t(p(i),j)-q*t(p(r),j);
				}
				r:=next r;
			}
		}
		d:R := 1;
		for i in 1..prev r repeat d := d*a(p(i),i);
		(a, mapping p, r, d);
	}
}




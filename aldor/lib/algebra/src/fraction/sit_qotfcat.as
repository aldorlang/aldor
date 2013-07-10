------------------------ sit_qotfcat.as ---------------------------
-- Copyright (c) Laurent Bernardin 1994
-- Copyright (c) INRIA 1999, Version 0.1.12
-- Logiciel Algebra (c) INRIA 1999, dans sa version 0.1.12
-- Copyright (c) Swiss Federal Polytechnic Institute Zurich, 1995
-----------------------------------------------------------------------------

#include "algebra"

macro {
	I   == MachineInteger;
	B   == Boolean;
	ARR == Array;
	V   == Vector;
	POL == UnivariatePolynomialAlgebra0;
}

#if ALDOC
\thistype{FractionFieldCategory}
\History{Laurent Bernardin}{1/12/94}{created}
\Usage{\this~R: Category}
\Params{ {\em R} & \altype{IntegralDomain} & an integral domain\\ }
\Descr{\this~R is the category of the fraction fields of R.}
\begin{exports}
\category{\altype{FractionFieldCategory0} R}\\
\end{exports}
#endif

define FractionFieldCategory(R:IntegralDomain): Category ==
	FractionFieldCategory0 R with {
	default {
	macro {
		MR == DenseMatrix R;
		PR == DenseUnivariatePolynomial R;
		P0 == % pretend FractionFieldCategory0 R;
	}

	local prod(v:V R):R == {
		p:R := 1;
		for x in v repeat p := times!(p, x);
		p;
	}

	local rgen(g:Generator V %, v:V R):Generator V R == generate {
		import from I, VectorOverFraction(R, P0);
		for w in g for j in 1.. repeat {
			(d, ww) := makeIntegral w;
			v.j := d;
			yield ww;
		}
	}

	linearDependence(g:Generator V %, n:I):V % == {
		import from R, V R,VectorOverFraction(R,P0),LinearAlgebra(R,MR);
		v:V R := zero next n;
		dep := firstDependence(rgen(g, v), n);
		zero?(m := #dep) => makeRational dep;
		a := v.1 * dep.1;
		w:V % := zero m;
		w.1 := 1;
		for i in 2..m repeat w.i := (v.i * dep.i) / a;
		w;
	}

	-- returns  diag(da) s diag(db^-1) diag(d^-1)
	-- except where d has 0 entries (no sol)
	local psol(M:MatrixCategory %):(V R,V R,MR,V R) -> (M, V %) == {
		import from I,B,R,MatrixCategoryOverFraction(R,MR,P0,M);
		(da:V R, db:V R, s:MR, d:V R):(M, V %) +-> {
			sl := makeRational s;
			(n, m) := dimensions sl;
			assert(m=#d); assert(m=#db); assert(n=#da);
			dd:V % := zero m;
			for j in 1..m repeat {
				dj := db.j;
				if ~zero?(d.j) then {
					dd.j := 1;
					dj := dj * d.j;
				}
				for i in 1..n repeat
					sl(i, j) := (da.i / dj) * sl(i, j);
			}
			(sl, dd);
		}
	}

	local mult!(M:MatrixCategory %):(V R, MR) -> MR == {
		import from I, R;
		(d:V R, s:MR):MR +-> {
			(n, m) := dimensions s;
			assert(n = #d);
			for i in 1..n | ~one?(d.i) repeat for j in 1..m repeat
				s(i, j) := d.i * s(i, j);
			s;
		}
	}

	solve(M:MatrixCategory %):(M, M) -> (M, M, V %) == {
		import from LinearAlgebra(R, MR);
		import from MatrixCategoryOverFraction(R, MR, P0, M);
		psl := psol M;
		mlt := mult! M;
		(a:M, b:M):(M, M, V %) +-> {
			(da, ma) := makeColIntegral a;	-- ma = a diag(da)
			(db, mb) := makeColIntegral b;	-- mb = b diag(db)
			(k, s, d) := solve(ma, mb);
			-- we have ma s = mb diag(d)
			-- whence  a diag(da) s = b diag(db) diag(d)
			-- so the part sol is  diag(da) s diag(db^-1) diag(d^-1)
			(sol, dd) := psl(da, db, s, d);
			-- we have ma k = 0, whence a diag(da) k = 0
			-- so the homogeneous kernel is diag(da) k
			(makeRational mlt(da, k), sol, dd);
		}
	}

	particularSolution(M:MatrixCategory %):(M, M) -> (M, V %) == {
		import from LinearAlgebra(R, MR);
		import from MatrixCategoryOverFraction(R, MR, P0, M);
		psl := psol M;
		(a:M, b:M):(M, V %) +-> {
			(da, ma) := makeColIntegral a;	-- ma = a diag(da)
			(db, mb) := makeColIntegral b;	-- mb = b diag(db)
			(s, d) := particularSolution(ma, mb);
			-- we have ma s = mb diag(d)
			-- whence  a diag(da) s = b diag(db) diag(d)
			-- so the part sol is  diag(da) s diag(db^-1) diag(d^-1)
			psl(da, db, s, d);
		}
	}

	invertibleSubmatrix(M:MatrixCategory %):M->(B,ARR I,ARR I) == {
		import from LinearAlgebra(R, MR);
		import from MatrixCategoryOverFraction(R, MR, P0, M);
		(m:M):(B, ARR I, ARR I) +-> {
			(denoms, mr) := makeRowIntegral m;
			invertibleSubmatrix mr;
		}
	}

	maxInvertibleSubmatrix(M:MatrixCategory %):M->(ARR I,ARR I) == {
		import from LinearAlgebra(R, MR);
		import from MatrixCategoryOverFraction(R, MR, P0, M);
		(m:M):(ARR I, ARR I) +-> {
			(denoms, mr) := makeRowIntegral m;
			maxInvertibleSubmatrix mr;
		}
	}

	inverse(M:MatrixCategory %):M -> (M, V %) == {
		import from R, V R, LinearAlgebra(R, MR);
		import from MatrixCategoryOverFraction(R, MR, P0, M);
		psl := psol M;
		(m:M):(M, V %) +-> {
			(denoms, mr) := makeColIntegral m;-- mr = m diag(denoms)
			(m1, d) := inverse mr;
			-- we have mr m1 = diag(d)
			-- whence  m diag(denoms) m1 = diag(d)
			-- so the inverse is  diag(denoms) m1 ident diag(d^-1)
			psl(denoms, new(#denoms, 1), m1, d);
		}
	}

	factorOfDeterminant(M:MatrixCategory %):M -> (B, %) == {
		import from LinearAlgebra(R, MR);
		import from MatrixCategoryOverFraction(R, MR, P0, M);
		(m:M):(B, %) +-> {
			(denoms, mr) := makeRowIntegral m;
			(det?, d) := factorOfDeterminant mr;
			(det?, d / prod(denoms));
		}
	}

	determinant(M:MatrixCategory %):M -> % == {
		import from LinearAlgebra(R, MR);
		import from MatrixCategoryOverFraction(R, MR, P0, M);
		(m:M):% +-> {
			(denoms, mr) := makeRowIntegral m;
			determinant(mr) / prod(denoms);
		}
	}

	rankLowerBound(M:MatrixCategory %):M -> (B, I) == {
		import from LinearAlgebra(R, MR);
		import from MatrixCategoryOverFraction(R, MR, P0, M);
		(m:M):(B, I) +-> {
			(denoms, mr) := makeRowIntegral m;
			rankLowerBound mr;
		}
	}

	rank(M:MatrixCategory %):M -> I == {
		import from LinearAlgebra(R, MR);
		import from MatrixCategoryOverFraction(R, MR, P0, M);
		(m:M):I +-> {
			(denoms, mr) := makeRowIntegral m;
			rank mr;
		}
	}

	span(M:MatrixCategory %):M -> ARR I == {
		import from LinearAlgebra(R, MR);
		import from MatrixCategoryOverFraction(R, MR, P0, M);
		(m:M):ARR I +-> {
			(denoms, mr) := makeRowIntegral m;
			span mr;
		}
	}

	kernel(M:MatrixCategory %):M -> M == {
		import from LinearAlgebra(R, MR);
		import from MatrixCategoryOverFraction(R, MR, P0, M);
		(m:M):M +-> {
			(denoms, mr) := makeRowIntegral m;
			makeRational kernel mr;
		}
	}

	subKernel(M:MatrixCategory %):M -> (B, M) == {
		import from LinearAlgebra(R, MR);
		import from MatrixCategoryOverFraction(R, MR, P0, M);
		(m:M):(B, M) +-> {
			(denoms, mr) := makeRowIntegral m;
			(ker?, k) := subKernel mr;
			(ker?, makeRational k);
		}
	}

	if R has UnivariateGcdRing then {
		gcdUP(P:POL %):(P,P)->P == {
			import from R,
			     UnivariateFreeRingOverFraction(R,PR,P0,P);
			gcdP := gcdUP PR;
			(p:P, q:P):P +-> {
				zero? p => q; zero? q => p;
				(d, pp) := makeIntegral p;
				(d, qq) := makeIntegral q;
				(d, g) := normalize gcdP(pp, qq);
				g;
			}
		}

		gcdquoUP(P:POL %):(P,P)->(P,P,P) == {
			import from R,
			     UnivariateFreeRingOverFraction(R,PR,P0,P);
			gcdP := gcdquoUP PR;
			(p:P, q:P):(P,P,P) +-> {
				zero? p => (q, 0, 1);
				zero? q => (p, 1, 0);
				(dp, pp) := makeIntegral p;
				(dq, qq) := makeIntegral q;
				(gg, yy, zz) := gcdP(pp, qq);
				(a, g) := normalize gg;
				(g, (a / dp) * yy, (a / dq) * zz);
			}
		}
	}

	if R has RationalRootRing then {
		macro RR == FractionalRoot Integer;

		integerRoots(P:POL %):P -> Generator RR == {
			import from R;
			roots(P, integerRoots(PR)$R);
		}

		rationalRoots(P:POL %):P -> Generator RR == {
			import from R;
			roots(P, rationalRoots(PR)$R);
		}

		local roots(P:POL %,rootP:PR->Generator RR):P->Generator RR == {
			(p:P):Generator RR +-> {
			import from Integer, B, List RR,
			     UnivariateFreeRingOverFraction(R,PR,P0,P);
				assert(~zero? p);
				zero? degree p => generator(empty$List(RR));
				(d, pp) := makeIntegral p;
				rootP pp;
			}
		}
	}

	if R has FactorizationRing then {
		factor(P:POL %):P->(%, Product P) == {
			import from R,
			     UnivariateFreeRingOverFraction(R,PR,P0,P);
			fact := factor PR;
			(p:P):(%, Product P) +-> {
				(d, pp) := makeIntegral p;
				(lc, pr) := fact pp;
				(c, q) := normalize(P, pr);
				(c * lc / d, q);
			}
		}

		-- returns (c, q) s.t p = c q
		local normalize(P:POL %, p:Product PR):(R, Product P) == {
			import from R,
			     UnivariateFreeRingOverFraction(R,PR,P0,P);
			c:R := 1;
			q:Product P := 1;
			for term in p repeat {
				(r, e) := term;
				(a, s) := normalize r;	-- r = a s, s monic
				q := times!(q, s, e);
				c := times!(c, a^e);
			}
			(c, q);
		}
	}
	}
}		


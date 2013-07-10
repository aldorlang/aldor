------------------------ sit_qotbyct.as ---------------------------
-- Copyright (c) Manuel Bronstein 1998
-- Copyright (c) INRIA 1999, Version 0.1.12
-- Logiciel Sum^it ©INRIA 1999, dans sa version 0.1.12
-----------------------------------------------------------------------------

#include "algebra"

macro {
	B == Boolean;
	I == MachineInteger;
	Z == Integer;
	PR == DenseUnivariatePolynomial R;
	MR == DenseMatrix R;
	ARR == Array;
	V == Vector;
	P0 == % pretend FractionByCategory0 R;
	POL == UnivariatePolynomialAlgebra0;
}

#if ALDOC
\thistype{FractionByCategory}
\History{Manuel Bronstein}{9/6/98}{created}
\Usage{\this~R: Category}
\Params{ {\em R} & \astype{IntegralDomain} & an integral domain\\ }
\Descr{
\this(R) is the category of fractions of the integral domain {\em R} by
some nonzero nonunit $p \in R$,
\ie the set of all fractions whose denominator is a power of {\em p}.
}
\begin{exports}
\category{\astype{FractionByCategory0} R}\\
\end{exports}
#endif

define FractionByCategory(R:IntegralDomain): Category ==
	FractionByCategory0 R with {
	default {
	local sum(v:V Z):Z == {
		s:Z := 0;
		for x in v repeat s := add!(s, x);
		s;
	}

	local rgen(g:Generator V %, v:V Z):Generator V R == generate {
		import from I, VectorOverFraction(R, P0);
		for w in g for j in 1.. repeat {
			(mu, ww) := makeIntegralBy w;
			v.j := mu;
			yield ww;
		}
	}

	linearDependence(g:Generator V %, n:I):V % == {
		import from LinearAlgebra(R, MR);
		import from Z, V Z, R, V R, VectorOverFraction(R, P0);
		v:V Z := zero next n;
		dep := firstDependence(rgen(g, v), n);
		zero?(m := #dep) => makeRational dep;
		w:V % := zero m;
		-- TEMPORARY: MAY NEED SOME NORMALIZATION
		for i in 1..m repeat w.i := shift((dep.i)::%, v.i);
		w;
	}

	local psol(M:MatrixCategory %):(V Z,V Z,MR,V R) -> (M, V %) == {
		import from I, Z, B, R, VectorOverFraction(R, P0),
			MatrixCategoryOverFraction(R, MR, P0, M);
		(da:V Z, db:V Z, s:MR, d:V R):(M, V %) +-> {
			sl := makeRational s;
			(n, m) := dimensions sl;
			assert(m=#d); assert(n=#da); assert(n=#db);
			for j in 1..m | ~zero?(d.j) repeat {
				for i in 1..n repeat
					sl(i,j) := shift(sl(i, j), _
							da.j - db.j);
			}
			-- TEMPORARY: MAY NEED SOME NORMALIZATION
			(sl, makeRational d);
		}
	}

	solve(M:MatrixCategory %):(M, M) -> (M, M, V %) == {
		import from LinearAlgebra(R, MR);
		import from MatrixCategoryOverFraction(R, MR, P0, M);
		psl := psol M;
		(a:M, b:M):(M, M, V %) +-> {
			(mua, ma) := makeRowIntegralBy a;
			(mub, mb) := makeRowIntegralBy b;
			(k, s, d) := solve(ma, mb);
			(sol, dd) := psl(mua, mub, s, d);
			-- TEMPORARY: MAY NEED SOME NORMALIZATION
			(makeRational k, sol, dd);
		}
	}

	particularSolution(M:MatrixCategory %):(M, M) -> (M, V %) == {
		import from LinearAlgebra(R, MR);
		import from MatrixCategoryOverFraction(R, MR, P0, M);
		psl := psol M;
		(a:M, b:M):(M, V %) +-> {
			(mua, ma) := makeRowIntegralBy a;
			(mub, mb) := makeRowIntegralBy b;
			(s, d) := particularSolution(ma, mb);
			psl(mua, mub, s, d);
		}
	}

	invertibleSubmatrix(M:MatrixCategory %):M->(B,ARR I,ARR I) == {
		import from LinearAlgebra(R, MR);
		import from MatrixCategoryOverFraction(R, MR, P0, M);
		(m:M):(B, ARR I, ARR I) +-> {
			(mu, mr) := makeRowIntegralBy m;
			invertibleSubmatrix mr;
		}
	}

	maxInvertibleSubmatrix(M:MatrixCategory %):M->(ARR I,ARR I) == {
		import from LinearAlgebra(R, MR);
		import from MatrixCategoryOverFraction(R, MR, P0, M);
		(m:M):(ARR I, ARR I) +-> {
			(mu, mr) := makeRowIntegralBy m;
			maxInvertibleSubmatrix mr;
		}
	}

	inverse(M:MatrixCategory %):M -> (M, V %) == {
		import from Z, V Z, LinearAlgebra(R, MR);
		import from MatrixCategoryOverFraction(R, MR, P0, M);
		psl := psol M;
		(m:M):(M, V %) +-> {
			(mu, mr) := makeRowIntegralBy m;
			(m1, d) := inverse mr;
			psl(mu, zero(#mu), m1, d);
		}
	}

	factorOfDeterminant(M:MatrixCategory %):M -> (B, %) == {
		import from Z, LinearAlgebra(R, MR);
		import from MatrixCategoryOverFraction(R, MR, P0, M);
		(m:M):(B, %) +-> {
			(mu, mr) := makeRowIntegralBy m;
			(det?, d) := factorOfDeterminant mr;
			(det?, shift(d::%, - sum mu));
		}
	}

	determinant(M:MatrixCategory %):M -> % == {
		import from Z, LinearAlgebra(R, MR);
		import from MatrixCategoryOverFraction(R, MR, P0, M);
		(m:M):% +-> {
			(mu, mr) := makeRowIntegralBy m;
			shift(determinant(mr)::%, - sum mu);
		}
	}

	rankLowerBound(M:MatrixCategory %):M -> (B, I) == {
		import from LinearAlgebra(R, MR);
		import from MatrixCategoryOverFraction(R, MR, P0, M);
		(m:M):(B, I) +-> {
			(mu, mr) := makeRowIntegralBy m;
			rankLowerBound mr;
		}
	}

	rank(M:MatrixCategory %):M -> I == {
		import from LinearAlgebra(R, MR);
		import from MatrixCategoryOverFraction(R, MR, P0, M);
		(m:M):I +-> {
			(mu, mr) := makeRowIntegralBy m;
			rank mr;
		}
	}

	span(M:MatrixCategory %):M -> ARR I == {
		import from LinearAlgebra(R, MR);
		import from MatrixCategoryOverFraction(R, MR, P0, M);
		(m:M):ARR I +-> {
			(mu, mr) := makeRowIntegralBy m;
			span mr;
		}
	}

	kernel(M:MatrixCategory %):M -> M == {
		import from LinearAlgebra(R, MR);
		import from MatrixCategoryOverFraction(R, MR, P0, M);
		(m:M):M +-> {
			(mu, mr) := makeRowIntegralBy m;
			-- TEMPORARY: MAY NEED SOME NORMALIZATION
			makeRational kernel mr;
		}
	}

	subKernel(M:MatrixCategory %):M -> (B, M) == {
		import from LinearAlgebra(R, MR);
		import from MatrixCategoryOverFraction(R, MR, P0, M);
		(m:M):(B, M) +-> {
			(mu, mr) := makeRowIntegralBy m;
			(ker?, k) := subKernel mr;
			-- TEMPORARY: MAY NEED SOME NORMALIZATION
			(ker?, makeRational k);
		}
	}

	if R has UnivariateGcdRing then {
		gcdUP(P:POL %):(P,P)->P == {
			import from R,
			     UnivariateFreeRingOverFraction(R,PR,P0,P);
			gcdP := gcdUP PR;
			(r:P, q:P):P +-> {
				zero? r => q; zero? q => r;
				(d, pp) := makeIntegralBy r;
				(d, qq) := makeIntegralBy q;
				-- TEMPORARY: MAY NEED SOME NORMALIZATION
				makeRational gcdP(pp, qq);
			}
		}

		-- TEMPORARY: MUST DO BETTER
		gcd(a:%, b:%):% == never;
	}

	if R has RationalRootRing then {
		macro RR == FractionalRoot Z;

		integerRoots(P:POL %):P->Generator RR == {
			import from R;
			roots(P, integerRoots(PR)$R);
		}

		rationalRoots(P:POL %):P->Generator RR == {
			import from R;
			roots(P, rationalRoots(PR)$R);
		}

		local roots(P:POL %,rootP:PR->Generator RR):P->Generator RR == {
			(r:P):Generator RR +-> {
				import from Boolean, Z, List RR,
				     UnivariateFreeRingOverFraction(R,_
								PR,P0,P);
				assert(~zero? r);
				zero? degree r => generator(empty$List(RR));
				(d, pp) := makeIntegralBy r;
				rootP pp;
			}
		}
	}

	if R has FactorizationRing then {
		factor(P:POL %):P->(%, Product P) == {
			import from R;
			factP := factor PR;
			(r:P):(%, Product P) +-> {
				import from Z,
				     UnivariateFreeRingOverFraction(R,_
								PR,P0,P);
				zero? r or zero?(d := degree r) =>
					(leadingCoefficient r, 1);
				one? d => (1, term(r, 1));
				(mu, pp) := makeIntegralBy r;
				(s, prod) := factP pp;
				(shift(s::%, -mu), makeRational(P, prod));
			}
		}

		local makeRational(P:POL %,
					prod: Product PR):Product P == {
			import from R,
			     UnivariateFreeRingOverFraction(R,PR,P0,P);
			c:R := 1;
			q:Product P := 1;
			for term in prod repeat {
				(r, e) := term;
				q := times!(q, makeRational r, e);
			}
			q;
		}
	}
	}		
}

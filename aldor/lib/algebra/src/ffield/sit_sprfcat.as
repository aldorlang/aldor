--------------------------- sit_sprfcat.as ------------------------------
-- Copyright (c) Manuel Bronstein 1998
-- Copyright (c) INRIA 1999, Version 0.1.12
-- Logiciel Sum^it ©INRIA 1999, dans sa version 0.1.12
-----------------------------------------------------------------------------

#include "algebra"

#if ALDOC
\thistype{SmallPrimeFieldCategory}
\History{Manuel Bronstein}{24/7/98}{created}
\Usage{\this: Category}
\Descr{\this~is the category for prime fields of the form $\ZZ / p \ZZ$
where $p \in \ZZ$ is a machine prime.}
\begin{exports}
% \category{\astype{FFTRing}}\\
\category{\astype{PrimeFieldCategory}}\\
\category{\astype{SmallPrimeFieldCategory0}}\\
\category{\astype{UnivariateGcdRing}}\\
\end{exports}
#endif

macro {
	Z	== MachineInteger;
	AZ	== Array Z;
	V	== Vector %;
	MAT	== MatrixCategory;
	POLY	== UnivariatePolynomialAlgebra0;
	GCD	== SmallPrimeFieldCategoryGcd;
	LINALG	== SmallPrimeFieldCategoryLinearAlgebra;
}

define SmallPrimeFieldCategory: Category ==
	Join(PrimeFieldCategory, SmallPrimeFieldCategory0,
		UnivariateGcdRing, LinearAlgebraRing) with {
		-- UnivariateGcdRing, LinearAlgebraRing, FFTRing) with {
	default {
		-- fftCutoff:Z	== 0;
#if WAITFORGOODFFT
		fft!(P:POLY %):(P, P, P) -> Boolean == {
			import from DiscreteFFT %;
			fftTimes! P;
		}

		fft(P:POLY %):(P, P) -> Partial P == {
			import from DiscreteFFT %;
			fftTimes P;
		}
#endif

		gcdUP(P:POLY %):(P,P) -> P		== gcdSPF$GCD(%, P);
		gcdquoUP(P:POLY %):(P,P) -> (P,P,P)	== gcdquoSPF$GCD(%, P);

		determinant(M:MAT %):M -> %	== determinant$LINALG(%, M);
		inverse(M:MAT %):M -> (M, V)	== inverse$LINALG(%, M);
		kernel(M:MAT %):M -> M		== kernel$LINALG(%, M);
		rank(M:MAT %):M -> Z		== rank$LINALG(%, M);
		solve(M:MAT %):(M,M) -> (M,M,V)	== solve$LINALG(%, M);
		span(M:MAT %):M -> AZ		== span$LINALG(%, M);

		linearDependence(g:Generator V, n:Z):V ==
			linearDependence(g, n)$LINALG(%, DenseMatrix %);

		particularSolution(M:MAT %):(M, M) -> (M, V) ==
			particularSolution$LINALG(%, M);

		maxInvertibleSubmatrix(M:MAT %):M -> (AZ, AZ) ==
			maxInvertibleSubmatrix$LINALG(%, M);
	}
}

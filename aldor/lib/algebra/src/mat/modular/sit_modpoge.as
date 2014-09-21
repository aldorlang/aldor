---------------------------- sit_modpoge.as ------------------------------------
-- Copyright (c) Thom Mulders 1996
-- Copyright (c) INRIA 1999, Version 0.1.12
-- Logiciel Sum^it ©INRIA 1999, dans sa version 0.1.12
-- Copyright (c) Swiss Federal Polytechnic Institute Zurich, 1996
-----------------------------------------------------------------------------
#include "algebra"
#include "aldorio"

#if ALDOC
\thistype{ModulopGaussElimination}
\History{Thom Mulders}{8 July 96}{created OGE for general fields}
\History{Manuel Bronstein}{15 December 98}{adapted for machine primes}
\Usage{import from \this}
\begin{exports}
\asexp{deter}: & (M, Z, V, Z, Z, Z) $\to$ Z & Determinant\\
\asexp{determinant!}: & (M, Z, Z) $\to$ Z & Determinant\\
\asexp{extendedRowEchelon!}:
& (M, Z, Z, Z) $\to$ (V, Z, V, M) & REF of a matrix\\
\asexp{firstDependence!}:
& (\astype{Generator} V, Z, Z, M, M) $\to$ Z & First dependence relation\\
\asexp{inverse!}: & (M, Z, Z, M, V) $\to$ () & Inverse\\
\asexp{kernel!}: & (M, Z, Z, M) $\to$ Z & Kernel\\
\asexp{maxInvertibleSubmatrix!}: & (M, Z, Z) $\to$ (V, V) & Maximal minor\\
\asexp{particularSolution!}: & (M, Z, Z, M, Z, Z, M, V) $\to$ () & A solution\\
\asexp{rank!}: & (M, Z, Z, Z) $\to$ Z & Rank\\
\asexp{rowEchelon!}: & (M, Z, Z, Z) $\to$ (V, Z, V) & REF of a matrix\\
\asexp{solve!}: & (M, Z, Z, M, Z, Z, M, V, M) $\to$ Z & All solutions\\
\asexp{span!}: & (M, Z, Z) $\to$ V & Span\\
\end{exports}
\begin{aswhere}
Z &==& \astype{MachineInteger}\\
V &==& \astype{PrimitiveArray} \astype{MachineInteger}\\
M &==& \astype{PrimitiveArray} \astype{PrimitiveArray} \astype{MachineInteger}\\
\end{aswhere}
\Descr{
This domain implements ordinary Gaussian elimination on dense matrices
over the integers modulo a machine prime.
}
#endif

macro {
	Z == MachineInteger;
	A == PrimitiveArray Z;
	M == PrimitiveArray A;
	-- GAMMA is an upper bound on the ratio (remainder/compare)
	-- for machine integers (actual value depend on hardware)
	GAMMA == 50;
}

ModulopGaussElimination: with {
	deter: (M, Z, A, Z, Z, Z) -> Z;
#if ALDOC
\aspage{deter}
\Usage{\name(a,n,$\sigma$,r,d,p)}
\Signature{(\astype{PrimitiveArray} \astype{PrimitiveArray} Z,Z,
\astype{PrimitiveArray} Z,Z,Z)}{Z}
\begin{aswhere}
Z & == & \astype{MachineInteger}\\
\end{aswhere}
\Params{
{\em a} & A A \astype{MachineInteger} & A square matrix in REF form\\
{\em n} & \astype{MachineInteger} & The size of $a$\\
{\em $\sigma$} & A \astype{MachineInteger} &
A permutation of the rows of {\em a}\\
{\em r} & \astype{MachineInteger} & The number of stairs of the REF\\
{\em d} & \astype{MachineInteger} & The sign of $\sigma$\\
{\em p} & \astype{MachineInteger} & a prime\\
}
\begin{aswhere}
A & == & \astype{PrimitiveArray}\\
\end{aswhere}
\Descr{
{\em (a,$\sigma$,r,d)} must be the representation of a REF computed by
either \asexp{extendedRowEchelon!} or \asexp{rowEchelon!}. The
determinant of the original matrix over $\ZZ/p\ZZ$ is returned.
}
#endif
	determinant!: (M, Z, Z) -> Z;
#if ALDOC
\aspage{determinant!}
\Usage{\name(a,n,p)}
\Signature{(\astype{PrimitiveArray} \astype{PrimitiveArray} Z,Z,Z)}{Z}
\begin{aswhere}
Z & == & \astype{MachineInteger}\\
\end{aswhere}
\Params{
{\em a}
& \astype{PrimitiveArray} \astype{PrimitiveArray} \astype{MachineInteger} &
A square matrix\\
{\em n} & \astype{MachineInteger} & The size of $a$\\
{\em p} & \astype{MachineInteger} & a prime\\
}
\Retval{ Returns the determinant of {\em a} over $\ZZ/p\ZZ$.  }
\Remarks{Does not make a copy of $a$, but performs all the
computations in--place, modifying the entries of $a$.}
#endif
	extendedRowEchelon!: (M, Z, Z, Z) -> (A, Z, A, Z, M);
#if ALDOC
\aspage{extendedRowEchelon!}
\Usage{\name(a, r, c, p)}
\Signature{(A A Z,Z,Z)}{(A Z, Z, A Z, Z, A A Z)}
\begin{aswhere}
Z & == & \astype{MachineInteger}\\
A & == & \astype{PrimitiveArray}\\
\end{aswhere}
\Params{
{\em a}
& \astype{PrimitiveArray} \astype{PrimitiveArray} \astype{MachineInteger} &
A matrix\\
{\em r} & \astype{MachineInteger} & Number of rows of $a$\\
{\em c} & \astype{MachineInteger} & Number of columns of $a$\\
{\em p} & \astype{MachineInteger} & A prime\\
}
\Descr{
We say that a matrix $a$ is in REF if there are $r$ (the rank) and
$j_1<j_2<\cdots<j_r$ (the stairs) such that $a(i,j_i)\neq 0$,
$a(i,j)=0$ for $j<j_i$ and $a(i,j)=0$ for $i>r$.

We say that a matrix $b$ is a REF of the matrix $a$ if $b$ is in REF
and there exists a non-singular matrix $u$ such that $ua=b$.

\bigskip
\name(a,r,c,p) computes a REF of {\em a} over $\ZZ/p\ZZ$.
It returns {\em ($\sigma$,r,st,d,w)}
where $\sigma$ is a permutation,
{\em r} is the number of stairs, {\em st} are the stairs,
{\em d} is the sign of $\sigma$ and {\em w} is a matrix.
For $i > r$, {\em st(i)} is set to $m+1$ where $m$ is the number of columns
of {\em a}.
For $j\ge j_i$ the entry $(i,j)$ of the REF is stored as entry
$(p(i),j)$ in {\em a}. The other entries of {\em a} may have random
values.
The entry $(i,j)$ of the transformation matrix {\em u} is stored as
entry $(p(i),j)$ in {\em w}.
}
\Remarks{Does not make a copy of $a$, but performs all the
computations in--place, modifying the entries of $a$.}
\alseealso{\asexp{rowEchelon!}}
#endif
	firstDependence!: (Generator A, Z, Z, M, M) -> Z;
#if ALDOC
\aspage{firstDependence!}
\Usage{\name(gen, n, p, a, d)}
\Signature{(\astype{Generator} A Z, Z, Z, A A Z, A A Z)}{Z}
\begin{aswhere}
A & == & \astype{PrimitiveArray}\\
Z & == & \astype{MachineInteger}\\
\end{aswhere}
\Params{
{\em gen}
& \astype{Generator} A \astype{MachineInteger} & A generator of vectors\\
{\em n} & \astype{MachineInteger} & The dimension of the vectors generated\\
{\em p} & \astype{MachineInteger} & a prime\\
{\em a} & A A \astype{MachineInteger} & A work matrix\\
{\em d} & A A \astype{MachineInteger} & A matrix for the dependence\\
}
\begin{aswhere}
A & == & \astype{PrimitiveArray}\\
\end{aswhere}
\Descr{\name(gen,n,p,a,d) computes the first dependence over $\ZZ/p\ZZ$ among
the vectors generated by {\em gen}.
It returns the number $r$ of vectors generated. This number is as small
as possible, meaning that the first $r-1$ vectors are linearly independent
over $\ZZ/p\ZZ$. The coefficients of the dependence are stored in the
first column of $d$, which must have at least $r$ rows, upon return.
The work matrix $a$ must have $n$ rows
and at least $r$ columns (note that $r \le n+1$).
The dimension of the vectors generated by {\em gen} must be
{\em n}. There must be a relation between the vectors generated.}
#endif
	inverse!: (M, Z, Z, M, A) -> ();
#if ALDOC
\aspage{inverse!}
\Usage{\name(a,n,p,b,d)}
\Signature{(A A Z, Z, Z, A A Z, A Z)}{Z}
\begin{aswhere}
Z & == & \astype{MachineInteger}\\
A & == & \astype{PrimitiveArray}\\
\end{aswhere}
\Params{
{\em a, b}
& \astype{PrimitiveArray} \astype{PrimitiveArray} \astype{MachineInteger} &
Square matrices\\
{\em n} & \astype{MachineInteger} & The size of $a$, $b$ and $d$\\
{\em p} & \astype{MachineInteger} & a prime\\
{\em d} & \astype{PrimitiveArray} \astype{MachineInteger} & a vector\\
}
\Retval{ Fills $b$ and $d$ such that $d_i \in \{0,1\}$ for each $i$ and
$$
a b = \pmatrix{
d_0 &     &        & \cr
    & d_1 &        & \cr
    &     & \ddots & \cr
    &     &        & d_{n-1} \cr
}\,.
$$
}
\Remarks{$\prod_{i=0}^{n-1} d_i = 1$ if and only if $a$ is invertible,
in which case $b = a^{-1}$.
Does not make a copy of $a$, but performs all the
computations in--place, modifying the entries of $a$.}
#endif
	kernel!: (M, Z, Z, Z, M) -> Z;
#if ALDOC
\aspage{kernel}
\Usage{\name(a,r,c,p,w)}
\Signature{(A A Z,Z,Z,A A Z)}{Z}
\begin{aswhere}
Z & == & \astype{MachineInteger}\\
A & == & \astype{PrimitiveArray}\\
\end{aswhere}
\Params{
{\em a, w}
& \astype{PrimitiveArray} \astype{PrimitiveArray} \astype{MachineInteger} &
Matrices\\
{\em r} & \astype{MachineInteger} & Number of rows of $a$\\
{\em c} & \astype{MachineInteger} & Number of columns of $a$\\
{\em p} & \astype{MachineInteger} & a prime\\
}
\Retval{Stores a basis of the kernel of {\em a} in the columns of {\em w},
which must be large enough, and returns the dimension of the kernel.}
\Remarks{Does not make a copy of $a$, but performs all the
computations in--place, modifying the entries of $a$.}
#endif
	maxInvertibleSubmatrix!: (M, Z, Z, Z) -> (Array Z, Array Z);
#if ALDOC
\aspage{maxInvertibleSubmatrix!}
\Usage{\name(a,r,c,p)}
\Signature{(A A Z,Z,Z)}{(\astype{Array} Z, \astype{Array} Z)}
\begin{aswhere}
Z & == & \astype{MachineInteger}\\
A & == & \astype{PrimitiveArray}\\
\end{aswhere}
\Params{
{\em a}
& \astype{PrimitiveArray} \astype{PrimitiveArray} \astype{MachineInteger} &
A matrix\\
{\em r} & \astype{MachineInteger} & Number of rows of $a$\\
{\em c} & \astype{MachineInteger} & Number of columns of $a$\\
{\em p} & \astype{MachineInteger} & a prime\\
}
\Retval{ Returns $([r_1,\dots,r_r], [c_1,\dots,c_r])$
where $r$ is the rank of {\em a} over $\ZZ / p\ZZ$
and the submatrix of $a$ formed by the intersections of the rows $r_i$
and $c_i$ is invertible over $\ZZ / p\ZZ$.}
\Remarks{Does not make a copy of $a$, but performs all the
computations in--place, modifying the entries of $a$.}
#endif
	particularSolution!: (M, Z, Z, M, Z, Z, M, A) -> ();
#if ALDOC
\aspage{particularSolution!}
\Usage{\name(a,ra,ca,b,cb,p,w,d)}
\Signature{(A A Z,Z,Z,A A Z,Z,Z,A A Z,A Z)}{()}
\begin{aswhere}
Z & == & \astype{MachineInteger}\\
A & == & \astype{PrimitiveArray}\\
\end{aswhere}
\Params{
{\em a,b,w}
& \astype{PrimitiveArray} \astype{PrimitiveArray} \astype{MachineInteger} &
Matrices\\
{\em ra} & \astype{MachineInteger} & Number of rows of $a$\\
{\em ca} & \astype{MachineInteger} & Number of columns of $a$\\
{\em cb} & \astype{MachineInteger} & Number of columns of $b$\\
{\em p} & \astype{MachineInteger} & a prime\\
{\em d} & \astype{PrimitiveArray} \astype{MachineInteger} & a vector\\
}
\Retval{ Fills $w$ and $d$ such that $d_i \in \{0,1\}$ for each $i$ and
$$
a w = b \pmatrix{
d_0 &     &        & \cr
    & d_1 &        & \cr
    &     & \ddots & \cr
    &     &        & d_{n-1} \cr
}\,.
$$
\Remarks{For each $i$, $d_i = 1$ if and only if the system
$a x = \sth{(i+1)}$ column of $b$ has a solution, which is then
the $\sth{(i+1)}$ column of $w$, which must have the same dimensions
than $b$, which must have the same number of rows that $a$.
Does not make a copy of $a$, but performs all the
computations in--place, modifying the entries of $a$,
while $b$ is not modified.}
}
#endif
	rank!: (M, Z, Z, Z) -> Z;
#if ALDOC
\aspage{rank!}
\Usage{\name(a,r,c,p)}
\Signature{(\astype{PrimitiveArray} \astype{PrimitiveArray} Z,Z,Z)}{Z}
\begin{aswhere}
Z & == & \astype{MachineInteger}\\
\end{aswhere}
\Params{
{\em a}
& \astype{PrimitiveArray} \astype{PrimitiveArray} \astype{MachineInteger} &
A matrix\\
{\em r} & \astype{MachineInteger} & Number of rows of $a$\\
{\em c} & \astype{MachineInteger} & Number of columns of $a$\\
{\em p} & \astype{MachineInteger} & a prime\\
}
\Retval{ Returns the rank of {\em a} over $\ZZ/p\ZZ$.  }
\Remarks{Does not make a copy of $a$, but performs all the
computations in--place, modifying the entries of $a$.}
#endif
	rowEchelon!: (M, Z, Z, Z) -> (A, Z, A, Z);
#if ALDOC
\aspage{rowEchelon!}
\Usage{\name(a, r, c, p)}
\Signature{(A A Z,Z,Z)}{(A Z, Z, A Z, Z)}
\begin{aswhere}
Z & == & \astype{MachineInteger}\\
A & == & \astype{PrimitiveArray}\\
\end{aswhere}
\Params{
{\em a}
& \astype{PrimitiveArray} \astype{PrimitiveArray} \astype{MachineInteger} &
A matrix\\
{\em r} & \astype{MachineInteger} & Number of rows of $a$\\
{\em c} & \astype{MachineInteger} & Number of columns of $a$\\
{\em p} & \astype{MachineInteger} & A prime\\
}
\Descr{
We say that a matrix $a$ is in REF if there are $r$ (the rank) and
$j_1<j_2<\cdots<j_r$ (the stairs) such that $a(i,j_i)\neq 0$,
$a(i,j)=0$ for $j<j_i$ and $a(i,j)=0$ for $i>r$.

We say that a matrix $b$ is a REF of the matrix $a$ if $b$ is in REF
and there exists a non-singular matrix $u$ such that $ua=b$.

\bigskip
\name(a,r,c,p) computes a REF of {\em a} over $\ZZ/p\ZZ$.
It returns {\em ($\sigma$,r,st,d)}
where $\sigma$ is a permutation, {\em r} is the
number of stairs, {\em st} are the stairs and {\em d} is the sign of {\em p}.
For $i > r$, {\em st(i)} is set to $m+1$ where $m$ is the number of columns
of {\em a}.
For $j\ge j_i$ the entry $(i,j)$ of the REF is stored as entry
$(p(i),j)$ in {\em a}. The other entries of {\em a} may have random values.
}
\Remarks{Does not make a copy of $a$, but performs all the
computations in--place, modifying the entries of $a$.}
\alseealso{\asexp{extendedRowEchelon!}}
#endif
	solve!: (M, Z, Z, M, Z, Z, M, A, M) -> Z;
#if ALDOC
\aspage{solve!}
\Usage{\name(a,ra,ca,b,cb,p,w,d,k)}
\Signature{(A A Z,Z,Z,A A Z,Z,Z,A A Z,A Z,A A Z)}{Z}
\begin{aswhere}
Z & == & \astype{MachineInteger}\\
A & == & \astype{PrimitiveArray}\\
\end{aswhere}
\Params{
{\em a,b,w,k}
& \astype{PrimitiveArray} \astype{PrimitiveArray} \astype{MachineInteger} &
Matrices\\
{\em ra} & \astype{MachineInteger} & Number of rows of $a$\\
{\em ca} & \astype{MachineInteger} & Number of columns of $a$\\
{\em cb} & \astype{MachineInteger} & Number of columns of $b$\\
{\em p} & \astype{MachineInteger} & a prime\\
{\em d} & \astype{PrimitiveArray} \astype{MachineInteger} & a vector\\
}
\Retval{ Fills $w$ and $d$ such that $d_i \in \{0,1\}$ for each $i$ and
$$
a w = b \pmatrix{
d_0 &     &        & \cr
    & d_1 &        & \cr
    &     & \ddots & \cr
    &     &        & d_{n-1} \cr
}\,.
$$
Furthermore, \name~stores a basis of the kernel of {\em a} in the columns of
{\em k},
which must be large enough, and returns the dimension of the kernel.}
\Remarks{For each $i$, $d_i = 1$ if and only if the system
$a x = \sth{(i+1)}$ column of $b$ has a solution, which is then
the $\sth{(i+1)}$ column of $w$, which must have the same dimensions
than $b$, which must have the same number of rows that $a$.
Does not make a copy of $a$, but performs all the
computations in--place, modifying the entries of $a$,
while $b$ is not modified.}
#endif
	span!: (M, Z, Z, Z) -> Array Z;
#if ALDOC
\aspage{span!}
\Usage{\name(a,r,c,p)}
\Signature{(\astype{PrimitiveArray} \astype{PrimitiveArray} Z,Z,Z)}
{\astype{Array} Z}
\begin{aswhere}
Z & == & \astype{MachineInteger}\\
\end{aswhere}
\Params{
{\em a}
& \astype{PrimitiveArray} \astype{PrimitiveArray} \astype{MachineInteger} &
A matrix\\
{\em r} & \astype{MachineInteger} & Number of rows of $a$\\
{\em c} & \astype{MachineInteger} & Number of columns of $a$\\
{\em p} & \astype{MachineInteger} & a prime\\
}
\Retval{ Returns $[c_1,\dots,c_r]$ where $r$ is the rank of {\em a}
over $\ZZ/p\ZZ$
and the span of $a$ is generated by its columns $c_1,\dots,c_r$.}
\Remarks{Does not make a copy of $a$, but performs all the
computations in--place, modifying the entries of $a$.}
#endif
} == add {
	local maxhalf:Z == maxPrime$HalfWordSizePrimes;
	local maxint:Z	== max$Z;

#if TRACE
	local prt(str:String, a:A, n:Z):() == {
		import from TextWriter, WriterManipulator;
		stderr << str;
		for i in 0..prev n repeat stderr << a.i << " ";
		stderr << endnl;
	}

	local prt(str:String, a:M, r:Z, c:Z):() == {
		import from TextWriter, WriterManipulator;
		stderr << str << " = ";
		for i in 0..prev r repeat prt("", a.i, c);
		stderr << endnl;
	}
#else
	local prt(str:String, a:A, n:Z):()	== {};
	local prt(str:String, a:M, r:Z, c:Z):() == {};
#endif

	rank!(a:M, ra:Z, ca:Z, p:Z):Z == {
		(pp,r,st,d) := rowEchelon!(a, ra, ca, p);
		r;
	}

	span!(a:M, ra:Z, ca:Z, p:Z):Array Z == {
		(pp,r,st,d) := rowEchelon!(a, ra, ca, p);
		array(st, r);
	}

	maxInvertibleSubmatrix!(a:M, ra:Z, ca:Z, p:Z):(Array Z, Array Z) == {
		import from A;
		(pp,r,st,d) := rowEchelon!(a, ra, ca, p);
		([next pp i for i in 0..prev r], array(st, r));
	}

	determinant!(a:M, n:Z, p:Z):Z == {
		(pp, r, st, d) := rowEchelon!(a, n, n, p);
		deter(a, n, pp, r, d, p);
	}

	rowEchelon!(a:M, ra:Z, ca:Z, p:Z):(A, Z, A, Z) == {
		import from String;
		prt("modpge::rowEchelon!: a", a, ra, ca);
		TRACE("modpge::rowEchelon!: p = ", p);
		rowEch!(a, ra, ca, empty, ra, 0, p);
	}

	extendedRowEchelon!(a:M, ra:Z, ca:Z, p:Z):(A, Z, A, Z, M) == {
		b:M := new ra;
		for i in 1..ra repeat {
			b.i := new(ra, 0);
			b.i.i := 1;
		}
		(pp, r, st, d) := rowEch!(a, ra, ca, b, ra, ra, p);
		(pp, r, st, d, b);
	}

	firstDependence!(gen:Generator A, n:Z, p:Z, work:M, sol:M):Z == {
		(sigma, r) := dependence(gen, n, p, work);
		assert(r <= next n);
		st:A := new(r1 := prev r);
		for i in 0..prev r1 repeat st.i := i;
		backSolve!(work, n, r, sol, 0, sigma, st, r1, r, p);
		r;
	}

	local dependence(gen:Generator A, n:Z, p:Z, work:M):(A, Z) == {
		assert(n > 0); assert(p > 1);
		p > maxhalf => fullDep(gen, n, p, work);
		halfDep(gen, n, p, work);
	}

	deter(a:M, n:Z, pp:A, r:Z, d:Z, p:Z):Z == {
		import from String;
		prt("modpge::deter:a", a, n, n);
		prt("modpge::deter:pp", pp, n);
		TRACE("modpge::deter:n = ", n);
		TRACE("modpge::deter:r = ", r);
		TRACE("modpge::deter:d = ", d);
		TRACE("modpge::deter: p = ", p);
		assert(p > 1);
		p > maxhalf => fullDeter(a, n, pp, r, d, p);
		halfDeter(a, n, pp, r, d, p);
	}

	-- stores the solutions in the matrix sol
	-- and returns the dimension of the kernel
	kernel!(a:M, n:Z, m:Z, p:Z, sol:M):Z == {
		import from A;
		(sigma, r, st, d) := rowEchelon!(a, n, m, p);
		nullSpace!(a, n, m, sol, sigma, r, st, p);
	}

	-- stores the solutions in the matrix sol
	-- and returns the dimension of the kernel
	local nullSpace!(a:M, n:Z, m:Z, sol:M, sigma:A, r:Z, st:A, p:Z):Z == {
		import from String;
		prt("modpge::nullSpace:a", a, n, m);
		prt("modpge::nullSpace:sigma = ", sigma, n);
		prt("modpge::nullSpace:st = ", st, r);
		TRACE("modpge::nullSpace:r = ", r);
		zero? r => {	-- kernel is an mxm identity matrix
			for i in 0..prev m repeat {
				for j in 0..prev m repeat sol.i.j := 0;
				sol.i.i := 1;
			}
			m;
		}
		k:Z := 0;
		for j in 1..st.0 repeat {
			backSolve!(a, n, m, sol, k, sigma, st, 0, j, p);
			k := next k;
		}
		for i in 1..prev r repeat {
			for j in next next(st(prev i))..st.i repeat {
				backSolve!(a, n, m, sol, k, sigma, st, i, j, p);
				k := next k;
			}
		}
		if r > 0 then for j in next next(st prev r)..m repeat {
				backSolve!(a, n, m, sol, k, sigma, st, r, j, p);
				k := next k;
		}
		k;
	}

	-- stores the solution in the matrix sol and the denoms in den
	inverse!(a:M, n:Z, p:Z, sol:M, den:A):() == {
		(sigma, r, st, d, b) := extendedRowEchelon!(a, n, n, p);
		backSolve!(a, n, n, sol, den, sigma, st, r, b, n, p);
	}

	-- stores a particular solution in the matrix sol, its denoms in den,
	-- a basis of the kernel in ker, and returns the dimension of the kernel
	-- the number of rows of b must be ra
	solve!(a:M, na:Z, ma:Z, b:M, mb:Z, p:Z, sol:M, den:A, ker:M):Z == {
		(sigma, r, st, d) := rowEch!(a, na, ma, b, na, mb, p);
		backSolve!(a, na, ma, sol, den, sigma, st, r, b, mb, p);
		nullSpace!(a, na, ma, ker, sigma, r, st, p);
	}

	-- stores the solution in the matrix sol and the denoms in den
	-- the number of rows of b must be ra
	particularSolution!(a:M, na:Z, ma:Z, b:M, mb:Z, p:Z, sol:M, den:A):()=={
		(sigma, r, st, d) := rowEch!(a, na, mb, b, na, mb, p);
		backSolve!(a, na, ma, sol, den, sigma, st, r, b, mb, p);
	}

	-- stores the solution in column k of the matrix sol
	-- c = column number is 1-indexed (first column is c = 1)
	local backSolve!(a:M,ra:Z,m:Z,sol:M,k:Z,sigma:A,st:A,r:Z,c:Z,p:Z):()=={
		p > maxhalf => fullBS!(a, ra, m, sol, k, sigma, st, r, c, p);
		halfBS!(a, ra, m, sol, k, sigma, st, r, c, p);
	}

	-- stores the solution in the matrix sol and the denoms in den
	-- the number of rows of b must be ra
	local backSolve!(a:M, ra:Z, ca:Z, sol:M, den:A,
				sigma:A, st:A, r:Z, b:M, cb:Z, p:Z):() == {
		p > maxhalf => fullBS!(a, ra, ca, sol, den, sigma, st,r,b,cb,p);
		halfBS!(a, ra, ca, sol, den, sigma, st, r, b, cb, p);
	}

	local rowEch!(a:M, ra:Z, ca:Z, b:M, rb:Z, cb:Z, p:Z):(A,Z,A,Z) == {
		import from String;
		prt("modpge::rowEch!: a", a, ra, ca);
		TRACE("modpge::rowEch!: p = ", p);
		assert(ra = rb); assert(p > 1);
		p > maxhalf => fullRowEch!(a, ra, ca, b, rb, cb, p);
		-- from normalized inputs, the first k loops cannot overflow
		k := prev(maxint quo (prev(p)*prev(p)));
		k > GAMMA or 4*k > ra => halfRowEch!(a, ra, ca, b, rb, cb, k,p);
		halfRowEch!(a, ra, ca, b, rb, cb, p);
	}

	local identity(n:Z):A == {
		assert(n > 0);
		a:A := new n;
		for i in 0..prev n repeat a.i := i;
		a;
	}

	local transpose!(p:A, i:Z, j:Z, d:Z):Z == {
		assert(i >= 0); assert(j >= 0);
		i = j => d;
		t := p.i;
		p.i := p.j;
		p.j := t;
		-d;
	}

-- Those 2 files contain similar code with 2 different product functions
-- (the product is not passed as parameter because the function-call
--  overhead is too high, and this would prevent inlining)
#include "sit_fullge.as"
#include "sit_halfge.as"
}

#if ALDORTEST

#include "algebra"
#include "aldorio"

macro {
	I == MachineInteger;
	Z == Integer;
	F == SmallPrimeField 3;
	M == DenseMatrix;
}

import from Symbol, I;

local bug():() == {
        import from Assert MachineInteger;
        import from Assert F;
	import from F, LinearAlgebra(F, M F);
	m:M F := zero(8,3);
	m(1,2) := 1;
	m(2,3) := 1;
	K := kernel m;
	assertEquals(3, numberOfRows K);
	assertEquals(1, numberOfColumns K);
	assertEquals(1, K(1,1));
	assertEquals(0, K(2,1));
	assertEquals(0, K(3,1));
}

bug();

local test(r: Z, c: Z, p: I): () == {
        import from Assert MachineInteger;
	Fp ==> SmallPrimeField p;
        import from Assert Fp;
	import from Vector Fp;
	import from Fp, LinearAlgebra(Fp, M Fp);
	for j in 1..10@Z repeat {
		m: M Fp := random(5,5);
		K := kernel m;
                (nrk, nck) := dimensions K;
	        for i in 1..10@Z repeat {
			v: M Fp := random(nck, 1);
			assertTrue(zero?(m * (K * v)));
	        }
       }
}

tt(): () == {
      import from AldorInteger;
      test(5,5,3::I);
      test(10,10,3::I);
      test(7,8,3::I);

      test(5,5,19::I);
      test(10,10,19::I);
      test(7,8,19::I)
}
tt();

#endif

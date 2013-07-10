--------------------------- sit_linelim.as ----------------------------------
-- Copyright (c) Thom Mulders 1996
-- Copyright (c) INRIA 1999, Version 0.1.12
-- Logiciel Sum^it ©INRIA 1999, dans sa version 0.1.12
-- Copyright (c) Swiss Federal Polytechnic Institute Zurich, 1996
-----------------------------------------------------------------------------

#include "algebra"

macro {
	I == MachineInteger;
	ARR == PrimitiveArray;
}

#if ALDOC
\thistype{LinearEliminationCategory}
\History{Thom Mulders}{8 July 96}{created}
\Usage{\this(R,M): Category}
\Params{
{\em R} & \altype{CommutativeRing} & A coefficient ring\\
{\em M} & \altype{MatrixCategory} R& A matrix type over R\\
}
\Descr{\this~is a common category for linear elimination computations.
The category provides operations for computating a row echelon form
(REF) of a matrix and the first dependence relation among vectors.}
\begin{exports}
\alexp{extendedRowEchelon}:
& M $\to$ (M,Z $\to$ Z,Z,ARR Z,Z,M) & REF of a matrix\\
\alexp{extendedRowEchelon!}:
& M $\to$ (Z $\to$ Z,Z,ARR Z,Z,M) & REF of a matrix\\
\alexp{extendedRowEchelonForm}: & M $\to$ (M,M) & REF of a matrix\\
\alexp{maxInvertibleSubmatrix}:
& M $\to$ (\altype{Array} Z, \altype{Array} Z) & Maximal minor\\
\alexp{maxInvertibleSubmatrix!}:
& M $\to$ (\altype{Array} Z, \altype{Array} Z) & Maximal minor\\
\alexp{pivot}: & (M, Z $\to$ Z, Z, Z) $\to$ Z & Select a pivot\\
\alexp{rowEchelon}: & M $\to$ (M,Z $\to$ Z,Z,ARR Z,Z) & REF of a matrix\\
\alexp{rowEchelon!}: & M $\to$ (Z $\to$ Z,Z,ARR Z,Z) & REF of a matrix\\
                     & (M, M) $\to$ (Z $\to$ Z,Z,ARR Z,Z) & \\
\alexp{rowEchelonForm}: & M $\to$ M & REF of a matrix\\
\alexp{span}: & M $\to$ \altype{Array} Z & Span of a matrix\\
\alexp{span!}: & M $\to$ \altype{Array} Z & Span of a matrix\\
\end{exports}
\begin{exports}[if \emph{R} has \altype{IntegralDomain} then]
\alexp{denominators}:
& (M,Z $\to$ Z,Z,ARR Z) $\to$ ARR R & Maximal denominators\\ 
\alexp{dependence}:
& (Generator V,Z) $\to$ (M,Z $\to$ Z,Z,R) & First linear dependence\\
\alexp{deter}: & (M,Z $\to$ Z,Z,Z) $\to$ R & Determinant\\
\alexp{determinant}: & M $\to$ R & Determinant\\
\alexp{determinant!}: & M $\to$ R & Determinant\\
\alexp{rank!}: & M $\to$ Z & Rank of a matrix\\
\alexp{rank}: & M $\to$ Z & Rank of a matrix\\
\end{exports}
\begin{alwhere}
Z & == & \altype{MachineInteger}\\
ARR & == & \altype{PrimitiveArray}\\
V & == & \altype{Vector} R\\
\end{alwhere}
#endif

define LinearEliminationCategory(R:CommutativeRing, M:MatrixCategory R):
	Category == with {
	if R has IntegralDomain then {
		denominators: (M,I->I,I,ARR I) -> ARR R;
#if ALDOC
\alpage{denominators}
\Usage{\name(a,p,r,st)}
\Signature{(M,Z $\to$ Z,Z, \altype{PrimitiveArray} Z)}
{\altype{PrimitiveArray} R}
\begin{aswhere}
Z & == & \altype{MachineInteger}\\
\end{aswhere}
\Params{
{\em a} & M & A matrix in REF form\\
{\em p} & \altype{MachineInteger} $\to$ \altype{MachineInteger} &
A permutation of the rows of {\em a}\\
{\em r} & \altype{MachineInteger} & The number of stairs of the REF\\
{\em st} & \altype{PrimitiveArray} \altype{MachineInteger} &
The stairs of the REF\\
}
\Descr{
{\em (a,p,r,st)} must be the representation of a REF computed by
\alexp{rowEchelon}, \alexp{extendedRowEchelon} or their bang-versions. When
{\em d} is returned then for $st(i)<j<st(i+1)$ we have that $d(i)$ times the
$j$-th column of the REF is a linear combination of the first $i$
leading columns of the REF. (The $i$-th column is called leading if
$i$ is a stair) 
}
#endif
		dependence: (Generator Vector R,I) -> (M,I -> I,I,R);
#if ALDOC
\alpage{dependence}
\Usage{\name(gen,n)}
\Signature{(\altype{Generator} \altype{Vector} R,Z)}{(M,Z $\to$ Z,Z,R)}
\begin{aswhere}
Z & == & \altype{MachineInteger}\\
\end{aswhere}
\Params{
{\em gen} & \altype{Generator} \altype{Vector} R & A generator of vectors\\
{\em n} & \altype{MachineInteger} & The dimension of the vectors generated\\
}
\Descr{
\name(gen,n) computes the first dependence among the vectors
generated. First means that \name(gen,n) stops as soon as a dependence
relation exists among the vectors generated so far. \name(gen,n)
returns a matrix {\em a}, a permutation {\em p}, the length {\em r} of a
relation and the maximal denominator {\em d} needed for a dependence
relation. After applying {\em p} to the rows of {\em a} one gets a
matrix whose first $r-1$ columns form an upper-triangular
matrix. $d$ times the last column of $a$ is a linear combination of
the first $r-1$ columns of $a$. A dependence relation between the
columns of $a$ is also a dependence relation between the vectors
generated. 
}
#endif
		deter: (M,I->I,I,I) -> R;
#if ALDOC
\alpage{deter}
\Usage{\name(a,p,r,d)}
\Signature{(M,Z $\to$ Z,Z,Z,Z)}{R}
\begin{aswhere}
Z & == & \altype{MachineInteger}\\
\end{aswhere}
\Params{
{\em a} & M & A matrix in REF form\\
{\em p} & \altype{MachineInteger} $\to$ \altype{MachineInteger} &
A permutation of the rows of {\em a}\\
{\em r} & \altype{MachineInteger} & The number of stairs of the REF\\
{\em d} & \altype{MachineInteger} & The sign of p\\
}
\Descr{
{\em (a,p,r,d)} must be the representation of a REF computed by
\alexp{rowEchelon}, \alexp{extendedRowEchelon} or their bang-versions. The
determinant of the original matrix is returned. 
}
#endif
		determinant: M -> R;
		determinant!: M -> R;
#if ALDOC
\alpage{determinant}
\altarget{\name!}
\Usage{\name~a\\ \name!~a}
\Signature{M}{R}
\Params{ {\em a} & M & A matrix whose determinant has to be computed\\ }
\Retval{ Returns the determinant of {\em a}.  }
\Remarks{\name!~does not make a copy of $a$, but performs all the
computations in--place, modifying the entries of $a$.}
#endif
	}
	extendedRowEchelon: M -> (M,I->I,I,ARR I,I,M);
	extendedRowEchelon!: M -> (I->I,I,ARR I,I,M);
#if ALDOC
\alpage{extendedRowEchelon}
\altarget{\name!}
\Usage{\name~a\\ \name!~a}
\Signatures{
\name: & M $\to$ (M,Z $\to$ Z,Z,\altype{PrimitiveArray} Z,Z,M)\\
\name!: & M $\to$ (Z $\to$ Z,Z,\altype{PrimitiveArray} Z,Z,M)\\
}
\begin{aswhere}
Z & == & \altype{MachineInteger}\\
\end{aswhere}
\Params{
{\em a} & M & A matrix whose REF and corresponding transformation matrix\\
& &  have to be computed\\
}
\Descr{
We say that a matrix $a$ is in REF if there are $r$ (the rank) and
$j_1<j_2<\cdots<j_r$ (the stairs) such that $a(i,j_i)\neq 0$,
$a(i,j)=0$ for $j<j_i$ and $a(i,j)=0$ for $i>r$.

We say that a matrix $b$ is a REF of the matrix $a$ if $b$ is in REF
and there exists a non-singular matrix $u$ such that $ua=b$.

\bigskip
\name~a computes a REF of {\em a} in {\em a}. It returns {\em (c,p,r,st,d,w)}
where {\em c} is a matrix, {\em p} is a permutation, {\em r} is the
number of stairs, {\em st} are the stairs,
{\em d} is the sign of {\em p} and {\em w} is a matrix.
For $i > r$, {\em st(i)} is set to $m+1$ where $m$ is the number of columns
of {\em a}.
For $j\ge j_i$ the entry $(i,j)$ of the REF is stored as entry
$(p(i),j)$ in {\em c}. The other entries of {\em c} may have random
values.
The entry $(i,j)$ of the transformation matrix {\em u} is stored as
entry $(p(i),j)$ in {\em w}.
}
\Remarks{\name!~does not make a copy of $a$, but performs all the
computations in--place, storing the final result in $a$.}
\alseealso{\alexp{extendedRowEchelonForm}}
#endif
	extendedRowEchelonForm: M -> (M,M);
#if ALDOC
\alpage{extendedRowEchelonForm}
\Usage{\name~a}
\Signature{M}{(M,M)}
\Params{ {\em a} & M & A matrix whose REF has to be computed}
\Descr{
We say that a matrix $a$ is in REF if there are $r$ (the rank) and
$j_1<j_2<\cdots<j_r$ (the stairs) such that $a(i,j_i)\neq 0$,
$a(i,j)=0$ for $j<j_i$ and $a(i,j)=0$ for $i>r$.

We say that a matrix $b$ is a REF of the matrix $a$ if $b$ is in REF
and there exists a non-singular matrix $u$ such that $ua=b$.
}
\Retval{Returns a REF {\em b} of {\em a} and the transformation matrix
{\em u} such that $ua=b$.}
\alseealso{\alexp{extendedRowEchelon}}
#endif
	maxInvertibleSubmatrix: M -> (Array I, Array I);
	maxInvertibleSubmatrix!: M -> (Array I, Array I);
#if ALDOC
\alpage{maxInvertibleSubmatrix}
\altarget{\name!}
\Usage{\name~a\\ \name!~a}
\Signature{M}{(\altype{Array} \altype{MachineInteger},
\altype{Array} \altype{MachineInteger})}
\Params{ {\em a} & M & A matrix\\ }
\Retval{ Returns $([r_1,\dots,r_r], [c_1,\dots,c_r])$
where $r$ is the rank of {\em a}
and the submatrix of $a$ formed by the intersections of the rows $r_i$
and $c_i$ is invertible.}
\Remarks{\name!~does not make a copy of $a$, but performs all the
computations in--place, modifying the entries of $a$.}
\alseealso{\alexp{rank},\alexp{span}}
#endif
	pivot: (M, I -> I, I, I) -> I;
#if ALDOC
\alpage{pivot}
\Usage{\name(a, p, c, r)}
\Signature{(M,Z $\to$ Z, Z, Z)}{Z}
\begin{aswhere}
Z & == & \altype{MachineInteger}\\
\end{aswhere}
\Params{
{\em a} & M & A matrix\\
{\em p} &\altype{MachineInteger} $\to$ \altype{MachineInteger} & A permutation\\
{\em c} & \altype{MachineInteger} & A column index\\
{\em r} & \altype{MachineInteger} & A row index\\
}
\Retval{Returns the row index of the an appropriate pivot for column $c$
at row $r$ or below. The matrix considered is $a$ with its rows permuted
by $p$.}
#endif
	if R has IntegralDomain then {
		rank: M -> I;
		rank!: M -> I;
#if ALDOC
\alpage{rank}
\altarget{\name!}
\Usage{\name~a\\ \name!~a}
\Signature{M}{\altype{MachineInteger}}
\Params{ {\em a} & M & A matrix whose rank has to be computed }
\Retval{ Returns the rank of {\em a}.  }
\Remarks{\name!~does not make a copy of $a$, but performs all the
computations in--place, modifying the entries of $a$.}
\alseealso{\alexp{span}}
#endif
	}
	rowEchelon: M -> (M,I->I,I,ARR I,I);
	rowEchelon!: M -> (I->I,I,ARR I,I);
	rowEchelon!: (M, M) -> (I->I,I,ARR I,I);
#if ALDOC
\alpage{rowEchelon}
\altarget{\name!}
\Usage{\name~a\\ \name!~a\\ \name!(a, b)}
\Signatures{
\name: & M $\to$ (M,Z $\to$ Z,Z,\altype{PrimitiveArray} Z,Z)\\
\name!: & M $\to$ (Z $\to$ Z,Z,\altype{PrimitiveArray} Z,Z)\\
\name!: & (M, M) $\to$ (Z $\to$ Z,Z,\altype{PrimitiveArray} Z,Z)\\
}
\begin{aswhere}
Z & == & \altype{MachineInteger}\\
\end{aswhere}
\Params{
{\em a} & M & A matrix whose REF has to be computed\\
{\em b} & M & A matrix to transform in the same way than $a$\\
}
\Descr{
We say that a matrix $a$ is in REF if there are $r$ (the rank) and
$j_1<j_2<\cdots<j_r$ (the stairs) such that $a(i,j_i)\neq 0$,
$a(i,j)=0$ for $j<j_i$ and $a(i,j)=0$ for $i>r$.

We say that a matrix $b$ is a REF of the matrix $a$ if $b$ is in REF
and there exists a non-singular matrix $u$ such that $ua=b$.

\bigskip
\name~a computes a REF of {\em a}. It returns {\em (c,p,r,st,d)}
where {\em c} is a matrix, {\em p} is a permutation, {\em r} is the
number of stairs, {\em st} are the stairs and {\em d} is the sign of {\em p}.
For $i > r$, {\em st(i)} is set to $m+1$ where $m$ is the number of columns
of {\em a}.
For $j\ge j_i$ the entry $(i,j)$ of the REF is stored as entry
$(p(i),j)$ in {\em c}. The other entries of {\em c} may have random values.\\
\name!(a, b) does the same than \name!(a) but applies all the elementary
transformations applied to $a$ also to $b$.
}
\Remarks{\name!~does not make a copy of $a$, but performs all the
computations in--place, storing the final result in $a$.
In addition, \name!(a, b) performs all the computations relative to $b$
in $b$.}
\alseealso{\alexp{rowEchelonForm}}
#endif
	rowEchelonForm: M -> M;
#if ALDOC
\alpage{rowEchelonForm}
\Usage{\name~a}
\Signature{M}{M}
\Params{ {\em a} & M & A matrix whose REF has to be computed}
\Descr{
We say that a matrix $a$ is in REF if there are $r$ (the rank) and
$j_1<j_2<\cdots<j_r$ (the stairs) such that $a(i,j_i)\neq 0$,
$a(i,j)=0$ for $j<j_i$ and $a(i,j)=0$ for $i>r$.

We say that a matrix $b$ is a REF of the matrix $a$ if $b$ is in REF
and there exists a non-singular matrix $u$ such that $ua=b$.
}
\Retval{Returns a REF of {\em a}.}
\alseealso{\alexp{rowEchelon!}}
#endif
	span: M -> Array I;
	span!: M -> Array I;
#if ALDOC
\alpage{span}
\altarget{\name!}
\Usage{\name~a\\ \name!~a}
\Signature{M}{\altype{Array} \altype{MachineInteger}}
\Params{ {\em a} & M & A matrix whose span has to be computed}
\Retval{ Returns $[c_1,\dots,c_r]$ where $r$ is the rank of {\em a}
and the span of $a$ is generated by its columns $c_1,\dots,c_r$.}
\Remarks{\name!~does not make a copy of $a$, but performs all the
computations in--place, modifying the entries of $a$.}
\alseealso{\alexp{maxInvertibleSubmatrix},\alexp{rank}}
#endif
	default {
		span(a:M):Array I == span! copy a;

		if R has IntegralDomain then {
			determinant(a:M):R	== determinant! copy a;
			rank(a:M):I		== rank! copy a;

			rank!(a:M):I == {
			-- TEMPORARY: BLOODY 1.1.12p4 COMPILER BUG OTHERWISE
				-- (p,r,st,d) := rowEchelon!(a);
				(p,r,st,d) := rowEchelon!(a,
						zero(numberOfRows a, 0$I));
				r;
			}

			determinant!(a:M):R == {
			-- TEMPORARY: BLOODY 1.1.12p4 COMPILER BUG OTHERWISE
				-- (p,r,st,d) := rowEchelon!(a);
				(p,r,st,d) := rowEchelon!(a,
						zero(numberOfRows a, 0$I));
				deter(a,p,r,d);
			}
		}

		span!(a:M):Array I == {
			import from I, ARR I;
			-- TEMPORARY: BLOODY 1.1.12p4 COMPILER BUG OTHERWISE
			-- (p,r,st,d) := rowEchelon!(a);
			(p,r,st,d) := rowEchelon!(a, zero(numberOfRows a, 0$I));
			[st i for i in 1..r];
		}

		maxInvertibleSubmatrix!(a:M):(Array I, Array I) == {
			import from I, ARR I;
			-- TEMPORARY: BLOODY 1.1.12p4 COMPILER BUG OTHERWISE
			-- (p,r,st,d) := rowEchelon!(a);
			(p,r,st,d) := rowEchelon!(a, zero(numberOfRows a, 0$I));
			([p i for i in 1..r], [st i for i in 1..r]);
		}

		maxInvertibleSubmatrix(a:M):(Array I, Array I) ==
			maxInvertibleSubmatrix! copy a;


		rowEchelon!(a:M):(I->I,I,ARR I,I) ==
			rowEchelon!(a, zero(numberOfRows a, 0));

		rowEchelon(a:M): (M,I->I,I,ARR I,I) == {
			copya := copy a;
			-- TEMPORARY: BLOODY 1.1.12p4 COMPILER BUG OTHERWISE
			-- (p,r,st,d) := rowEchelon!(copya);
			(p,r,st,d) :=
				rowEchelon!(copya,zero(numberOfRows copya,0$I));
			(copya,p,r,st,d);
		}

		rowEchelonForm(a:M):M == {
			import from I, ARR I;
			(b,p,r,st,d) := rowEchelon(a);
			(n, m) := dimensions b;
			c := zero(n,m);
			for i in 1..n repeat {
				for j in st(i)..m repeat c(i,j) := b(p(i),j);
			}
			c;
		}

		extendedRowEchelon(a:M): (M,I->I,I,ARR I,I,M) == {
			copya := copy a;
			(p,r,st,d,t) := extendedRowEchelon!(copya);
			(copya,p,r,st,d,t);
		}

		extendedRowEchelon!(a:M): (I->I,I,ARR I,I,M) == {
			b := one numberOfRows a;
			(p,r,st,d) := rowEchelon!(a, b);
			(p,r,st,d,b);
		}

		extendedRowEchelonForm(a:M): (M,M) == {
			import from I, ARR I;
			(b,p,r,st,d,t) := extendedRowEchelon(a);
			(n, m) := dimensions b;
			c := zero(n,m);
			u := zero(n,n);
			for i in 1..n repeat {
				for j in st(i)..m repeat c(i,j) := b(p(i),j);
				for j in 1..n repeat u(i,j) := t(p(i),j);
			}
			(c, u);
		}

		pivot(a:M, p:I->I, c:I, r:I):I == {
			import from Boolean, R;
			n := numberOfRows a;
			l := r;
			while (l <= n and zero? a(p l, c)) repeat l := next l;
			l > n => l;
			sz := relativeSize a(p l, c);
			for ll in next(l)..n repeat {
				if (~zero?(x := a(p ll, c))) and
					((s := relativeSize x) < sz) then {
						l := ll;
						sz := s;
				}
			}
			l;
		}
	}
}


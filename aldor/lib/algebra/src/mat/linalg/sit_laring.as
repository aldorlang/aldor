------------------------   sit_laring.as   -----------------------
-- Copyright (c) Manuel Bronstein 1999
-- Copyright (c) INRIA 1999, Version 0.1.12
-- Logiciel Sum^it ©INRIA 1999, dans sa version 0.1.12
-----------------------------------------------------------------------------

#include "algebra"

macro {
	B == Boolean;
	I == MachineInteger;
	V == Vector;
	A == Array;
	ARR == PrimitiveArray;
}

#if ALDOC
\thistype{LinearAlgebraRing}
\History{Manuel Bronstein}{27/4/98}{created}
\Usage{\this: Category}
\Descr{\this~is the category of rings that export algorithms for
linear algebra for matrices over themselves.}
\begin{exports}
\category{\altype{IntegralDomain}}\\
\alexp{determinant}: & (M:MC) $\to$ M $\to$ \% & Determinant\\
\alexp{factorOfDeterminant}:
& (M:MC) $\to$ M $\to$ (B, \%) & Probable determinant\\
\alexp{invertibleSubmatrix}:
& (M:MC) $\to$ M $\to$ (B, AZ, AZ) & Probable maximal minor\\
\alexp{inverse}: & (M:MC) $\to$ M $\to$ (M, V) & Inverse\\
\alexp{kernel}: & (M:MC) $\to$ M $\to$ M & Kernel\\
\alexp{linearDependence}:
& (\altype{Generator} V, Z) $\to$ V & First dependence relation\\
\alexp{maxInvertibleSubmatrix}:
& (M:MC) $\to$ M $\to$ (AZ, AZ) & Maximal minor\\
\alexp{particularSolution}: & (M:MC) $\to$ (M, M) $\to$ (M, V) & A solution\\
\alexp{rank}: & (M:MC) $\to$ M $\to$ Z & Rank\\
\alexp{rankLowerBound}:
& (M:MC) $\to$ M $\to$ (B, Z) & Probable rank\\
\alexp{solve}: & (M:MC) $\to$ (M, M) $\to$ (M, M, V) & All solutions\\
\alexp{span}: & (M:MC) $\to$ M $\to$ AZ & Span\\
\alexp{subKernel}: & (M:MC) $\to$ M $\to$ (B, M) & Subspace of the kernel\\
\end{exports}
\begin{aswhere}
AZ &==& \altype{Array} \altype{MachineInteger}\\
B &==& \altype{Boolean}\\
MC &==& \altype{MatrixCategory} \%\\
V &==& \altype{Vector} \%\\
Z &==& \altype{MachineInteger}\\
\end{aswhere}
#endif

define LinearAlgebraRing: Category == IntegralDomain with {
	determinant: (M:MatrixCategory %) -> M -> %;
#if ALDOC
\alpage{determinant}
\Usage{\name(M)(a)}
\Signature{(M:\altype{MatrixCategory} \%)}{M $\to$ R}
\Params{
{\em M} & \altype{MatrixCategory} \% & A matrix type\\
{\em a} & M & A matrix\\
}
\Retval{ Returns the determinant of {\em a}.}
\alseealso{\alexp{factorOfDeterminant}}
#endif
	factorOfDeterminant: (M:MatrixCategory %) -> M -> (B, %);
#if ALDOC
\alpage{factorOfDeterminant}
\Usage{\name(M)(a)}
\Signature{(M:\altype{MatrixCategory} \%)}{M $\to$ (\altype{Boolean}, R)}
\Params{
{\em M} & \altype{MatrixCategory} \% & A matrix type\\
{\em a} & M & A matrix\\
}
\Retval{ Returns $(det?, d)$ such that $d$ is always a factor of
the determinant of {\em a}, and $d$ is exactly the determinant of $a$
if $det?$ is \true. }
\Remarks{$d$ can also happen to be the determinant of $a$ when $det?$ is
\false, but the algorithm was unable to prove it.}
\alseealso{\alexp{determinant}}
#endif
	inverse: (M:MatrixCategory %) -> M -> (M, V %);
#if ALDOC
\alpage{inverse}
\Usage{\name(M)(a)}
\Signature{(M:\altype{MatrixCategory} \%)}{M $\to$ (M, \altype{Vector} R)}
\Params{
{\em M} & \altype{MatrixCategory} \% & A matrix type\\
{\em a} & M & A matrix\\
}
\Retval{ Returns $(b, [d_1,\dots,d_n])$ such that
$$
a b = \pmatrix{
d_1 &     &        & \cr
    & d_2 &        & \cr
    &     & \ddots & \cr
    &     &        & d_n \cr
}\,.
$$
}
\Remarks{$\prod_{i=1}^n d_i \ne 0$ if and only if $a$ is invertible.
When $R$ is a \altype{Field}, then $d_i \in \{0,1\}$ for $1 \le i \le n$,
so $b = a^{-1}$ when $R$ is a \altype{Field} and $a$ is invertible.}
#endif
	invertibleSubmatrix: (M:MatrixCategory %) -> M -> (B, A I, A I);
#if ALDOC
\alpage{invertibleSubmatrix}
\Usage{\name(M)(a)}
\Signature{(M:\altype{MatrixCategory} \%)}{M $\to$ (\altype{Boolean}, AZ, AZ)}
\begin{aswhere}
AZ &==& \altype{Array} \altype{MachineInteger}\\
\end{aswhere}
\Params{
{\em M} & \altype{MatrixCategory} \% & A matrix type\\
{\em a} & M & A matrix\\
}
\Retval{ Returns $(max?, [r_1,\dots,r_r], [c_1,\dots,c_r])$
where $r \le \mbox{rank}(a)$
and the submatrix of $a$ formed by the intersections of the rows $r_i$
and $c_i$ is always invertible. If $max?$ is \true, then $r$ is exactly
the rank of $a$ and the given minor is of maximal size.}
\Remarks{$r$ can also happen to be the rank of $a$ when $max?$ is \false,
but the algorithm was unable to prove it.}
\alseealso{\alexp{maxInvertibleSubmatrix}}
#endif
	kernel: (M:MatrixCategory %) -> M -> M;
#if ALDOC
\alpage{kernel}
\Usage{\name(M)(a)}
\Signature{(M:\altype{MatrixCategory} \%)}{M $\to$ M}
\Params{
{\em M} & \altype{MatrixCategory} \% & A matrix type\\
{\em a} & M & A matrix\\
}
\Retval{ Returns a matrix whose columns form a basis of the kernel of {\em a}.}
\alseealso{\alexp{solve},\alexp{subKernel}}
#endif
	linearDependence: (Generator V %, I) -> V %;
#if ALDOC
\alpage{linearDependence}
\Usage{\name(gen,n)}
\Signature{(\altype{Generator} \altype{Vector} R, \altype{MachineInteger})}
{\altype{Vector} R}
\Params{
{\em gen} & \altype{Generator} \altype{Vector} R & A generator of vectors\\
{\em n} & \altype{MachineInteger} & The dimension of the vectors generated\\
}
\Descr{
Returns a vector {\em v} which contains the coefficients
of a dependence relation among the vectors generated by {\em gen}. The
relation is as small as possible, meaning that if {\em v} has
dimension {\em m} then the first $m-1$ vectors generated are
independent. The dimension of the vectors generated by {\em gen} must be
{\em n}. There must be a relation between the vectors generated.
}
#endif
	maxInvertibleSubmatrix: (M:MatrixCategory %) -> M -> (A I, A I);
#if ALDOC
\alpage{maxInvertibleSubmatrix}
\Usage{\name(M)(a)}
\Signature{(M:\altype{MatrixCategory} \%)}{M $\to$ (AZ, AZ)}
\begin{aswhere}
AZ &==& \altype{Array} \altype{MachineInteger}\\
\end{aswhere}
\Params{
{\em M} & \altype{MatrixCategory} \% & A matrix type\\
{\em a} & M & A matrix\\
}
\Retval{ Returns $([r_1,\dots,r_r], [c_1,\dots,c_r])$
where $r$ is the rank of {\em a}
and the submatrix of $a$ formed by the intersections of the rows $r_i$
and $c_i$ is invertible.}
\alseealso{\alexp{invertibleSubmatrix}}
#endif
	particularSolution: (M:MatrixCategory %) -> (M, M) -> (M, V %);
#if ALDOC
\alpage{particularSolution}
\Usage{\name(M)(a, b)}
\Signature{(M:\altype{MatrixCategory} \%)}{(M, M) $\to$ (M, \altype{Vector} R)}
\Params{
{\em M} & \altype{MatrixCategory} \% & A matrix type\\
{\em a, b} & M & Matrices\\
}
\Retval{ Returns $(m, [d_1,\dots,d_n])$ such that
$$
a m = b \pmatrix{
d_1 &     &        & \cr
    & d_2 &        & \cr
    &     & \ddots & \cr
    &     &        & d_n \cr
}\,.
$$
}
\Remarks{For each $i$, $d_i \ne 0$ if and only if the system
$a x = \sth{i}$ column of $b$ has a solution, which is then $d_i^{-1}$
times the $\sth{i}$ column of $m$.
When $R$ is a \altype{Field}, then $d_i \in \{0,1\}$ for $1 \le i \le n$,
so $m$ is a solution of $a x = b$ when $R$ is a \altype{Field} and all
the $d_i$'s are nonzero.}
\alseealso{\alexp{solve}}
#endif
	rank: (M:MatrixCategory %) -> M -> I;
#if ALDOC
\alpage{rank}
\Usage{\name(M)(a)}
\Signature{(M:\altype{MatrixCategory} \%)}{M $\to$ \altype{MachineInteger}}
\Params{
{\em M} & \altype{MatrixCategory} \% & A matrix type\\
{\em a} & M & A matrix\\
}
\Retval{ Returns the rank of {\em a}.  }
\alseealso{\alexp{rankLowerBound},\alexp{span}}
#endif
	rankLowerBound: (M:MatrixCategory %) -> M -> (B, I);
#if ALDOC
\alpage{rankLowerBound}
\Usage{\name(M)(a)}
\Signature{(M:\altype{MatrixCategory} \%)}{M $\to$ (\altype{Boolean}, \altype{MachineInteger})}
\Params{
{\em M} & \altype{MatrixCategory} \% & A matrix type\\
{\em a} & M & A matrix\\
}
\Retval{ Returns $(rank?, r)$ such that $r \le \mbox{rank}(a)$,
and $r$ is exactly the rank of $a$ if $rank?$ is \true. }
\Remarks{$r$ can also happen to be the rank of $a$ when $rank?$ is \false,
but the algorithm was unable to prove it.}
\alseealso{\alexp{rank},\alexp{span}}
#endif
	solve: (M:MatrixCategory %) -> (M, M) -> (M, M, V %);
#if ALDOC
\alpage{solve}
\Usage{\name(M)(a, b)}
\Signature{(M:\altype{MatrixCategory} \%)}{(M, M) $\to$ (M, M, \altype{Vector} R)}
\Params{
{\em M} & \altype{MatrixCategory} \% & A matrix type\\
{\em a, b} & M & Matrices\\
}
\Retval{ Returns $(w, m, [d_1,\dots,d_n])$ such that the columns
of $w$ form a basis of the kernel of $a$ and
$$
a m = b \pmatrix{
d_1 &     &        & \cr
    & d_2 &        & \cr
    &     & \ddots & \cr
    &     &        & d_n \cr
}\,.
$$
}
\Remarks{For each $i$, $d_i \ne 0$ if and only if the system
$a x = \sth{i}$ column of $b$ has a solution, which is then $d_i^{-1}$
times the $\sth{i}$ column of $m$.
When $R$ is a \altype{Field}, then $d_i \in \{0,1\}$ for $1 \le i \le n$,
so the general solution of $a x = b$ when $R$ is a \altype{Field} and all
the $d_i$'s are nonzero is $x = m + \sum_j r_j w_j$ where $w_j$ is the
$\sth{j}$ column of $w$.}
\alseealso{\alexp{kernel},\alexp{particularSolution}}
#endif
	span: (M:MatrixCategory %) -> M -> A I;
#if ALDOC
\alpage{span}
\Usage{\name(M)(a)}
\Signature{(M:\altype{MatrixCategory} \%)}{M $\to$ \altype{Array} \altype{MachineInteger}}
\Params{
{\em M} & \altype{MatrixCategory} \% & A matrix type\\
{\em a} & M & A matrix\\
}
\Retval{ Returns $[c_1,\dots,c_r]$ where $r$ is the rank of {\em a}
and the span of $a$ is generated by its columns $c_1,\dots,c_r$.}
\alseealso{\alexp{rank}}
#endif
	subKernel: (M:MatrixCategory %) -> M -> (B, M);
#if ALDOC
\alpage{subKernel}
\Usage{\name(M)(a)}
\Signature{(M:\altype{MatrixCategory} \%)}{M $\to$ (\altype{Boolean}, M)}
\Params{
{\em M} & \altype{MatrixCategory} \% & A matrix type\\
{\em a} & M & A matrix\\
}
\Retval{ Returns $(ker?, m)$ such that the columns of $m$, which are always
linearly independent over R, generate a subspace of the kernel of $a$,
and generate the full kernel if $ker?$ is \true.}
\Remarks{$m$ can also happen to generate the full kernel of $a$ when $ker?$ is
\false, but the algorithm was unable to prove it.}
\alseealso{\alexp{kernel}}
#endif
	default {
		factorOfDeterminant(M:MatrixCategory %):M -> (B, %) == {
			(m:M):(B, %) +-> (true, determinant(M)(m));
		}

		rankLowerBound(M:MatrixCategory %):M -> (B, I) == {
			(m:M):(B, I) +-> (true, rank(M)(m));
		}

		invertibleSubmatrix(M:MatrixCategory %):M -> (B, A I, A I) == {
			(m:M):(B, A I, A I) +-> {
				(r, c) := maxInvertibleSubmatrix(M)(m);
				(true, r, c);
			}
		}

		subKernel(M:MatrixCategory %):M -> (B, M) == {
			(m:M):(B, M) +-> (true, kernel(M)(m));
		}
	}
}

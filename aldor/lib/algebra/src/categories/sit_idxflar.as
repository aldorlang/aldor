----------------------------- sit_idxflar.as ----------------------------------
-- Copyright (c) Manuel Bronstein 2001
-- Copyright (c) INRIA 2001
-----------------------------------------------------------------------------

#include "algebra"

#if ALDOC
\thistype{IndexedFreeLinearArithmeticType}
\History{Manuel Bronstein}{26/4/2001}{extracted from UnivariateFreeAlgebra}
\Usage{\this(R, E): Category}
\Params{
{\em R} & \altype{ExpressionType} & The coefficient domain\\
        & \altype{ArithmeticType} &\\
{\em E} & \altype{ExpressionType} & The index domain\\
}
\Descr{\this~R is the category of arithmetic types containing linear
combinations of their elements with coefficients in R
with respect to a linearly independent generating set.
Its elements are not assumed to have finite support, so this type
cannot be asserted to be a \altype{Algebra}~R even when R is a \altype{Ring}.}
\begin{exports}
\category{\altype{IndexedFreeLinearCombinationType}(R, E)}\\
\category{\altype{FreeLinearArithmeticType} R}\\
\alexp{add!}: & (\%, R, E, \%) $\to$ \% & Add a shifted element\\
\end{exports}
#endif

define IndexedFreeLinearArithmeticType(R:Join(ArithmeticType, ExpressionType),
	E: ExpressionType): Category ==
		Join(IndexedFreeLinearCombinationType(R, E),
					FreeLinearArithmeticType R) with {
		add!: (%, R, E, %) -> %;
#if ALDOC
\alpage{add!}
\Usage{\name(p, c, e, q)}
\Signature{(\%, R, E, \%)}{\%}
\Params{
{\em p} & \% & An element of the module (to be destroyed)\\
{\em c} & R & A scalar\\
{\em e} & E & The degree of the term to add\\
{\em q} & \% & An element of the module\\
}
\Retval{\name(p, c, e, q) computes the sum $p + c e q$.}
\Remarks{The storage used by p is allowed to be destroyed or reused, so p
is lost after this call. This may cause p to be destroyed, so do not use
this unless p has been locally allocated, and is thus guaranteed not to
share space with other polynomials.}
#endif
}


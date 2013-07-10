----------------------------- sit_freelc.as ----------------------------------
-- Copyright (c) Manuel Bronstein 2001
-- Copyright (c) INRIA 2001
-----------------------------------------------------------------------------

#include "algebra"

#if ALDOC
\thistype{FreeLinearCombinationType}
\History{Manuel Bronstein}{26/4/2001}{extracted from UnivariateFreeAlgebra}
\Usage{\this~R: Category}
\Params{
{\em R} & \altype{ExpressionType} & The coefficient domain\\
        & \altype{ArithmeticType} &\\
}
\Descr{\this~R is the category of types containing linear
combinations of their elements with coefficients in R with
respect to a linearly independent generating set.
Its elements are not assumed to have finite support, so this type
cannot be asserted to be a \altype{Module}~R even when R is a \altype{Ring}.}
\begin{exports}
\category{\altype{ExpressionType}}\\
\category{\altype{LinearCombinationType} R}\\
\alexp{map}: & (R $\to$ R) $\to$ \% $\to$ \% & Lift a mapping\\
\alexp{map!}: & (R $\to$ R) $\to$ \% $\to$ \% & Lift a mapping\\
\end{exports}
#endif

define FreeLinearCombinationType(R:Join(ArithmeticType, ExpressionType)):
	Category == Join(ExpressionType, LinearCombinationType R) with {
	map: (R -> R) -> % -> %;
	map!: (R -> R) -> % -> %;
#if ALDOC
\alpage{map}
\altarget{\name!}
\Usage{\name~f\\\name!~f\\\name(f)(m)\\\name!(f)(m)}
\Signature{(R $\to$ R) $\to$ \%}{\%}
\Params{
{\em f} & R $\to$ R & A map\\
{\em m} & \% & An element of the module\\
}
\Descr{
\name(f)(m) returns
$$
f(m) = \sum_i f(r_i) e_i
$$
where $m = \sum_i r_i e_i$,
while \name(f) returns the mapping $m \to f(m)$. In both cases,
\name!~does not make a copy of $m$ but modifies it in place.}
#endif
	default {
		map!(f:R -> R)(p:%):% == map(f) p;

		if R has Ring then {
			(n:Integer) * (p:%):% == { import from R; n::R * p; }
		}
	}
}

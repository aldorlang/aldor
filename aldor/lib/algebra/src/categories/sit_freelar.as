----------------------------- sit_freelar.as ----------------------------------
-- Copyright (c) Manuel Bronstein 2001
-- Copyright (c) INRIA 2001
-----------------------------------------------------------------------------

#include "algebra"

#if ALDOC
\thistype{FreeLinearArithmeticType}
\History{Manuel Bronstein}{26/4/2001}{extracted from UnivariateFreeAlgebra}
\Usage{\this~R: Category}
\Params{
{\em R} & \altype{ExpressionType} & The coefficient domain\\
        & \altype{ArithmeticType} &\\
}
\Descr{\this~R is the category of arithmetic types containing linear
combinations of their elements with coefficients in R
with respect to a linearly independent generating set.
Its elements are not assumed to have finite support, so this type
cannot be asserted to be a \altype{Algebra}~R even when R is a \altype{Ring}.}
\begin{exports}
\category{\altype{FreeLinearCombinationType} R}\\
\category{\altype{LinearArithmeticType} R}\\
\end{exports}
#endif

define FreeLinearArithmeticType(R:Join(ArithmeticType, ExpressionType)):
	Category == Join(FreeLinearCombinationType R, LinearArithmeticType R);


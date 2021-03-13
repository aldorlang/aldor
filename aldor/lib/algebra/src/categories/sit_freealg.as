----------------------------- sit_freealg.as ----------------------------------
-- Copyright (c) Manuel Bronstein 2001
-- Copyright (c) Marc Moreno Maza 2001
-- Copyright (c) INRIA 2001
-- Copyright (c) LIFL 2001
-----------------------------------------------------------------------------

#include "algebra"

#if ALDOC
\thistype{FreeAlgebra}
\History{Manuel Bronstein/Marc Moreno Maza}{26/4/2001}
{merged from UnivariateFreeFiniteAlgebra (Sumit),
and DirectProductAsModule and DirectSumAsModule (BasicMath)}
\Usage{\this~R: Category}
\Params{
{\em R} & \altype{ExpressionType} & The coefficient domain\\
        & \altype{ArithmeticType} &\\
}
\Descr{\this~R is a category for free algebras over an
arbitrary arithmetic system R and with respect to an arbitrary basis.
Its elements are assumed to have finite support.}
\begin{exports}
\category{\altype{FreeRRing} R}\\
\end{exports}
\begin{exports}[If R has \altype{Ring} then]
\category{\altype{Algebra} R}\\
\end{exports}
#endif

define FreeAlgebra(R:Join(ArithmeticType, ExpressionType)):
	Category == FreeRRing R with {
	if R has Ring then Algebra R
}


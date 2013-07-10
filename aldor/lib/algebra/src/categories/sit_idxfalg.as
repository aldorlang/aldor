----------------------------- sit_idxfalg.as ----------------------------------
-- Copyright (c) Manuel Bronstein 2001
-- Copyright (c) Marc Moreno Maza 2001
-- Copyright (c) INRIA 2001
-- Copyright (c) LIFL 2001
-----------------------------------------------------------------------------

#include "algebra"

#if ALDOC
\thistype{IndexedFreeAlgebra}
\History{Manuel Bronstein/Marc Moreno Maza}{26/4/2001}
{merged from UnivariateFreeFiniteAlgebra (Sumit),
and DirectProductAsModule and DirectSumAsModule (BasicMath)}
\Usage{\this(R,E): Category}
\Params{
{\em R} & \altype{ExpressionType} & The coefficient domain\\
        & \altype{ArithmeticType} &\\
{\em E} & \altype{ExpressionType} & The index domain\\
        & \altype{TotallyOrderedType} &\\
}
\Descr{\this(R, E) is a category for free algebras over an
arbitrary arithmetic system {\em R},
with respect to a linearly independent generating set {\em E}.
Its elements are assumed to have finite support.}
\begin{exports}
\category{\altype{IndexedFreeRRing}(R, E)}\\
\end{exports}
#endif

define IndexedFreeAlgebra(R:Join(ArithmeticType, ExpressionType),
			E:Join(TotallyOrderedType, ExpressionType)):
	Category == IndexedFreeRRing(R, E) with {};


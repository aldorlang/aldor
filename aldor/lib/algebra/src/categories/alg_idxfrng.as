----------------------------- alg_idxfrng.as ----------------------------------
-- Copyright (c) Manuel Bronstein 2004
-- Copyright (c) Marc Moreno Maza 2004
-- Copyright (c) INRIA 2004
-- Copyright (c) LIFL 2004
-----------------------------------------------------------------------------

#include "algebra"

#if ALDOC
\thistype{IndexedFreeRRing}
\History{Manuel Bronstein/Marc Moreno Maza}{17/11/2004}
{formerly called IndexedFreeAlgebra}
\Usage{\this(R,E): Category}
\Params{
{\em R} & \altype{ExpressionType} & The coefficient domain\\
        & \altype{ArithmeticType} &\\
{\em E} & \altype{ExpressionType} & The index domain\\
        & \altype{TotallyOrderedType} &\\
}
\Descr{\this(R, E) is a category for free R-rings over an
arbitrary arithmetic system {\em R},
with respect to a linearly independent generating set {\em E}.
Its elements are assumed to have finite support.}
\begin{exports}
\category{\altype{FreeRRing} R}\\
\category{\altype{IndexedFreeModule}(R, E)}\\
\category{\altype{IndexedFreeLinearArithmeticType}(R, E)}\\
\end{exports}
#endif

define IndexedFreeRRing(R:Join(ArithmeticType, ExpressionType),
			E:Join(TotallyOrderedType, ExpressionType)):
	Category == Join(FreeRRing R, IndexedFreeModule(R, E),
			IndexedFreeLinearArithmeticType(R, E));


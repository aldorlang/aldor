----------------------------- alg_frering.as ----------------------------------
-- Copyright (c) Manuel Bronstein 2004
-- Copyright (c) Marc Moreno Maza 2004
-- Copyright (c) INRIA 2004
-- Copyright (c) LIFL 2004
-----------------------------------------------------------------------------

#include "algebra"

#if ALDOC
\thistype{FreeRRing}
\History{Manuel Bronstein/Marc Moreno Maza}{17/11/2004}
{formerly called FreeAlgebra}
\Usage{\this~R: Category}
\Params{
{\em R} & \altype{ExpressionType} & The coefficient domain\\
        & \altype{ArithmeticType} &\\
}
\Descr{\this~R is a category for free R-rings over an
arbitrary arithmetic system R and with respect to an arbitrary basis.
Its elements are assumed to have finite support.}
\begin{exports}
\category{\altype{FreeLinearArithmeticType} R}\\
\category{\altype{FreeModule} R}\\
\alexp{ground?}: & \% $\to$ \altype{Boolean} & Test for a ground element\\
\end{exports}
\begin{exports}[If $R$ has \altype{CharacteristicZero} then]
\category{\altype{CharacteristicZero}}\\
\end{exports}
\begin{exports}[If $R$ has \altype{FiniteCharacteristic} then]
\category{\altype{FiniteCharacteristic}}\\
\end{exports}
\begin{exports}[If R has \altype{Ring} then]
\category{\altype{RRing} R}\\
\end{exports}
\begin{exports}[if $R$ has \altype{RittRing} then]
\category{\altype{RittRing}}\\
\end{exports}
#endif

define FreeRRing(R:Join(ArithmeticType, ExpressionType)):
	Category == Join(FreeModule R, FreeLinearArithmeticType R) with {
	if R has Ring then RRing R;
	if R has CharacteristicZero then CharacteristicZero;
	if R has FiniteCharacteristic then FiniteCharacteristic;
	if R has RittRing then RittRing;
	ground?: % -> Boolean;
#if ALDOC
\alpage  {ground?}
\Usage{ \name~m }
\Signature {\%} {\altype{Boolean}}
\Params{
\emph{m} & \% & An element of the module \\
}
\Retval{
Returns \true{} if $m = r \cdot 1$ for $r \in R$,
        \false{} otherwise.
}
#endif
	default {
		ground?(x:%):Boolean == leadingCoefficient(x)::% = x;
		if R has RittRing then
			inv(n:Integer):% == { import from R; inv(n)$R :: % };
	}
}


-------------------------- sit_linarith.as ----------------------------------
-- Copyright (c) Manuel Bronstein 2000
-- Copyright (c) INRIA 1999, Version 0.1.12
-- Logiciel Sum^it (c) INRIA 1999, dans sa version 0.1.12
-----------------------------------------------------------------------------

#include "algebra"

#if ALDOC
\thistype{LinearArithmeticType}
\History{Manuel Bronstein}{14/2/2000}{created}
\Usage{\this~R:Category}
\Params{
{\em R} & \altype{ExpressionType} & The coefficient domain\\
        & \altype{AdditiveType} &\\
}
\Descr{\this~R is the category of arithmetic types containing linear
combinations of their elements with coefficients in R.}
\Remarks{Use \altype{Algebra} instead
if R is always meant to be a \altype{Ring}.}
\begin{exports}
\category{\altype{ArithmeticType}}\\
\category{\altype{ExpressionType}}\\
\category{\altype{LinearCombinationType} R}\\
\alalias{\this}{**}{$\land$}:
& (\%, \altype{Integer}) $\to$ \% & exponentiation\\
\alexp{coerce}: & R $\to$ \% & Natural embedding\\
\end{exports}
#endif

define LinearArithmeticType(R:Join(AdditiveType, ExpressionType)): Category ==
	Join(ArithmeticType, ExpressionType, LinearCombinationType R) with {
		^: (%, Integer) -> %;
#if ALDOC
\alpage{**}
\Usage{$x\land n$}
\Signatures{$\land$: (\%,\altype{Integer}) $\to$ \%}
\Params{
{\em x} & \% & an element of the type\\
{\em n} & \altype{Integer} & an exponent\\
}
\Retval{Returns $x$ to the power $n$.}
#endif
		coerce: R -> %;
#if ALDOC
\alpage{coerce}
\Usage{\name~r}
\Signature{R}{\%}
\Params{ {\em r} & R & An element of the base type\\ }
\Retval{Returns $r \cdot 1$.}
#endif

	default {
		coerce(r:R):% == r * 1;

		(p:%) ^ (n:Integer):% == {
			import from BinaryPowering(%, Integer);
			assert(n >= 0);
			binaryExponentiation(p, n);
		}

		if R has Ring then {
			(n:Integer) * (p:%):% == { import from R; n::R * p; }
		}
	}
}

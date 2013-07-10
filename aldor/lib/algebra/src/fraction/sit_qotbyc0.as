------------------------ sit_qotbyc0.as ---------------------------
-- Copyright (c) Manuel Bronstein 1998
-- Copyright (c) INRIA 1999, Version 0.1.12
-- Logiciel Sum^it ©INRIA 1999, dans sa version 0.1.12
-----------------------------------------------------------------------------

#include "algebra"

macro Z == Integer;

#if ALDOC
\thistype{FractionByCategory0}
\History{Manuel Bronstein}{9/6/98}{created}
\Usage{\this~R: Category}
\Params{ {\em R} & \astype{IntegralDomain} & an integral domain\\ }
\Descr{
\this(R) is the category of fractions of the integral domain {\em R} by
some nonzero nonunit $p \in R$,
\ie the set of all fractions whose denominator is a power of {\em p}.
}
\begin{exports}
\category{\astype{FractionCategory} R}\\
\asexp{order}: & \% $\to$ \astype{Integer} & Valuation at p\\
\asexp{shift}:
& (\%, \astype{Integer}) $\to$ \% & Multiplication by a power of p\\
\end{exports}
#endif

define FractionByCategory0(R:IntegralDomain):Category==FractionCategory R with {
	order: % -> Z;
#if ALDOC
\aspage{order}
\Usage{\name~x}
\Signature{\%}{\astype{Integer}}
\Params{ {\em x} & \% & A fraction whose denominator is a power of p\\ }
\Retval{Returns $n$ such that $x = a p^n$ and $a \in R$, $p \nodiv a$.}
#endif
	shift: (%, Z) -> %;
#if ALDOC
\aspage{shift}
\Usage{\name(x, n)}
\Signature{(\%, \astype{Integer})}{\%}
\Params{
{\em x} & \% & A fraction whose denominator is a power of p\\
{\em n} & \astype{Integer} & An exponent\\
}
\Retval{Returns $x p^n$.}
#endif
}

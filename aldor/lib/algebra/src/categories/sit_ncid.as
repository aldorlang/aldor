------------------------------ sit_ncid.as ----------------------------------
-- Copyright (c) Manuel Bronstein 1994
-- Copyright (c) INRIA 1999, Version 0.1.12
-- Logiciel Sum^it (c) INRIA 1999, dans sa version 0.1.12
-- Copyright (c) Swiss Federal Polytechnic Institute Zurich, 1994-97
-----------------------------------------------------------------------------

#include "algebra"

#if ALDOC
\thistype{NonCommutativeIntegralDomain}
\History{Manuel Bronstein}{22/11/94}{created}
\Usage{\this: Category}
\Descr{\this~is the category of non--commutative integral domains.}
\begin{exports}
\category{\astype{Ring}}\\
\asexp{leftExactQuotient}:
& (\%, \%) $\to$ \astype{Partial} \% & Left exact quotient\\
\asexp{rightExactQuotient}:
& (\%, \%) $\to$ \astype{Partial} \% & Right exact quotient\\
\end{exports}
#endif

define NonCommutativeIntegralDomain: Category == Ring with {
	leftExactQuotient: (%, %) -> Partial %;
#if ALDOC
\aspage{leftExactQuotient}
\Usage{\name(x, y)}
\Signature{(\%, \%)}{\astype{Partial} \%}
\Params{
{\em x} & \% & The numerator\\
{\em y} & \% & The denominator\\
}
\Retval{Returns either $q$ such that $x = y\, q$ if such a $q$ exists,
\failed otherwise.}
\alseealso{rightExactQuotient(\this)}
#endif
	rightExactQuotient: (%, %) -> Partial %;
#if ALDOC
\aspage{rightExactQuotient}
\Usage{\name(x, y)}
\Signature{(\%, \%)}{\astype{Partial} \%}
\Params{
{\em x} & \% & The numerator\\
{\em y} & \% & The denominator\\
}
\Retval{Returns either $q$ such that $x = q\, y$ if such a $q$ exists,
\failed otherwise.}
\alseealso{leftExactQuotient(\this)}
#endif
	default { commutative?:Boolean == false }
}

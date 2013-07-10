------------------------------ sit_group.as ----------------------------------
-- Copyright (c) Manuel Bronstein 1999
-- Copyright (c) INRIA 1999, Version 0.1.12
-- Logiciel Sum^it (c) INRIA 1999, dans sa version 0.1.12
-----------------------------------------------------------------------------

#include "algebra"

#if ALDOC
\thistype{Group}
\History{Manuel Bronstein}{24/3/99}{created}
\Usage{\this: Category}
\Descr{\this~is the category of groups, not necessarily commutative.}
\begin{exports}
\category{\astype{Monoid}}\\
\asexp{/}: & (\%, \%) $\to$ \% & quotient\\
\asexp{inv}: & \% $\to$ \% & inverse\\
\end{exports}
#endif

define Group: Category == Monoid with {
	/: (%, %) -> %;
#if ALDOC
\aspage{/}
\Usage{$x \name y$}
\Signature{\%}{\%}
\Params{ {\em x,y} & \% & Elements of the group\\ }
\Retval{Returns $x y^{-1}$.}
#endif
	inv: % -> %;
#if ALDOC
\aspage{inv}
\Usage{\name~x}
\Signature{\%}{\%}
\Params{ {\em x} & \% & An element of the group\\ }
\Retval{Returns $x^{-1}$.}
#endif
	default (x:%) / (y:%):% == x * inv y;
}

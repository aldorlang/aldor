------------------------------- sit_qring.as ----------------------------------
-- Copyright (c) Manuel Bronstein 1997
-- Copyright (c) INRIA 1999, Version 0.1.12
-- Logiciel Sum^it (c) INRIA 1999, dans sa version 0.1.12
-- Copyright (c) Swiss Federal Polytechnic Institute Zurich, 1994-97
-----------------------------------------------------------------------------

#include "algebra"

#if ALDOC
\thistype{RittRing}
\History{Manuel Bronstein}{17/12/97}{created}
\Usage{\this: Category}
\Descr{\this~is the category of rings of characteristic {\em 0}
in which all nonzero integers are invertible. The center of such a ring
contains an isomorphic image of the rational numbers.}
\begin{exports}
\category{\altype{CharacteristicZero}}\\
\alexp{$/$}: & (\%, \altype{Integer}) $\to$ \% & Division by a nonzero integer\\
\alexp{inv}: & \altype{Integer} $\to$ \% & Inversion of a nonzero integer\\
\end{exports}
#endif

define RittRing: Category == CharacteristicZero with {
	/: (%, Integer) -> %;
#if ALDOC
\alpage{$/$}
\Usage{$x$ \name $n$}
\Signature{(\%,\altype{Integer})}{\%}
\Params{
{\em x} & \% & An element of the ring\\
{\em n} & \altype{Integer} & A nonzero integer\\
}
\Retval{Returns $x/n$ as an element of the ring.}
#endif
	inv: Integer -> %;
#if ALDOC
\alpage{inv}
\Usage{\name~n}
\Signature{\altype{Integer}}{\%}
\Params{ {\em n} & \altype{Integer} & A nonzero integer\\ }
\Retval{Returns $1/n$ as an element of the ring.}
#endif
	default {
		(x:%) / (n:Integer):% == x * inv n;
		if % has Field then { inv(n:Integer):% == inv(n::%) }
	}
}

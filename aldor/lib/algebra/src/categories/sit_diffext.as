----------------------------- sit_diffext.as ----------------------------------
--
-- Differential extensions
--
-- Copyright (c) Manuel Bronstein 1994
-- Copyright (c) INRIA 1999, Version 0.1.12
-- Logiciel Sum^it (c) INRIA 1999, dans sa version 0.1.12
-- Copyright (c) Swiss Federal Polytechnic Institute Zurich, 1994-97
-----------------------------------------------------------------------------

#include "algebra"

#if ALDOC
\thistype{DifferentialExtension}
\History{Manuel Bronstein}{22/12/94}{created}
\Usage{\this~R: Category}
\Params{ {\em R} & \astype{CommutativeRing} & The base ring\\ }
\Descr{\this(R)~is the category of differential extensions of R.}
\begin{exports}
\category{\astype{CommutativeRing}}\\
\asexp{lift}: & \astype{Derivation} R $\to$ \astype{Derivation} \% &
Extension of a derivation\\
\end{exports}
\begin{exports}[if R has \astype{DifferentialRing} then]
\category{\astype{DifferentialRing}}\\
\end{exports}
#endif

define DifferentialExtension(R:CommutativeRing):Category==CommutativeRing with {
	lift: Derivation R -> Derivation %;
#if ALDOC
\aspage{lift}
\Usage{\name~D}
\Signature{\astype{Derivation} R}{\astype{Derivation} \%}
\Params{ {\em D} & \astype{Derivation} R & A derivation on R\\ }
\Retval{Returns the derivation $D$ extended to the ring extension.}
#endif
	if R has DifferentialRing then DifferentialRing;
	default {
		if R has DifferentialRing then {
			differentiate(x:%):% == {
				import from Derivation %;
				lift(derivation$R) x;
			}
		}
	}
}


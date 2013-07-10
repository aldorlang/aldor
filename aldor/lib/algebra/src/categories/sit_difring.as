----------------------------- sit_difring.as ----------------------------------
--
-- Differential rings
--
-- Copyright (c) Manuel Bronstein 1994
-- Copyright (c) INRIA 1999, Version 0.1.12
-- Logiciel Sum^it (c) INRIA 1999, dans sa version 0.1.12
-- Copyright (c) Swiss Federal Polytechnic Institute Zurich, 1994-97
-----------------------------------------------------------------------------

#include "algebra"

#if ALDOC
\thistype{DifferentialRing}
\History{Manuel Bronstein}{21/12/94}{created}
\Usage{\this: Category}
\Descr{\this~is the category of commutative differential rings.}
\begin{exports}
\category{\astype{CommutativeRing}}\\
\asexp{derivation}: & $\to$ \astype{Derivation} \% & The derivation\\
\asexp{differentiate}:
& (\%, \astype{Integer}) $\to$ \% & Differentiate an element\\
\end{exports}
#endif

define DifferentialRing: Category == CommutativeRing with {
	derivation: Derivation %;
#if ALDOC
\aspage{derivation}
\Usage{\name}
\alconstant{\astype{Derivation} \%}
\Retval{Returns the derivation of the ring.}
\alseealso{differentiate(\this)}
#endif
	differentiate: % -> %;
	differentiate: (%, Integer) -> %;
#if ALDOC
\aspage{differentiate}
\Usage{\name~x\\\name(x, n)}
\Signature{(\%, \astype{Integer})}{\%}
\Params{
{\em x} & \% & The element to differentiate\\
{\em n} & \astype{Integer} & The order of differentiation (optional)\\
}
\Retval{
\name~x returns $x'$, the derivative of $x$.\\
\name(x, n) returns $x^{(n)}$, the \Th{n} derivative of $x$.
}
\alseealso{derivation(\this)}
#endif
	default {
		derivation:Derivation % ==
			derivation((r:%):% +-> differentiate r);

		differentiate(x:%, n:Integer):% == {
			import from Derivation %;
			assert(n >= 0);
			apply(derivation, x, n);
		}
	}
}


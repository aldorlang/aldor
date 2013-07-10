------------------------------ sit_dcmprng.as ----------------------------------
-- Copyright (c) Manuel Bronstein 2000
-- Copyright (c) INRIA 1999, Version 0.1.12
-- Logiciel Sum^it (c) INRIA 1999, dans sa version 0.1.12
-----------------------------------------------------------------------------

#include "algebra"

#if ALDOC
\thistype{DecomposableRing}
\History{Manuel Bronstein}{17/2/2000}{created}
\Usage{\this: Category}
\Descr{\this~is the category of commutative rings whose elements can
sometimes be decomposed into products (not to be confused with true
factorization).}
\begin{exports}
\category{\astype{CommutativeRing}}\\
\asexp{provablyIrreducible?}: & \% $\to$ \astype{Boolean}\\
\asexp{someFactors}: & \% $\to$ \astype{List} \% & Get some factors\\
\end{exports}
#endif

define DecomposableRing: Category == CommutativeRing with {
	provablyIrreducible?: % -> Boolean;
#if ALDOC
\aspage{provablyIrreducible?}
\Usage{\name~x}
\Signature{\%}{\astype{Boolean}}
\Params{ {\em x} & \% & A ring element\\ }
\Retval{Returns \true if $x$ can be proven to be irreducible, \false
if either $x$ is reducible or the proof of irreducibility cannot be
obtained quickly enough.}
\Remarks{This function is not meant to use factorization or catch all
irreducible elements, even when those functionalities are available.
It is however meant to be efficient.}
#endif
	someFactors: % -> List %;
#if ALDOC
\aspage{someFactors}
\Usage{\name~x}
\Signature{\%}{\astype{List} \%}
\Params{ {\em x} & \% & A ring element\\ }
\Retval{Returns $[x_1,\dots,x_n])$ such that each $x_i$ divides $x$ exactly.}
\Remarks{This function is not meant to use factorization or return a
complete decomposition, even when those functionalities are available.
It is however meant to be efficient.}
#endif
	default {
		someFactors(x:%):List %			== [x];
		provablyIrreducible?(x:%):Boolean	== false;
	}
}

------------------------------- sit_spzble.as ----------------------------------
-- Copyright (c) Manuel Bronstein 1995
-- Copyright (c) INRIA 1999, Version 0.1.12
-- Logiciel Sum^it (c) INRIA 1999, dans sa version 0.1.12
-- Copyright (c) Swiss Federal Polytechnic Institute Zurich, 1995-97
-----------------------------------------------------------------------------

#include "algebra"

#if ALDOC
\thistype{Specializable}
\History{Manuel Bronstein}{23/6/95}{created}
\Usage{\this: Category}
\Descr{\this~is the category of specializable types.}
\begin{exports}
\asexp{specialization}:
& (Image:\astype{CommutativeRing}) $\to$ \astype{PartialFunction}(\%, Image) &
Morphism\\
\end{exports}
#endif

define Specializable: Category == with {
	specialization: (Image:CommutativeRing) -> PartialFunction(%, Image);
#if ALDOC
\aspage{specialization}
\Usage{\name~R}
\Signature{(Image:\astype{CommutativeRing})}{\astype{PartialFunction}(\%,Image)}
\Params{ {\em R} & \astype{CommutativeRing} & A ring\\ }
\Retval{Returns a partial map from \% into R.}
#endif
}

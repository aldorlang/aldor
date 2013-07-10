-------------------------- alg_rring.as ----------------------------------
-- Copyright (c) Manuel Bronstein 2004
-- Copyright (c) INRIA 2004, Version 1.1.0
-- Logiciel libalgebra (c) INRIA 2004, dans sa version 1.1.0
-----------------------------------------------------------------------------

#include "algebra"

#if ALDOC
\thistype{RRing}
\History{Manuel Bronstein}{17/11/2004}{created}
\Usage{\this~R:Category}
\Params{{\em R} & \altype{Ring} & The coefficient ring\\}
\Descr{\this~R is the category of R-rings, \ie~rings that are also R-modules.}
\begin{exports}
\category{\altype{LinearArithmeticType} R}\\
\category{\altype{Module} R}\\
\category{\altype{Ring}}\\
\end{exports}
#endif

define RRing(R:Ring):Category == Join(Ring, Module R, LinearArithmeticType R);


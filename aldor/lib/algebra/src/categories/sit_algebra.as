-------------------------- sit_algebra.as ----------------------------------
-- Copyright (c) Manuel Bronstein 2000
-- Copyright (c) INRIA 1999, Version 0.1.12
-- Logiciel Sum^it (c) INRIA 1999, dans sa version 0.1.12
-----------------------------------------------------------------------------

#include "algebra"

#if ALDOC
\thistype{Algebra}
\History{Manuel Bronstein}{14/2/2000}{created}
\Usage{\this~R:Category}
\Params{{\em R} & \altype{Ring} & The coefficient ring\\}
\Descr{\this~R is the category of algebras over R, \ie~R-rings such
that $R \cdot 1$ is included in the center (in other word, multiplication
by R is bilinear).}
\begin{exports}
\category{\altype{RRing} R}\\
\end{exports}
#endif

define Algebra(R:Ring):Category == RRing(R) with {};


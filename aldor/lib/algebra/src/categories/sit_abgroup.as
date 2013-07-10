------------------------------ sit_abgroup.as ----------------------------------
-- Copyright (c) Manuel Bronstein 1999
-- Copyright (c) INRIA 1999, Version 0.1.12
-- Logiciel Sum^it (c) INRIA 1999, dans sa version 0.1.12
-----------------------------------------------------------------------------

#include "algebra"

#if ALDOC
\thistype{AbelianGroup}
\History{Manuel Bronstein}{24/3/99}{created}
\Usage{\this: Category}
\Descr{\this~is the category of commutative groups.}
\begin{exports}
\category{\altype{AdditiveType}}\\
\category{\altype{AbelianMonoid}}\\
\end{exports}
#endif

define AbelianGroup: Category == Join(AdditiveType, AbelianMonoid);


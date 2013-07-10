-------------------------- sit_module.as ----------------------------------
-- Copyright (c) Manuel Bronstein 1997
-- Copyright (c) INRIA 1999, Version 0.1.12
-- Logiciel Sum^it (c) INRIA 1999, dans sa version 0.1.12
-- Copyright (c) Swiss Federal Polytechnic Institute Zurich, 1997
-----------------------------------------------------------------------------

#include "algebra"

#if ALDOC
\thistype{Module}
\History{Manuel Bronstein}{17/2/97}{created}
\Usage{\this~R:Category}
\Params{{\em R} & \altype{Ring} & The coefficient ring\\}
\Descr{\this~R is the category of modules over R.}
\begin{exports}
\category{\altype{AbelianGroup}}\\
\category{\altype{ExpressionType}}\\
\category{\altype{LinearCombinationType} R}\\
\end{exports}
#endif

define Module(R:Ring): Category ==
	Join(AbelianGroup, ExpressionType, LinearCombinationType R) with {
		default (n:Integer) * (p:%):% == { import from R; n::R * p; }
	}


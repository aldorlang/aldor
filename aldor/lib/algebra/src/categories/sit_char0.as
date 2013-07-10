------------------------------- sit_char0.as ----------------------------------
-- Copyright (c) Manuel Bronstein 1997
-- Copyright (c) INRIA 1999, Version 0.1.12
-- Logiciel Sum^it (c) INRIA 1999, dans sa version 0.1.12
-----------------------------------------------------------------------------

#include "algebra"

#if ALDOC
\thistype{CharacteristicZero}
\History{Manuel Bronstein}{17/12/97}{created}
\Usage{\this: Category}
\Descr{\this~is the category of rings of characteristic $0$.}
\begin{exports}
\category{\altype{Ring}}\\
\end{exports}
#endif

define CharacteristicZero: Category == Ring with {
	default characteristic:Integer == 0;
}

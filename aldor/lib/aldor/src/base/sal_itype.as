---------------------------- sal_itype.as ---------------------------------
--
-- This file defines readable objects
--
-- Copyright (c) Manuel Bronstein 1998
-- Copyright (c) INRIA 1998, Version 29-10-98
-- Logiciel Salli ©INRIA 1998, dans sa version du 29/10/1998
-----------------------------------------------------------------------------

#include "aldor"

#if ALDOC
\thistype{InputType}
\History{Manuel Bronstein}{1/10/98}{created}
\Usage{\this: Category}
\Descr{\this~is the category of types whose objects can be read in in
text format.}
\begin{exports}
\asexp{$<<$}: & \astype{TextReader} $\to$ \% & read using text encoding\\
\end{exports}
#endif

define InputType: Category == with {
	<<: TextReader -> %;
#if ALDOC
\aspage{$<<$}
\Usage{$<<$ s}
\Signature{\astype{TextReader}}{\%}
\Params{ {\em s} & \astype{TextReader} & an input stream\\ }
\Retval{$<<$ s reads an element of the current type in text format
from the stream s and returns the element read.}
#endif
}


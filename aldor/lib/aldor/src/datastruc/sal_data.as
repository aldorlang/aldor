---------------------------- sal_data.as ----------------------------------
--
-- This file defines general data structures
--
-- Copyright (c) Manuel Bronstein 1998
-- Copyright (c) INRIA 1998, Version 29-10-98
-- Logiciel Salli ©INRIA 1998, dans sa version du 29/10/1998
-----------------------------------------------------------------------------

#include "aldor"

#if ALDOC
\thistype{DataStructureType}
\History{Manuel Bronstein}{9/7/2001}{created}
\Usage{\this: Category}
\Descr{\this~is the category of general data structures, not necessarily
finite or linear.}
\begin{exports}
\alexp{empty?}:
& \% $\to$ \altype{Boolean} & test whether a structure is empty\\
\alexp{free!}: & \% $\to$ () & memory disposal\\
\end{exports}
#endif

define DataStructureType: Category == with {
	empty?: % -> Boolean;
#if ALDOC
\alpage{empty?}
\Usage{\name~a}
\Signature{\%}{\altype{Boolean}}
\Params{ {\em a} & \% & a data structure\\ }
\Retval{Returns \true if $a$ contains no element, \false otherwise.}
#endif
	free!: % -> ();
#if ALDOC
\alpage{free!}
\Usage{\name~a}
\Signature{\%}{()}
\Params{ {\em a} & \% & a data structure\\ }
\Descr{Releases the memory occupied by {\em a}.}
#endif
}


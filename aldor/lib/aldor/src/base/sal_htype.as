----------------------------- sal_htype.as ----------------------------------
--
-- This file defines hashable types
--
-- Copyright (c) Manuel Bronstein 1999
-- Copyright (c) INRIA 1999, Version 24-06-99
-- Logiciel Salli ©INRIA 1999, dans sa version du 24/06/1999
-----------------------------------------------------------------------------

#include "aldor"

#if ALDOC
\thistype{HashType}
\History{Manuel Bronstein}{24/6/99}{created}
\Usage{\this: Category}
\Descr{\this~is the category of types whose objects can be hashed into
machine integers.}
\begin{exports}
\category{\astype{PrimitiveType}}\\
\asexp{hash}: & \% $\to$ \astype{MachineInteger} & hash function\\
\end{exports}
#endif

define HashType: Category == PrimitiveType with {
	hash: % -> MachineInteger;
#if ALDOC
\aspage{hash}
\Usage{\name~x}
\Signature{\%}{\astype{MachineInteger}}
\Params{ {\em x} & \% & an element of the type\\ }
\Retval{Returns a hash--code for x.}
#endif
}


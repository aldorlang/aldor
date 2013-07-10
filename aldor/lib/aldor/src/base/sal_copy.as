------------------------------- sal_copy.as ----------------------------------
--
-- Types that have a copy function
--
-- Copyright (c) Manuel Bronstein 1999
-- Copyright (c) INRIA 1999, Version 0.1.12
-- Logiciel Salli ©INRIA 1998, dans sa version du 29/10/1998
-----------------------------------------------------------------------------

#include "aldor"

#if ALDOC
\thistype{CopyableType}
\History{Manuel Bronstein}{24/3/99}{created}
\Usage{\this: Category}
\Descr{ \this~is the category of types whose objects can be copied.}
\begin{exports}
\alexp{copy}: & \% $\to$ \% & Make a copy\\
\alexp{copy!}: & (\%, \%) $\to$ \% & In-place copy\\
\end{exports}
#endif

define CopyableType: Category == with {
	copy: % -> %;
	copy!: (%, %) -> %;
#if ALDOC
\alpage{copy}
\altarget{\name!}
\Usage{\name~y\\ \name!(x, y)}
\Signatures{
\name: \% $\to$ \%\\
\name!: (\%, \%) $\to$ \%\\
}
\Params{ {\em x,y} & \% & Element of the type\\ }
\Retval{\name(y) returns a copy of y, while
\name!(x, y) returns a copy of y, where the storage used by x is allowed
to be destroyed or reused, so x is lost after this call.}
\Remarks{Use \name~before making in--place operations on a parameter.
The call \name!(x, y) may cause x to be destroyed, so do not use it unless
x has been locally allocated, and is guaranteed not to share space
with other elements. Some functions are not necessarily copying their
arguments and can thus create memory aliases.}
#endif
	default copy!(a:%, b:%):% == copy b;
}

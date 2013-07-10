---------------------------- sal_ckarray.as ----------------------------------
--
-- This file defines arrays with base-translations.
-- Those arrays are 0-indexed and carry-out bound-checking.
--
-- Copyright (c) Manuel Bronstein 1998
-- Copyright (c) INRIA 1998, Version 29-10-98
-- Logiciel Salli ©INRIA 1998, dans sa version du 29/10/1998
-----------------------------------------------------------------------------

#include "aldor"

macro Z  == MachineInteger;

#if ALDOC
\thistype{CheckingArray}
\History{Manuel Bronstein}{12/04/2000}{created}
\Usage{import from \this~T}
\Params{{\em T} & Type & the type of the array entries\\}
\Descr{\this~provides arrays of entries of type $T$,
$0$-indexed and with bound checking.}
\begin{exports}
\category{\altype{ArrayType}(T, \altype{PrimitiveArray} T)}\\
\end{exports}
\Remarks{The functions \alfunc{LinearStructureType}{apply}
and \alfunc{LinearStructureType}{set!}
throw the exception \altype{ArrayException} when attempting to access
an array out of its bounds.}
#endif

CheckingArray(T:Type): ArrayType(T, PrimitiveArray T) == Array T add {
	apply(x:%, n:Z):T == {
		import from PrimitiveArray T;
		n < 0 or n >= #x => throw ArrayException;
		data(x).n;
	}

	set!(x:%, n:Z, y:T):T == {
		import from PrimitiveArray T;
		n < 0 or n >= #x => throw ArrayException;
		data(x).n := y;
	}
}


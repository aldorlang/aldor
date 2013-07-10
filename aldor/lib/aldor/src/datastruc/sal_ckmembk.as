---------------------------- sal_ckmembk.as ----------------------------------
--
-- This file defines memory blocks, i.e. packed byte-arrays
-- Those arrays are 0-indexed and carry-out bound-checking.
--
-- Copyright (c) Manuel Bronstein 2000
-- Copyright (c) INRIA 1998, Version 29-10-98
-- Logiciel Salli ©INRIA 1998, dans sa version du 29/10/1998
-----------------------------------------------------------------------------

#include "aldor"

macro Z  == MachineInteger;

#if ALDOC
\thistype{CheckingMemoryBlock}
\History{Manuel Bronstein}{24/5/2000}{created}
\Usage{import from \this}
\Descr{\this~provides packed arrays of bytes,
$0$-indexed and with bound checking.}
\begin{exports}
\category{\altype{ArrayType}(\altype{Byte}, \altype{PrimitiveMemoryBlock})}\\
\end{exports}
\Remarks{The functions \alfunc{LinearStructureType}{apply}
and \alfunc{LinearStructureType}{set!}
throw the exception \altype{ArrayException} when attempting to access
a memory block out of its bounds.}
#endif

CheckingMemoryBlock: ArrayType(Byte, PrimitiveMemoryBlock) == MemoryBlock add {
	apply(x:%, n:Z):Byte == {
		import from PrimitiveMemoryBlock;
		n < 0 or n >= #x => throw ArrayException;
		data(x).n;
	}

	set!(x:%, n:Z, y:Byte):Byte == {
		import from PrimitiveMemoryBlock;
		n < 0 or n >= #x => throw ArrayException;
		data(x).n := y;
	}
}


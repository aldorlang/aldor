---------------------------- sal_memblk.as ----------------------------------
--
-- This file defines memory blocks, i.e. packed byte-arrays
-- Those arrays are 0-indexed and do not carry out bound-checking.
--
-- Copyright (c) Manuel Bronstein 2000
-- Copyright (c) INRIA 1998, Version 29-10-98
-- Logiciel Salli ©INRIA 1998, dans sa version du 29/10/1998
-----------------------------------------------------------------------------

#include "aldor"

macro {
	A  == PrimitiveMemoryBlock;
	Z  == MachineInteger;
}

#if ALDOC
\thistype{MemoryBlock}
\History{Manuel Bronstein}{24/5/2000}{created}
\Usage{import from \this}
\Descr{\this~provides packed arrays of bytes,
$0$-indexed and without bound checking (the debug version
of \salli provides bound--checking for memory blocks).}
\begin{exports}
\category{\astype{ArrayType}(\astype{Byte}, \astype{PrimitiveMemoryBlock})}\\
\end{exports}
#endif

MemoryBlock: ArrayType(Byte, A) == add {
	Rep == Record(size:Z, arr: A);
	import from A;

	#(x:%):Z			== { import from Rep; rep(x).size }
	data(x:%):A			== { import from Rep; rep(x).arr }
	array(p:A, n:Z):%		== { import from Rep; per [n, p] }
	empty:%				== { import from Z; array(empty, 0) }
	firstIndex:Z			== 0;

	resize!(x:%, n:Z):% == {
		assert(n >= 0);
		a := data x;
		if n > #x then a := resize!(a, #x, n);
		reset!(x, n, a);
	}

	local reset!(x:%, n:Z, a:A):% == {
		import from Rep;
		rep(x).size := n;
		rep(x).arr := a;
		x;
	}

	copy!(b:%, a:%):% == {
		import from Z;
		q:A := {
			(n := #a) > #b => new n;
			data b;
		}
		p := data a;		-- optimizes code generation
		for i in 0..prev n repeat q.i := p.i;
		reset!(b, n, q);
	}
}


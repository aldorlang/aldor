------------------------------- ald_flags.as ----------------------------------
--
-- Compact 1-indexed blocks of boolean flags
--
-- Copyright (c) Manuel Bronstein 1998
-- Copyright (c) INRIA 1999, Version 0.1.12
-- Logiciel Sum^it (c) INRIA 1999, dans sa version 0.1.12
-----------------------------------------------------------------------------

#include "aldor"

#if ALDOC
\thistype{Flags}
\History{Manuel Bronstein}{12/6/98}{created as Bits in libsumit}
\History{Manuel Bronstein}{28/4/2004}{added PrimitiveArrayType(Boolean) view}
\Usage{import from \this}
\Descr{\this~is a type whose elements are 1-indexed contiguous blocks
of boolean flags. The functionality is similar to using arrays of
\altype{Boolean}, but the implementation is more compact, each flag
using only one machine bit.}
\begin{exports}
\category{\altype{PrimitiveArrayType} \altype{Boolean}}\\
\alexp{dimension}: & (\%, Z) $\to$ Z & number of flags that are set\\
\end{exports}
\begin{alwhere}
Z &==& \altype{MachineInteger}\\
\end{alwhere}
#endif

macro Z == MachineInteger;

Flags: PrimitiveArrayType(Boolean) with {
	dimension: (%, Z) -> Z;
#if ALDOC
\alpage{dimension}
\Signature{(\%, \altype{MachineInteger})}{\altype{MachineInteger}}
\Usage{\name(b, n)}
\Params{
\emph{b} & \% & A block  of flags\\
{\em n} & \altype{MachineInteger} & An index\\
}
\Retval{Returns the number of flags that are set whose index is
between 1 and $n$.}
#endif
} == add {
	Rep == PrimitiveMemoryBlock;
	import from Rep;

	empty:%				== per empty;
	empty?(b:%):Boolean		== empty? rep b;
	free!(b:%):()			== free! rep b;
	firstIndex:Z			== 1;
	pointer(b:%):Pointer		== pointer rep b;
	array(p:Pointer,n:Z):%		== per array(p, bytesize n);
	new(n:Z):%			== per new bytesize n;
	new(n:Z, b?:Boolean):%		== bits(n, { b? => -1; 0 });
	local page(n:Z):Z		== shift(prev n, -3);
	local index(n:Z):Z		== prev(n) /\ 7;
	local bytesize(n:Z):Z		== next page n;
	read(p:BinaryReader, n:Z):%	== per read(p, bytesize n);

	write(p:BinaryWriter, b:%, n:Z):BinaryWriter ==
		write(p, rep b, bytesize n);

	resize!(b:%, oldsz:Z, newsz:Z):% ==
		per resize!(rep b, bytesize oldsz, bytesize newsz);

	local bits(n:Z, m:Z):%	== {
		import from Byte;
		per new(bytesize n, lowByte m);
	}

	bracket(g:Generator Boolean):% == {
		import from Z, List Boolean;
		l:List Boolean := [g];
		a:% := new(n := #l);
		for t in l for i in 1.. repeat a.i := t;
		a;
	}

	bracket(t:Tuple Boolean):% == {
		import from Z;
		zero?(n := length t) => empty;
		a:% := new n;
		for i in 1..n repeat a.i := element(t, i);
		a;
	}

	equal?(a:%, b:%, n:Z):Boolean == {
		import from Byte;
		sz := bytesize n;
		sz > 1 and ~equal?(rep a, rep b, prev sz) => false;
		abyte := rep(a)(prev sz)::Z;
		bbyte := rep(b)(prev sz)::Z;
		for i in 0..index n repeat
			bit?(abyte, i) ~= bit?(bbyte, i) => return false;
		true;
	}

	apply(b:%, n:Z):Boolean	== {
		import from Byte;
		bit?(rep(b)(page n)::Z, index n);
	}

	set!(b:%, n:Z, v?:Boolean):Boolean == {
		import from Byte;
		p := page n;
		a := rep(b)(p)::Z;
		m := index n;
		aa := { v? => set(a, m); clear(a, m); }
		rep(b)(p) := lowByte aa;
		v?
	}

	dimension(b:%, n:Z):Z == {
		m:Z := 0;
		for i in 1..n repeat if b.i then m := next m;
		m;
	}
}

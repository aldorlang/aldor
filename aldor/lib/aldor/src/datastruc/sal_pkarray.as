---------------------------- sal_pkarray.as ----------------------------------
--
-- This file defines packed primitive arrays with base-translations.
-- Those arrays are 0-indexed and do not do bound-checking.
--
-- Copyright (c) Manuel Bronstein 1998
-- Copyright (c) INRIA 1998, Version 29-10-98
-- Logiciel Salli ©INRIA 1998, dans sa version du 29/10/1998
-----------------------------------------------------------------------------

#include "aldor"

macro Z == MachineInteger;

#if ALDOC
\thistype{PackedPrimitiveArray}
\History{Manuel Bronstein}{14/6/2001}{created}
\Usage{import from \this~T}
\Params{{\em T} & \altype{PackableType} & the type of the array entries\\}
\Descr{\this~provides packed arrays of entries of type $T$,
$0$-indexed and without bound checking (the debug version
of \salli provides bound--checking for packed primitive arrays).}
\begin{exports}
\category{\astype{PrimitiveArrayType} T}\\
\end{exports}
#endif

PackedPrimitiveArray(T:PackableType): PrimitiveArrayType T == add {
	import from Machine;
#if DEBUG
	Rep == Record(sz:Z, data:Arr);
	local size(x:%):Z	== { import from Rep; { empty? x => 0; rep(x).sz }}
	local arr(x:Arr, n:Z):%	== { import from Rep; { zero? n => empty; per [n,x]}}
	local data(x:%):Arr == {
		import from Rep;
		empty? x => (nil$Pointer) pretend Arr;
		rep(x).data;
	}
#else
	Rep == Arr;
	local data(x:%):Arr	== rep x;
	local arr(x:Arr, n:Z):%	== per x;
	local arr(x:Arr):%	== per x;
#endif

	array(p:Pointer, n:Z):%		== arr(p pretend Arr, n);
	pointer(a:%):Pointer		== data(a) pretend Pointer;
	empty:%				== (nil$Pointer) pretend %;
	empty?(a:%):Boolean		== nil?(a pretend Pointer)$Pointer;
	firstIndex:Z			== 0;

	free!(x:%):() == {
		import from Boolean;
		if ~empty?(x) then dispose! data x;
	}

	resize!(a:%, oldsz:Z, newsz:Z):% == {
		assert(oldsz >= 0); assert(newsz >= 0);
		newsz <= oldsz => a;
		b := new newsz;
		for i in 0..prev oldsz repeat b.i := a.i;
		free! a;
		b;
	}

	new(n:Z):% == {
		import from T;
		assert(n >= 0);
		AGAT("pkarraynew", n);
		array(newPackedArray n, n);
	}

	new(n:Z, x:T):%	== {
		assert(n >= 0);
		AGAT("pkarraynew", n);
		a := newPackedArray n;
		for i in 0..prev n repeat setPackedArray!(a, i, x);
		array(a, n);
	}

#if DEBUG
	apply(x:%, n:Z):T == {
		assert(n >= 0); assert(n < size x);
		getPackedArray(pointer x, n);
	}

	set!(x:%, n:Z, y:T):T == {
		assert(n >= 0); assert(n < size x);
		setPackedArray!(pointer x, n, y);
		y;
	}
#else
	apply(x:%, n:Z):T	== getPackedArray(pointer x, n);
	set!(x:%, n:Z, y:T):T	== { setPackedArray!(pointer x, n, y); y }
#endif

	bracket(g:Generator T):% == {
		import from Z, List T;
		l:List T := [g];
		a:% := new(n := #l);
		for t in l for i in 0.. repeat a.i := t;
		a;
	}

	bracket(t:Tuple T):% == {
		import from Z;
		zero?(n := length t) => empty;
		a := new n;
		for i in 0..prev n repeat a.i := element(t, next i);
		a;
	}

	if T has PrimitiveType then {
		equal?(a:%, b:%, n:Z):Boolean == {
			import from T;
			assert(0 <= n);
			-- must go by increasing indices to avoid
			-- ArrayException for unequal arrays
			for i in 0..prev n repeat a.i ~= b.i => return false;
			true;
		}
	}
}


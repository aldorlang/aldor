---------------------------- sal_parray.as ----------------------------------
--
-- This file defines primitive arrays with base-translations.
-- Those arrays are 0-indexed and do not do bound-checking.
--
-- Copyright (c) Manuel Bronstein 1998
-- Copyright (c) INRIA 1998, Version 29-10-98
-- Logiciel Salli (c) INRIA 1998, dans sa version du 29/10/1998
-----------------------------------------------------------------------------

#include "aldor"

macro Z == MachineInteger;

#if ALDOC
\thistype{PrimitiveArray}
\History{Manuel Bronstein}{28/9/98}{created}
\Usage{import from \this~T}
\Params{{\em T} & Type & the type of the array entries\\}
\Descr{\this~provides arrays of entries of type $T$,
$0$-indexed and without bound checking (the debug version
of \salli provides bound--checking for primitive arrays).}
\begin{exports}
\category{\altype{PrimitiveArrayType} T}\\
\end{exports}
#endif

PrimitiveArray(T:Type): PrimitiveArrayType T == add {
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

	-- TEMPORARY: CONDITIONAL CODE STOPS IN-LINING IN CLIENTS (BUG1315)
--	if T has PackableType then {
--		new(n:Z):% == {
--			import from T;
--			assert(n >= 0);
--			AGAT("parraynew", n);
--			array(newPackedArray n, n);
--		}
--
--		new(n:Z, x:T):%	== {
--			assert(n >= 0);
--			AGAT("parraynew", n);
--			a := newPackedArray n;
--			for i in 0..prev n repeat setPackedArray!(a, i, x);
--			array(a, n);
--		}
--	}
--	else {
		new(n:Z):% == {
			assert(n >= 0);
			AGAT("parraynew", n);
			arr(array(Z)(0, n::SInt), n);
		}

		new(n:Z, x:T):%	== {
			assert(n >= 0);
			AGAT("parraynew", n);
			a := array(T)(x, n::SInt);
			for i in 0..prev n repeat set!(T)(a, i::SInt, x);
			arr(a, n);
		}
--	}

#if DEBUG
	-- TEMPORARY: CONDITIONAL CODE STOPS IN-LINING IN CLIENTS (BUG1315)
--	if T has PackableType then {
--		apply(x:%, n:Z):T == {
--			assert(n >= 0); assert(n < size x);
--			getPackedArray(pointer x, n);
--		}
--
--		set!(x:%, n:Z, y:T):T == {
--			assert(n >= 0); assert(n < size x);
--			setPackedArray!(pointer x, n, y);
--			y;
--		}
--	}
--	else {
		apply(x:%, n:Z):T == {
			assert(n >= 0); assert(n < size x);
			get(T)(data x, n::SInt);
		}

		set!(x:%, n:Z, y:T):T == {
			assert(n >= 0); assert(n < size x);
			set!(T)(data x, n::SInt, y);
		}
--	}
#else
	-- TEMPORARY: CONDITIONAL CODE STOPS IN-LINING IN CLIENTS (BUG1315)
--	if T has PackableType then {
--		apply(x:%, n:Z):T	== getPackedArray(pointer x, n);
--		set!(x:%, n:Z, y:T):T	== { setPackedArray!(pointer x,n,y); y }
--	}
--	else {
		-- TEMPORARY: DOES NOT OPTIMIZE IF 2 FUNCTIONS (BUG1182)
		-- local elt:(Arr, SInt) -> T	== get T;
		-- apply(x:%, n:Z):T		== elt(data x, n::SInt);
		apply(x:%, n:Z):T		== get(T)(data x, n::SInt);
		set!(x:%, n:Z, y:T):T		== set!(T)(data x, n::SInt, y);
--	}
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

#if ALDOC
\thistype{PrimitiveArrayType}
\History{Manuel Bronstein}{25/5/2000}{created}
\Usage{import from \this~T}
\Params{{\em T} & Type & the type of the array entries\\}
\Descr{\this~is the category of primitive arrays whose entries are of type $T$.}
\begin{exports}
\category{\altype{FiniteLinearStructureType} T}\\
\alexp{array}: & (\altype{Pointer}, Z) $\to$ \% &
conversion from a C--array\\
\alexp{new}: & Z $\to$ \% & creation of an array\\
\alexp{pointer}: & \% $\to$ \altype{Pointer} &
conversion to a C--array\\
\alexp{resize!}: & (\%, Z, Z) $\to$ \% & resize an array\\
\alexp{sort!}:
& (\%, Z, Z, (T, T) $\to$ \altype{Boolean}) $\to$ \% & sort an array\\
\end{exports}
\begin{exports}[if $T$ has \altype{SerializableType} then]
\alexp{read}:
& (\altype{BinaryReader}, Z) $\to$ \% & read using binary encoding\\
\alexp{write}:
& (\altype{BinaryWriter}, \%, Z) $\to$ \altype{BinaryWriter} &
write using binary encoding\\
\end{exports}
\begin{exports}[if $T$ has \altype{TotallyOrderedType} then]
\alexp{sort!}: & (\%, Z, Z) $\to$ \% & sort an array\\
\end{exports}
\begin{aswhere}
Z &==& \altype{MachineInteger}\\
\end{aswhere}
#endif

define PrimitiveArrayType(T:Type):Category == FiniteLinearStructureType T with {
	array: (Pointer, Z) -> %;
	pointer: % -> Pointer;
#if ALDOC
\alpage{array,pointer}
\altarget{array}
\altarget{pointer}
\Usage{array(p, n)\\ pointer~a}
\Signatures{
array: & (\altype{Pointer}, \altype{MachineInteger}) $\to$ \%\\
pointer: & \% $\to$ \altype{Pointer}\\
}
\Params{
{\em p} & \altype{Pointer} & a C--array\\
{\em n} & \altype{MachineInteger} & a nonnegative size\\
{\em a} & \% & A primitive array\\
}
\Descr{Use those functions to safely convert between pointers
returned or needed by C--functions and primitive arrays.
Those function have no effect
in the release version of \salli, but they are
necessary when using the debug version, so it is recommended to use
them in all cases.}
#endif
	new: Z -> %;
#if ALDOC
\alpage{new}
\Usage{\name~n}
\Signature{\altype{MachineInteger}}{\%}
\Params{ {\em n} & \altype{MachineInteger} & a nonnegative size\\ }
\Retval{Returns a primitive array of $n$ uninitialized entries.}
#endif
	if T has SerializableType then {
		read: (BinaryReader, Z) -> %;
		write: (BinaryWriter, %, Z) -> BinaryWriter;
#if ALDOC
\alpage{read,write}
\altarget{read}
\altarget{write}
\Usage{read(in, n)\\ write(out, a, n)}
\Signatures{
read: & (\altype{BinaryReader}, \altype{MachineInteger}) $\to$ \%\\
write:
& (\altype{BinaryWriter}, \%, \altype{MachineInteger})
$\to$ \altype{BinaryWriter}\\
}
\Params{
{\em in} & \altype{BinaryReader} & an input stream\\
{\em out} & \altype{BinaryWriter} & an output stream\\
{\em n} & \altype{MachineInteger} & a nonnegative size\\
{\em a} & \% & A primitive array\\
}
\Retval{read(in, n) reads a primitive array of $n$ elements
in binary format from the stream in and
returns the array read, while
write(out, a, n) writes the first $n$ elements of $a$ to the stream out
and returns the stream after the write.}
#endif
	}
	resize!: (%, Z, Z) -> %;
#if ALDOC
\alpage{resize!}
\Usage{\name(a, n, m)}
\Signature{(\%,\altype{MachineInteger},\altype{MachineInteger})}{\%}
\Params{
{\em a} & \% & a primitive array\\
{\em n, m} & \altype{MachineInteger} & nonnegative sizes\\
}
\Retval{Returns a primitive array of $m$ entries, whose first $n$ entries
are the first $n$ entries of $a$ and whose remaining entries are
uninitialized.}
\Remarks{\name~may free the space previously used by $a$, so
it is unsafe to use the variable $a$
after the call, unless it has been assigned to the result
of the call, as in {\tt a := resize!(a, n, m)}.}
#endif
	sort!: (%, Z, Z, (T, T) -> Boolean) -> %;
	if T has TotallyOrderedType then {
		sort!: (%, Z, Z) -> %;
	}
#if ALDOC
\alpage{sort!}
\Usage{\name(a, n, m)\\ \name(a, n, m, f)}
\Signature{(\%,\altype{MachineInteger},\altype{MachineInteger},
(T,T) $\to$ \altype{Boolean})}{\%}
\Params{
{\em a} & \% & a primitive array\\
{\em n, m} & \altype{MachineInteger} & indices\\
{\em f} & (T, T) $\to$ \altype{Boolean} & a comparison function\\
}
\Descr{Sorts the subarray $[a.n,\dots,a.m]$ using the ordering
$x < y \iff f(x,y)$. The comparison function $f$ is optional if
$T$ has \altype{TotallyOrderedType}, in which case the order
function of $T$ is taken.}
#endif
	default {
		if T has SerializableType then {
			read(p:BinaryReader, n:Z):% == {
				import from T;
				assert(n >= 0);
				zero? n => empty;
				f := firstIndex;
				a := new n;
				for m in 0..prev n repeat a(m+f) := << p;
				a;
			}

			write(p:BinaryWriter, a:%, n:Z):BinaryWriter == {
				import from T;
				assert(n >= 0);
				f := firstIndex;
				for m in 0..prev n repeat p := p << a(m+f);
				p;
			}
		}

		if T has TotallyOrderedType then {
			sort!(a:%, n:Z, m:Z):% == sort!(a, n, m, <$T);
		}

		sort!(a:%, n:Z, m:Z, less:(T, T) -> Boolean):% == {
			assert(n >= 0); assert(m >= 0);
			(s := m - n) <= 0 => a;
			s = 1 => {
				less(a.n, a.m) => a;
				swap!(a, n, m);
			}
			k := partition(a, n, m, less);
			sort!(a, n, k, less);
			sort!(a, next k, m, less);
		}

		local swap!(a:%, n:Z, m:Z):% == {
			t := a.n;
			a.n := a.m;
			a.m := t;
			a;
		}

		local partition(a:%, n:Z, m:Z, less:(T, T) -> Boolean):Z == {
			import from Boolean;
			assert(m > next n);
			r := n + (random()$Z mod next(m-n));
			swap!(a, r, n);		-- randomize the array
			x := a.n;		-- (random) pivot
			i := next n;
			j := m;
			while i < j repeat {
				while i<= m and ~less(x,a.i) repeat i := next i;
				if i > m then {		-- x = max(a[n..m])
					swap!(a, n, m);
					return prev m;
				}
				while j > n and ~less(a.j,x) repeat j := prev j;
				if i < j then {
					swap!(a, i, j);
					i := next i;
					j := prev j;
				}
			}
			j;
		}
	}
}


#if ALDORTEST
#include "aldor"

test(): () == {
    import from Assert MachineInteger;
    import from MachineInteger;
    arr: PrimitiveArray MachineInteger := new(2, 3);
    assertEquals(3, arr(0));
    arr(1) := 1;
    assertEquals(1, arr(1));

    arr := [1,2,3];
    assertEquals(1, arr(0));
    assertEquals(2, arr(1));
    assertEquals(3, arr(2));
}

test()
#endif

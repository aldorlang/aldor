---------------------------- sal_array.as ----------------------------------
--
-- This file defines arrays with base-translations.
-- Those arrays are 0-indexed and do not carry out bound-checking.
--
-- Copyright (c) Manuel Bronstein 1998
-- Copyright (c) INRIA 1998, Version 29-10-98
-- Logiciel Salli �INRIA 1998, dans sa version du 29/10/1998
-----------------------------------------------------------------------------

#include "aldor"

macro {
	A  == PrimitiveArray;
	Z  == MachineInteger;
	Ch == Character;
}

#if ALDOC
\thistype{Array}
\History{Manuel Bronstein}{7/10/98}{created}
\History{Manuel Bronstein}{21/10/99}{added exceptions}
\Usage{import from \this~T}
\Params{{\em T} & Type & the type of the array entries\\}
\Descr{\this~provides arrays of entries of type $T$,
$0$-indexed and without bound checking.}
\begin{exports}
\category{\altype{ArrayType}(T, \altype{PrimitiveArray} T)}\\
\end{exports}
#endif

Array(T:Type): ArrayType(T, A T) == add {
	Rep == Record(size:Z, arr: A T);
	import from A T;

	#(x:%):Z			== { import from Rep; rep(x).size }
	data(x:%):A T			== { import from Rep; rep(x).arr }
	array(p:A T, n:Z):%		== { import from Rep; per [n, p] }
	firstIndex:Z			== 0;
	empty:%				== { import from Z; array(empty, 0) }

	apply(x:%, n:Z):T == {
		assert(n >= 0 and n < #x);
		data(x).n;
	}
	set!(x:%, n:Z, y:T):T == {
		assert(n >= 0 and n < #x);
		data(x).n := y;
	}

	resize!(x:%, n:Z):% == {
		assert(n >= 0);
		a := data x;
		if n > #x then a := resize!(a, #x, n);
		reset!(x, n, a);
	}

	local reset!(x:%, n:Z, a:A T):% == {
		import from Rep;
		rep(x).size := n;
		rep(x).arr := a;
		x;
	}

	copy!(b:%, a:%):% == {
		import from Z;
		q:A T := {
			(n := #a) > #b => new n;
			data b;
		}
		p := data a;		-- optimizes code generation
		for i in 0..prev n repeat q.i := p.i;
		reset!(b, n, q);
	}
}

#if ALDOC
\thistype{ArrayException}
\History{Manuel Bronstein}{21/10/99}{created}
\Usage{
throw \this\\
try \dots catch E in \{ E has \altype{ArrayExceptionType} $=>$ \dots \}
}
\Descr{\this~is an exception type thrown by array access.}
#endif
ArrayException: ArrayExceptionType == add;

#if ALDOC
\thistype{ArrayExceptionType}
\History{Manuel Bronstein}{21/10/99}{created}
\Usage{\this: Category}
\Descr{\this~is the category of exceptions thrown by array access.}
#endif
define ArrayExceptionType:Category == with;

#if ALDOC
\thistype{ArrayType}
\History{Manuel Bronstein}{12/04/2000}{created}
\Usage{\this(T, P): Category}
\Params{
{\em T} & Type & the type of the array entries\\
{\em P} & \altype{PrimitiveArrayType} T & the type of the underlying data\\
}
\Descr{\this~is the category of arrays whose entries are of type $T$ and
whose underlying data is of type $P$.}
\begin{exports}
\category{\altype{BoundedFiniteLinearStructureType} T}\\
\alexp{array}: & (P, \altype{MachineInteger}) $\to$ \% &
construction of an array\\
\alexp{data}: & \% $\to$ P & access to raw data\\
\alexp{new}: & \altype{MachineInteger} $\to$ \% & creation of an array\\
\alexp{resize!}: & (\%, \altype{MachineInteger}) $\to$ \% & resize an array\\
\alexp{sort!}: & (\%, (T, T) $\to$ \altype{Boolean}) $\to$ \% & sort an array\\
\end{exports}
\begin{exports}[if $T$ has \altype{TotallyOrderedType} then]
\category{\altype{TotallyOrderedType}}\\
\alexp{binarySearch}:
& (T, \%) $\to$ (\altype{Boolean}, \altype{MachineInteger}) & binary search\\
\alexp{sort!}: & \% $\to$ \% & sort an array\\
\end{exports}
#endif

define ArrayType(T:Type, PT:PrimitiveArrayType T):
	Category == BoundedFiniteLinearStructureType T with {
	array: (PT, Z) -> %;
#if ALDOC
\alpage{array}
\Usage{\name(p, n)}
\Signature{(P, \altype{MachineInteger})}{\%}
\Params{
{\em p} & P & a primitive array structure\\
{\em n} & \altype{MachineInteger} & a number of elements\\
}
\Retval{Returns an array containing the first n entries of p.
No copying is made.}
#endif
	if T has TotallyOrderedType then {
		TotallyOrderedType;
		binarySearch: (T, %) -> (Boolean, Z);
#if ALDOC
\alpage{binarySearch}
\Usage{\name(t, a)}
\Signature{(T, \%)}{(\altype{Boolean}, \altype{MachineInteger})}
\Params{
{\em t} & T & the value to search for\\
{\em a} & \% & an array\\
}
\Retval{Returns (found?, i) such that $0 \le i < \#a$ and
$t = a.i$ if found?~is \true. Otherwise, found?~is \false~and:
\begin{itemize}
\item ~if $i < 0$ then $t < a.0$;
\item ~if $i \ge \#a - 1$ then $t > a(\#a-1)$;
\item ~if $0 \le i < \#a - 1$, then $a.i < t < a(i+1)$.
\end{itemize}
The array a must be sorted in increasing order. If a is sorted with respect
to a different order, it is still possible to use binary search, but from
\altype{BinarySearch}.
}
\alseealso{\alfunc{BoundedFiniteLinearStructureType}{linearSearch}}
#endif
	}
	data: % -> PT;
#if ALDOC
\alpage{data}
\Usage{\name~a}
\Signature{\%}{P}
\Params{ {\em a} & \% & an array\\ }
\Retval{Returns the raw data of the array a. No copying is made. This function
can be used for efficiency before accessing the elements of a inside a loop,
or in order to pass the elements of a to a C function.}
#endif
	new: Z -> %;
#if ALDOC
\alpage{new}
\Usage{\name~n}
\Signature{\altype{MachineInteger}}{\%}
\Params{ {\em n} & \altype{MachineInteger} & a nonnegative size\\ }
\Retval{Returns an array of $n$ uninitialized entries.}
#endif
	resize!: (%, Z) -> %;
#if ALDOC
\alpage{resize!}
\Usage{\name(a, n)}
\Signature{(\%,\altype{MachineInteger})}{\%}
\Params{
{\em a} & \% & an array\\
{\em n} & \altype{MachineInteger} & a nonnegative size\\
}
\Retval{Returns an array of $n$ entries, whose first $\#a$ entries
are the first $\#a$ entries of $a$ and whose remaining entries are
uninitialized.}
#endif
	sort!: (%, (T, T) -> Boolean) -> %;
	if T has TotallyOrderedType then {
		sort!: % -> %;
	}
#if ALDOC
\alpage{sort!}
\Usage{\name~a\\ \name(a, f)}
\Signature{(\%,(T,T) $\to$ \altype{Boolean})}{\%}
\Params{
{\em a} & \% & a primitive array\\
{\em f} & (T, T) $\to$ \altype{Boolean} & a comparison function\\
}
\Descr{Sorts the array $a$ using the ordering
$x < y \iff f(x,y)$. The comparison function $f$ is optional if
$T$ has \altype{TotallyOrderedType}, in which case the order
function of $T$ is taken.}
#endif
	if T has PrimitiveType then {
		linearReverseSearch: (T, %) -> (Boolean, Z, T);
		linearReverseSearch: (T, %, Z) -> (Boolean, Z, T);
	}
	default {
		import from PT;

		local comma:Ch			== { import from Z; char 44; }
		local leftBracket:Ch		== { import from Z; char 91; }
		local rightBracket:Ch		== { import from Z; char 93; }
		empty?(a:%):Boolean		== { import from Z; zero? #a; }
		free!(x:%):()			== free! data x;
		new(n:Z):%			== array(new n, n);
		new(n:Z, x:T):%			== array(new(n, x), n);
		bracket(t:Tuple T):%		== array(bracket t, length t);

		sort!(a:%, f:(T, T) -> Boolean):% == {
			import from Z;
			zero?(n := #a) => empty;
			sort!(data a, 0, prev n, f);
			a;
		}

		map!(f:T -> T)(a:%):% == {
			import from Z;
			zero?(n := #a) => empty;
			p := data a;		-- optimizes code generation
			for i in 0..prev n repeat p.i := f(p.i);
			a;
		}

		map(f:T -> T)(a:%):% == {
			import from Z;
			zero?(n := #a) => empty;
			b:% := new n;
			p := data a;		-- optimizes code generation
			q := data b;		-- optimizes code generation
			for i in 0..prev n repeat q.i := f(p.i);
			b;
		}

		copy(a:%):% == {
			import from Z;
			zero?(n := #a) => empty;
			b:% := new n;
			p := data a;		-- optimizes code generation
			q := data b;		-- optimizes code generation
			for i in 0..prev n repeat q.i := p.i;
			b;
		}

		generator(a:%):Generator T == generate {
			import from Z;
			p := data a;		-- optimizes code generation
			for i in 0..prev(#a) repeat yield(p.i);
		}

		bracket(g:Generator T):% == {
			import from Z, List T;
			l:List T := [g];
			a:% := new(n := #l);
			p := data a;		-- optimizes code generation
			for t in l for i in 0.. repeat p.i := t;
			a;
		}

		if T has PrimitiveType then {
			linearSearch(t:T, a:%, start:Z):(Boolean, Z, T) == {
				assert(start >= firstIndex);
				p := data a;	-- optimizes code generation
				for n in start-firstIndex..prev(#a) repeat {
					p.n=t => return(true,n+firstIndex,p.n);
				}
				(false, prev firstIndex, t);
			}
			linearReverseSearch(t:T, a:%):(Boolean, Z, T) == {
				p := data a;	-- optimizes code generation
				for n in prev(#a) + firstIndex..firstIndex by -1 repeat {
					p.n=t => return(true,n,p.n);
				}
				(false, prev firstIndex, t);
			}

			linearReverseSearch(t:T, a:%, end:Z):(Boolean, Z, T) == {
				assert(end >= firstIndex);
				p := data a;	-- optimizes code generation
				for n in end..firstIndex by -1 repeat {
					p.n=t => return(true,n,p.n);
				}
				(false, prev firstIndex, t);
			}

			(a:%) = (b:%):Boolean == {
				import from Z, T;
				(n := #a) ~= #b => false;
				p := data a;	-- optimizes code generation
				q := data b;	-- optimizes code generation
				for i in 0..prev n repeat {
					p.i ~= q.i => return false;
				}
				true;
			}
		}

		apply(x:%, n:Z):T == {
			assert(n >= 0 and n < #x);
			data(x).n;
		}

		set!(x:%, n:Z, y:T):T == {
			assert(n >= 0 and n < #x);
			data(x).n := y;
		}

		if T has OutputType then {
			(p:TextWriter) << (a:%):TextWriter == {
				import from Ch, Boolean, Z, T;
				n := #a;
				x := data a;	-- optimizes code generation
				p := p << leftBracket;
				if ~zero?(n) then p := p << x.0;
				for i in 1..prev n repeat p:= p << comma << x.i;
				p << rightBracket;
			}
		}

		if T has InputType then {
			<< (p:TextReader):% == {
				import from Z, List T;
				l:List T := << p;
				empty? l => empty;
				a:% := new(#l, first l);
				for x in l for n in 0.. repeat a.n := x;
				free! l;
				a;
			}
		}

		if T has SerializableType then {
			<< (p:BinaryReader):% == {
				import from Z, T;
				n:Z := << p;		-- read size first
				zero? n => empty;
				array(read(p, n), n);
			}
		}

		if T has TotallyOrderedType then {
			sort!(a:%):% == sort!(a, <$T);

			binarySearch(t:T, a:%):(Boolean, Z) == {
				import from BinarySearch(Z, T);
				p := data a;	-- optimizes code generation
				binarySearch(t, (i:Z):T +-> p.i, 0, prev(#a));
			}

			(a:%) < (b:%):Boolean ==  {
				import from Z, T;
				for x in a for y in b repeat {
					x ~= y => return(x < y);
				}
				#a < #b;
			}
		}
	}
}

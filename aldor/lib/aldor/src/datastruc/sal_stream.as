---------------------------- sal_stream.as ----------------------------------
--
-- This file defines streams with base-translations.
-- Those streams are 0-indexed and carry-out bound-checking.
--
-- Copyright (c) Manuel Bronstein 2000
-- Copyright (c) INRIA 2000, Version 29-10-98
-- Logiciel Salli ©INRIA 2000, dans sa version du 29/10/1998
-----------------------------------------------------------------------------

#include "aldor"

macro {
	A  == PrimitiveArray;
	Z  == MachineInteger;
}

#if ALDOC
\thistype{Stream}
\History{Manuel Bronstein}{28/3/2000}{created}
\History{Manuel Bronstein}{30/1/2003}{added interlacing}
\Usage{import from \this~T}
\Params{{\em T} & Type & the type of the stream entries\\}
\Descr{\this~provides streams of entries of type $T$,
$0$-indexed and with bound checking.}
\begin{exports}
\category{\altype{LinearStructureType} T}\\
\alalias{\this}{size}{\#}: & \% $\to$ Z & number of computed elements\\
\alexp{constant}:
& \% $\to$ (Z, \altype{Partial} T) & check for an eventually constant stream\\
\alexp{generator} : & \% $\to$ \altype{Generator} T & iteration over a stream\\
\alexp{interlacing}: & \altype{List} \% $\to$ \% & interlacing\\
\alexp{map!}: & (T $\to$ T) $\to$ \% $\to$ \% & lift a mapping\\
\alexp{orbit}: & (T, T $\to$ T) $\to$ \% & creation of a stream\\
\alexp{stream}: & T $\to$ \% & creation of a stream\\
& (() $\to$ T) $\to$ \% & \\
& ((Z, T) $\to$ T) $\to$ \% \\
& (Z, Z $\to$ T) $\to$ \% & \\
& (Z, Z $\to$ T, Z, T) $\to$ \% & \\
& (\altype{Generator} T, T) $\to$ \% \\
& (\altype{Generator} T, Z, T) $\to$ \% \\
\end{exports}
\begin{alwhere}
Z &==& \altype{MachineInteger}\\
\end{alwhere}
\begin{exports}[if $T$ has \altype{OutputType} then]
\category{\altype{OutputType}}\\
\end{exports}
#endif

Stream(T:Type): LinearStructureType T with {
	if T has OutputType then OutputType;
	#: % -> Z;
#if ALDOC
\alpage{size}
\Usage{\#~s}
\Signatures{ \#: & \% $\to$ \altype{MachineInteger}\\ }
\Params{ {\em s} & \% & a stream\\ }
\Retval{Returns the number of elements of $s$ that have been computed.}
#endif
	constant: % -> (Z, Partial T);
#if ALDOC
\alpage{constant}
\Usage{\name~s}
\Signature{\%}{(\altype{MachineInteger}, \altype{Partial} T)}
\Params{ {\em s} & \% & a stream\\ }
\Retval{Returns $(-1, \failed)$ if $s$ is not known to be eventually constant,
otherwise returns $(n \ge 0, [t])$ such that $s.k = s.n = t$ for $k \ge n$.}
#endif
	generator: % -> Generator T;
#if ALDOC
\alpage{generator}
\Usage{ for x in s repeat \{ \dots \}\\ for x in \name~s repeat \{ \dots \} }
\Signature{\%}{\altype{Generator} T}
\Params{ {\em s} & \% & a stream\\ }
\Descr{This function allows the elements of a stream to be iterated.}
\Remarks{Since those generators are infinite, you should have
a termination condition either inside the loop or in parallel with
the generator in order to guarantee termination.}
\begin{asex}
The following code creates the stream of all the squares for $n \ge 0$ and
prints those of them that are smaller than $1000$:
\begin{ttyout}
	import from MachineInteger, Stream MachineInteger;
	s := stream(0, (n:MachineInteger):MachineInteger +-> n^2);
	for x in s while x < 1000 repeat { stdout << x << newline; }
\end{ttyout}
\end{asex}
#endif
	interlacing: List % -> %;
#if ALDOC
\alpage{interlacing}
\Usage{\name~[$s_1,\dots,s_n$]}
\Signature{\altype{List} \%}{\%}
\Params{ $s_i$ & \% & streams\\ }
\Descr{Given $s_i = t_{i1},t_{i2},\dots$, returns the stream
$t_{11},t_{21},\dots,t_{n1},t_{12},t_{22},\dots,t_{n2},\dots$.}
#endif
	map!: (T -> T) -> % -> %;
#if ALDOC
\alpage{map!}
\Usage{\name~f\\ \name(f)(s)}
\Signature{(T $\to$ T) $\to$ \%}{\%}
\Params{
{\em f} & T $\to$ T & a map\\
{\em s} & \% & a stream\\
}
\Retval{\name(f)(s) returns the stream {\tt [f(x) for x in s]}, while
\name(f) returns the mapping $s \to$ {\tt [f(x) for x in s]}. In both cases,
the stream s is modified in place and no copy is made.}
#endif
	orbit: (T, T -> T) -> %;
#if ALDOC
\alpage{orbit}
\Usage{\name(t, f)}
\Signature{(T, T $\to$ T)}{\%}
\Params{
{\em t} & T & an element of T\\
{\em f} & T $\to$ T & a function\\
}
\Retval{Returns the stream $[t, f(t), f^2(t), \dots]$.}
\alseealso{\alexp{stream}}
#endif
	stream: T -> %;
	stream: (() -> T) -> %;
	stream: (Z, Z -> T) -> %;
	stream: ((Z, %) -> T) -> %;
	stream: (Z, Z -> T, Z, T) -> %;
	stream: (Generator T, T) -> %;
	stream: (Generator T, Z, T) -> %;
#if ALDOC
\alpage{stream}
\Usage{\name~t\\ \name~f\\ \name~h\\ \name(n, g)\\ \name(n, g, m, t)\\
\name(gen, t)\\ \name(gen, m, t)}
\Signatures{
\name: & T $\to$ \% \\
\name: & (() $\to$ T) $\to$ \% \\
\name: & ((\altype{MachineInteger}, \%) $\to$ T) $\to$ \% \\
\name: & (\altype{MachineInteger}, \altype{MachineInteger} $\to$ T) $\to$ \% \\
\name: & (\altype{MachineInteger}, \altype{MachineInteger} $\to$ T,
\altype{MachineInteger}, T) $\to$ \% \\
\name: & (\altype{Generator} T, T) $\to$ \% \\
\name: & (\altype{Generator} T, \altype{MachineInteger}, T) $\to$ \% \\
}
\Params{
{\em n, m} & \altype{MachineInteger} & machine integers\\
{\em t} & T & an element\\
{\em f} & () $\to$ T & a function\\
{\em g} & \altype{MachineInteger} $\to$ T & a function\\
{\em h} & (\altype{MachineInteger}, \%) $\to$ T & a function\\
{\em gen} & \altype{Generator} T & a generator\\
}
\Descr{\name(t) returns the constant stream $[t,t,t,\dots]$,
while \name(f) returns the stream obtained by successive calls to $f()$,
\name(n, g) returns the stream $[g(n),g(n+1),g(n+2),\dots]$,
\name(n, g, m, t) returns the eventually constant stream
$[g(n),g(n+1),g(n+2),\dots,g(m-1),t,t,t,\dots]$
and \name(h) returns the stream $[s_0,s_1,s_2,\dots]$ where
$s_k = h(k, s_0, \dots s_{i-k})$ for any $k \ge 0$.
When using generators, \name(gen, t) returns the stream
$[x \mbox{ for } x \mbox{ in } gen]$ followed by $[t,t,t,\dots]$
if $gen$ is finite, while \name(gen, m, t) returns the first
$m$ elements of the stream $[x \mbox{ for } x \mbox{ in } gen]$ followed
by $[t,t,t,\dots]$.}
\alseealso{\alexp{orbit}}
\Remarks{The function $g$ must be defined for all $n\le k$ (resp.~$n\le k < m$)
and $h$ must be defined for all $n \ge 0$, even if you intend to use
\alfunc{LinearStructureType}{set!} to set specific values of the
stream later. Note that
\name(f) does not necessarily returns a constant stream since
each call to $f()$ can side--effect its environment.
Constant or eventually constant streams do not repeatedly store
their constant value and are therefore preferable. For example,
${\tt stream(0)}$ stores only one value no matter how many values
are requested, while ${\tt stream(():MachineInteger +-> 0)}$ stores
$n$ times $0$ when its $\sth{n}$ element is requested.}
\begin{asex}
The following code creates the stream of the Fibonnaci numbers and prints
the first 10 of them:
\begin{ttyout}
	import from MachineInteger, AldorInteger, Stream AldorInteger;
	fib(n:MachineInteger, f:Stream AldorInteger):AldorInteger == {
		n = 0 or n = 1 => 1;
		f(n-1) + f(n-2);
	}
	sfib := stream fib;
	a := sfib.9;	-- computes sfib.0 to sfib.9
	stdout << a << newline;
\end{ttyout}
\end{asex}
#endif
} == add {
	-- ncnst = value is constant for index >= ncnst >= 0
        --         value is not constant if ncnst < 0
	-- elts = number of elements computed (arr.0 to arr(elts-1))
	-- szarr = allocated size of arr
	-- arr = actual data
	-- calling fun() computes the next element
	Rep == Record(ncnst:Z,cval:Partial T,elts:Z,szarr:Z,arr:A T,fun:()->T);

	firstIndex:Z		== 0;
	local func(s:%):() -> T	== { import from Rep; rep(s).fun; }
	#(s:%):Z		== { import from Rep; rep(s).elts; }
	local data(s:%):A T	== { import from Rep; rep(s).arr; }
	local size(s:%):Z	== { import from Rep; rep(s).szarr; }
	local constFrom(s:%):Z	== { import from Rep; rep(s).ncnst; }
	empty?(s:%):Boolean	== false;
	stream(n:Z, f:Z -> T):%	== stream iter(f, n);
	stream(t:T):%		== stream(0, (n:Z):T +-> t, 0, t);
	bracket(g:Generator T):%== stream(():T +-> next! g);
	stream(g:Generator T, t:T):%	== { import from Z; streamgen(g,-1,t) }
	local setFunc!(s:%, f:()->T):%	== { import from Rep; rep(s).fun:=f;s }
	local constVal(s:%):Partial T	== { import from Rep; rep(s).cval; }
	constant(s:%):(Z, Partial T)	== (constFrom s, constVal s);

	-- m = max number of terms to compute from g
	stream(g:Generator T, m:Z, t:T):% == {
		assert(m >= 0);
		zero? m => stream t;
		setConst!(streamgen(g, m, t), m, t);
	}

	-- m = max number of terms to compute from g or -1 (forever)
	local streamgen(g:Generator T, m:Z, t:T):% == {
		s := stream(nil$Pointer pretend (() -> T));
		h := compose(s, g, m, t);
		setFunc!(s, ():T +-> next! h);
	}

	-- m = max number of terms to compute from g or -1 (forever)
	local compose(s:%, g:Generator T, m:Z, t:T):Generator T == generate {
		n:Z := 0;
		for x in g while (m < 0 or n < m) repeat {
			n := next n;
			yield x;
		}
		setConst!(s, n, t);
		repeat yield t;
	}

	local setConst!(s:%, n:Z, t:T):% == {
		import from Rep;
		rep(s).cval := [t];
		rep(s).ncnst := n;
		s;
	}

	interlacing(l:List %):% == {
		import from Generator T, List Generator T;
		assert(~empty? l);
		[interlacing [generator s for s in l]];
	}

	map!(f:T -> T):% -> % == {
		(s:%):% +-> {
			import from Boolean, Z, Partial T, A T;
			n := prev(#s);
			a := data s;
			for i in 0..n repeat a.i := f(a.i);
			u := constVal s;
			if ~failed?(u) then
				setConst!(s, constFrom s, f retract u);
			g := func s;
			setFunc!(s, ():T +-> f g());
		}
	}

	stream(f:() -> T):% == {
		import from A T, Partial T;
		stream(empty, -1, failed, 0, 0, f);
	}

	stream(n:Z, f:Z -> T, m:Z, t:T):% == {
		import from Partial T, A T;
		stream(empty, max(m - n, 0), [t], 0, 0, iter(f, n, m, t));
	}

	stream(f:(Z, %) -> T):%	== {
		s := stream(nil$Pointer pretend (() -> T));
		setFunc!(s, iter(f, 0, s));
	}

	local stream(a:A T, n:Z, t:Partial T, e:Z, s:Z, f:() -> T):% == {
		import from Rep;
		per [n, t, e, s, a, f];
	}

	local iter(f:T -> T, a:T):() -> T == {
		b := a;
		():T +-> { free b := f b; b }
	}

	local iter(f:Z -> T, n:Z):() -> T == {
		m := n;
		():T +-> { a := f m; free m := next m; a }
	}

	local iter(f:Z -> T, n:Z, m:Z, t:T):() -> T == {
		p := n;
		():T +-> { a := { p >= m => t; f p }; free p := next p; a }
	}

	local iter(f:(Z, %) -> T, n:Z, s:%):() -> T == {
		m := n;
		():T +-> { a := f(m, s); free m := next m; a }
	}

	orbit(t:T, f:T -> T):%	== {
		import from Z, A T, Partial T;
		a := new 1;
		a.0 := t;
		stream(a, -1, failed, 1, 1, iter(f, t));
	}

	generator(s:%):Generator T == generate {
		import from Z;
		for i in 0.. repeat yield s.i;
	}

	-- want to be able to access s.k after resize
	local resize!(s:%, k:Z):() == {
		import from Rep, A T;
		if (sz := size s) <= k then {
			a := data s;
			-- make sure that a is at least doubled
			m := max(next k, sz + sz);
			a := resize!(a, sz, m);
			rep(s).arr := a;
			rep(s).szarr := m;
		}
	}

	apply(s:%, n:Z):T == {
		import from Rep, A T;
		assert(n >= 0);
		f := func s;
		if (m := constFrom s) >= 0 and n > m then n := m;
		resize!(s, n);
		a := data s;
		itm := #s;
		-- elts must be set before f() is called
		-- since f() could recursively ask for previous elements
		rep(s).elts := max(itm, next n);
		for j in itm..n repeat a.j := f();
		a.n;
	}

	set!(s:%, n:Z, t:T):T == {
		import from Rep, A T, Partial T;
		assert(n >= 0);
		x := s.n;
		(m, u) := constant s;
		m >= 0 and n >= m => {
			resize!(s, n);	-- force resizing beyond constant
			c := retract u;
			a := data s;
			itm := #s;
			for j in itm..prev n repeat a.j := c;
			rep(s).elts := max(itm, next n);
			rep(s).ncnst := next n;
			a.n := t;
		}
		data(s)(n) := t;
	}

	free!(s:%):() == {
		import from A T, Rep;
		free! data s;
		dispose! rep s;
	}

	if T has OutputType then {
		(p:TextWriter) << (s:%):TextWriter == {
			import from String, Boolean, T, Partial T;
			p := p << "[";
		       	if (n := #s) > 0 then {
				p := p << s.0;
				for i in 1..prev n repeat p := p << "," << s.i;
				p := p << ",";
			}
			(m, u) := constant s;
			m < 0 => p << "...]";
			if n <= m then p := p << "...";
			p << "(" << retract u << ")]";
		}
	}

	if T has PrimitiveType then {
		equal?(s:%, t:%, n:Z):Boolean == {
			import from T;
			assert(n >= 0);
			-- must go by increasing indices to avoid
			-- extra calculations for unequal streams
			for i in 0..prev n repeat { s.i ~= t.i => return false }
			true;
		}
	}
}

#if ALDORTEST
---------------------- test sal_stream.as --------------------------
#include "aldor"
#include "aldortest"

macro Z == MachineInteger;

genfib(n:Z, f:Stream Z):Z == {
	n = 0 or n = 1 => 1;
	f(n-1) + f(n-2);
}

local fib():Boolean == {
	import from Z, Stream Z;
	s := stream genfib;
	s.20 = 10946;
}

stderr << "Testing sal__stream..." << newline;
aldorTest("fib", fib);
stderr << newline;
#endif


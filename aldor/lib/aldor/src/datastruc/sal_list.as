---------------------------- sal_list.as ------------------------------------
--
-- This file defines basic lists
--
-- Copyright (c) Manuel Bronstein 1998
-- Copyright (c) INRIA 1998, Version 29-10-98
-- Logiciel Salli (c) INRIA 1998, dans sa version du 29/10/1998
-----------------------------------------------------------------------------

#include "aldor"

macro {
	Ch == Character;
	Z  == MachineInteger;
}

#if ALDOC
\thistype{List}
\History{Manuel Bronstein}{28/9/98}{created}
\Usage{import from \this~T}
\Params{{\em T} & Type & the type of the list entries\\}
\Descr{\this~provides lists of entries of type $T$, $1$-indexed and without
bound checking.}
\begin{exports}
\category{\altype{ListType} T}\\
\end{exports}
#endif

List(T:Type): ListType T == add {
	Rep == Record(elt:T, next:%);
	import from Rep;

	empty:%					== per nil;
	empty?(l:%):Boolean			== nil? rep l;
	apply(l:%, n:Z):T			== first(l + prev n);-- 1-indexd
	cons(x:T, l:%):%			== per [x, l];
	insert!(x:T, l:%):%			== cons(x, l);
	insert(x:T, l:%):%			== cons(x, l);
	firstIndex:Z				== 1;
	copy(l:%):%				== [x for x in l];
	map(f:T -> T)(l:%):%			== [f x for x in l];
	local comma:Ch				== char 44;
	local leftBracket:Ch			== char 91;
	local rightBracket:Ch			== char 93;
	append!(l:%, x:T):%			== append!(l, cons(x, empty));
	local setEntry!(l:%, e:T):T		== rep(l).elt := e;
	sort!(l:%, less:(T, T) -> Boolean):%	== sort!(l, #l, less);

	setFirst!(l:%, t:T):T == {
		import from Boolean;
		assert(~empty? l);
		rep(l).elt := t;
	}

	setRest!(l:%, s:%):% == {
		import from Boolean;
		assert(~empty? l);
		rep(l).next := s;
	}

	delete!(l:%, n:Z):% == {
		assert(1 <= n); assert(n <= #l);
		n = 1 => rest l;
		ll := l + prev prev n;
		setRest!(ll, rest rest ll);
		l;
	}

	copy!(m:%, l:%):% == {
		import from Boolean;
		ans := m;
		last:% := empty;
		while ~(empty? m or empty? l) repeat {
			setEntry!(m, first l);
			last := m;
			m := rest m;
			l := rest l;
		}
		if ~empty?(last) then setRest!(last, copy l);
		ans;
	}

	reverse!(l:%):% == {
		import from Boolean;
		empty? l => l;
		forward := rest l;
		setRest!(l, empty);
		while ~empty? forward repeat {
			t := rest forward;
			setRest!(forward, l);
			l := forward;
			forward := t;
		}
		l;
	}

	append!(l:%, s:%):% == {
		import from Boolean;
		ans := chase := l;
		while ~empty? l repeat {
			chase := l;
			l := rest l;
		}
		empty? chase => s;		-- l was empty at the start
		assert(empty? rest chase);
		setRest!(chase, s);
		ans;
	}

	map!(f:T -> T)(l:%):% == {
		import from Boolean;
		ll := l;
		while ~empty? l repeat {
			rep(l).elt := f first l;
			l := rest l;
		}
		ll;
	}

	new(n:Z, t:T):% == {
		l := empty;
		for i in 1..n repeat l := cons(t, l);
		l;
	}

	set!(l:%, n:Z, x:T):T == {
		assert(1 <= n); assert(n <= #l);
		r := rep(l + prev n);	-- 1-indexed so translate by n-1
		r.elt := x;
	}

	generator(l:%):Generator T == generate {
		import from Boolean;
		while ~empty? l repeat {
			yield first l;
			l := rest l;
		}
	}

	free!(l:%):() == {
		import from Boolean, Machine;
		while ~empty? l repeat {
			rec := rep l;
			l := rest l;
			dispose! rec;
		}
	}

	bracket(t:Tuple T):% == {
		import from Z;
		length(t) = 0 => empty;
		length(t) = 1 => cons(element(t, 1), empty);
		l := empty;
		for n in length(t)..1 by -1 repeat l := cons(element(t, n), l);
		l;
	}

	bracket(g:Generator T):% == {
		h := l := empty;
		for t in g repeat {
			temp := l;
			l := cons(t, empty);
			if empty? temp then h := l; else rep(temp).next := l;
		}
		h;
	}

	local sort!(l:%, n:Z, less:(T, T) -> Boolean):%	== {
		assert(n = #l);
		n < 2 => l;
		n = 2 => {
			less(first l, first rest l) => l;
			reverse! l;
		}
		-- break l in the middle (in-place)
		m := n quo 2;
		assert(m >= 1);
		t := l + prev m;	-- will be last elt of first-half
		l2 := rest t;		-- second half
		setRest!(t, empty);	-- break l into 2 pieces (l and l2)
		assert(#l = m); assert(#l2 = n - m);
		l := sort!(l, m, less);
		l2 := sort!(l2, n - m, less);
		merge!(l, l2, less);
	}

	-- l1 and l2 must be sorted in ascending order
	merge!(l1:%, l2:%, less:(T, T) -> Boolean):% == {
		import from Boolean;
		empty? l1 => l2;
		empty? l2 => l1;
		local l:%;
		if less(first l1, first l2)	then { l := l1; l1 := rest l1 }
						else { l := l2; l2 := rest l2 }
		t := l;
		while (~empty? l1) and (~empty? l2) repeat {
			if less(first l1, first l2) then {
				setRest!(t, l1);
				t := l1;
				l1 := rest l1;
			}
			else {
				setRest!(t, l2);
				t := l2;
				l2 := rest l2;
			}
		}
		setRest!(t, { empty? l1 => l2; l1 });
		l;
	}

	if T has PrimitiveType then {
		removeAll(t:T, l:%):% == {
			import from Boolean;
			while ~empty?(l) and t = first l repeat l := rest l;
			empty? l => l;
			assert(~empty? l); assert(t ~= first l);
			cons(first l, removeAll(t, rest l));
		}

		remove(t:T, l:%):% == {
			empty? l => l;
			t = first l => rest l;
			cons(first l, remove(t, rest l));
		}

		remove!(t:T, l:%):% == {
			import from Boolean;
			remove!(t, l, true);
		}

		removeAll!(t:T, l:%):% == {
			import from Boolean;
			remove!(t, l, false);
		}

		local remove!(t:T, l:%, once?:Boolean):% == {
			empty? l => l;
			if t = first l then {
				l := rest l;
				once? => return l;
				while ~empty?(l) and t=first l repeat l:=rest l;
				empty? l => return l;
			}
			assert(~empty? l); assert(t ~= first l);
			ans := l;
			while ~empty?(rest l) repeat {
				if t = first rest l then {
					setRest!(l, rest rest l);
					once? => return ans;
				}
				l := rest l;
			}
			ans;
		}

		linearSearch(t:T, l:%, start:Z):(Boolean, Z, T) == {
			assert(start >= 1);
			(l, n) := find(t, l + prev start);
			empty? l => (false, n, t);
			(true, n, first l);
		}

		find(t:T, l:%):(%, Z) == {
			import from Boolean;
			while ~empty? l for n in firstIndex.. repeat {
				t = first l => return(l, n);
				l := rest l;
			}
			(empty, prev firstIndex);
		}

		(l1:%) = (l2:%):Boolean == {
			while ~empty?(l1) repeat {
				empty? l2 or (first l1 ~= first l2) =>
								return false;
				l1 := rest l1;
				l2 := rest l2;
			}
			empty? l2;
		}
	}

	first(l:%):T == {
		import from Boolean;
		assert(~empty? l);
		rep(l).elt;
	}

	rest(l:%):% == {
		import from Boolean;
		assert(~empty? l);
		rep(l).next;
	}

	(l:%) + (n:Z):% == {
		assert(0 <= n); assert(n <= #l);
		for i in 1..n repeat l := rest l;
		l;
	}

	#(l:%):Z == {
		import from Boolean;
		n:Z := 0;
		while ~empty?(l) repeat { l := rest l; n := next n; }
		n::Z;
	}

	reverse(l:%):% == {
		rl := empty;
		for x in l repeat rl := cons(x, rl);
		rl;
	}

	split!(l:%, pred?:T -> Boolean):(%, %) == {
		empty? l => (empty, empty);
		(l1, l2) := split!(rest l, pred?);
		pred? first l => {
			setRest!(l, l1);
			(l, l2);
		}
		setRest!(l, l2);
		(l1, l);
	}

	if T has OutputType then {
		(p:TextWriter) << (l:%):TextWriter == {
			import from Character, Boolean, T;
			p := p << leftBracket;
			if ~empty?(l) then {
				p := p << first l;
				while ~empty?(l := rest l) repeat
					p := p << comma << first l;
			}
			p << rightBracket;
		}
	}

	if T has SerializableType then {
		<< (p:BinaryReader):% == {
			import from Z, T;
			l:% := empty;
			n:Z := << p;			-- read size first
			for i in 1..n repeat l := cons(<< p, l);
			reverse! l;
		}
	}

	if T has InputType then {
		<< (p:TextReader):% == {
			import from Character, Z, T;
			local c:Character;
			while space?(c := << p) or c = newline repeat {};
			c ~= leftBracket => throw SyntaxException;
			-- watch for the empty list ([] or [   ])
			while space?(c := << p) or c = newline repeat {};
			c = rightBracket => empty;
			push!(c, p);
			l := empty;
			c := comma;
			while c = comma repeat {
				l := cons(<< p, l);
				while space?(c := <<p) or c = newline repeat {};
			}
			c = rightBracket => reverse! l;
			push!(c, p);
			throw SyntaxException;
		}
	}
}

#if ALDOC
\thistype{ListException}
\History{Manuel Bronstein}{10/7/2001}{created}
\Usage{
throw \this\\
try \dots catch E in \{ E has \altype{ListExceptionType} $=>$ \dots \}
}
\Descr{\this~is an exception type thrown by list access.}
#endif
ListException: ListExceptionType == add;

#if ALDOC
\thistype{ListExceptionType}
\History{Manuel Bronstein}{10/7/2001}{created}
\Usage{\this: Category}
\Descr{\this~is the category of exceptions thrown by list access.}
#endif
define ListExceptionType:Category == with;

#if ALDOC
\thistype{ListType}
\History{Manuel Bronstein}{9/7/2001}{created}
\Usage{\this~T: Category}
\Params{{\em T} & Type & the type of the list entries\\}
\Descr{\this~is the category of lists of entries of type $T$.}
\begin{exports}
\category{\altype{BoundedFiniteLinearStructureType} T}\\
\category{\altype{DynamicDataStructureType} T}\\
\alexp{$+$}: & (\%, \altype{MachineInteger}) $\to$ \% & translate the base\\
\alexp{append!}: & (\%, T) $\to$ \% & adds an entry at the end\\
\alexp{append!}: & (\%, \%) $\to$ \% & adds a list at the end\\
\alexp{cons}: & (T, \%) $\to$ \% & adds an entry at the front\\
\alexp{delete!}: & (\%, \altype{MachineInteger}) $\to$ \% & remove an entry\\
\alexp{first}: & \% $\to$ T & first entry\\
\alexp{merge!}:
& (\%, \%, (T, T) $\to$ \altype{Boolean}) $\to$ \% & merge sorted lists\\
\alexp{rest}: & \% $\to$ \% & all entries after the first\\
\alexp{reverse}: & \% $\to$ \% & reverse a list\\
\alexp{reverse!}: & \% $\to$ \% & reverse a list in--place\\
\alexp{setFirst!}: & (\%, T) $\to$ T & changes the first element of a list\\
\alexp{setRest!}: & (\%, \%) $\to$ \% & changes the rest of a list\\
\alexp{sort!}: & (\%, (T, T) $\to$ \altype{Boolean}) $\to$ \% & sort a list\\
\alexp{split!}: & (\%, T $\to$ \altype{Boolean}) $\to$ (\%, \%) & split a list\\
\end{exports}
\begin{exports}[if $T$ has \altype{PrimitiveType} then]
\alexp{find}: & (T, \%) $\to$ (\%, \altype{MachineInteger}) & linear search\\
\end{exports}
\begin{exports}[if $T$ has \altype{TotallyOrderedType} then]
\category{\altype{TotallyOrderedType}}\\
\alexp{merge!}: & (\%, \%) $\to$ \% & merge sorted lists\\
\alexp{sort!}: & \% $\to$ \% & sort a list\\
\end{exports}
#endif

define ListType(T:Type): Category == Join(BoundedFiniteLinearStructureType T,
					DynamicDataStructureType T) with {
	+: (%, Z) -> %;
#if ALDOC
\alpage{$+$}
\Usage{l \name~n}
\Signature{(\%, \altype{MachineInteger})}{\%}
\Params{
{\em l} & \% & a list\\
{\em n} & \altype{MachineInteger} & an index\\
}
\Retval{$l\name n$ returns the sublist of $l$ starting at the
${(n+1)}^{{\rm st}}$ element of $l$, \ie $l + 0$ returns $l$, $l + 1$
returns the subslist starting at the second element of $l$, etc\dots.
No copy of $l$ is made.}
\begin{asex}
If $l$ is the list of \altype{MachineInteger} $[1,2,3,4,5]$,
then $l + 2$ is the list $[3,4,5]$.
\end{asex}
#endif
	append!: (%, T) -> %;
	append!: (%, %) -> %;
#if ALDOC
\alpage{append!}
\Usage{\name(l, x)\\ \name(l, s)}
\Signatures{
\name: & (\%, T) $\to$ \%\\
\name: & (\%, \%) $\to$ \%\\
}
\Params{
{\em l,s} & \% & lists\\
{\em x} & T & an entry\\
}
\Retval{\name(l,x) and \name(l,s) return the lists
$[l,x]$ and $[l,s]$ respectively.}
\Remarks{\name~does not make a copy of $l$, which is therefore
modified after the call. It is unsafe to use the variable $l$
after the call, unless it has been assigned to the result
of the call, as in {\tt l := append!(l, x)}.}
\alseealso{\alexp{cons}}
#endif
	cons: (T, %) -> %;
#if ALDOC
\alpage{cons}
\Usage{\name(x, l)}
\Signature{(T, \%)}{\%}
\Params{
{\em x} & T & an entry\\
{\em l} & \% & a list\\
}
\Retval{Returns the list $[x, l]$. Does not make a copy of $l$.}
\alseealso{\alexp{append!}}
#endif
	delete!: (%, Z) -> %;
#if ALDOC
\alpage{delete!}
\Usage{\name(l, n)}
\Signature{(\%, \altype{MachineInteger})}{\%}
\Params{
{\em l} & \% & a list\\
{\em n} & \altype{MachineInteger} & an index\\
}
\Retval{Returns the list $l$ with $l.n$ removed. Does not make a copy of $l$.}
#endif
	if T has PrimitiveType then {
		find: (T, %) -> (%, Z);
#if ALDOC
\alpage{find}
\Usage{\name(t, l)}
\Signature{(T, \%)}{(\%, \altype{MachineInteger})}
\Params{
{\em t} & T & the value to search for\\
{\em l} & \% & a list\\
}
\Retval{Returns (b, i) such that:
\begin{itemize}
\item if b is not \alfunc{FiniteLinearStructureType}{empty},
then $l.i = t$ is the first occurence
of t in l, and b is the sublist of l starting at $l.i$,
\item if b is \alfunc{FiniteLinearStructureType}{empty},
then i is undefined and t does not occur in l.
\end{itemize}
The list a does not need to be sorted, and the complexity is expected
to be linear in its size.}
\Remarks{No copy of l is made:
if b is not \alfunc{FiniteLinearStructureType}{empty},
then its data is shared with l.
This function allows all the occurences of t to be found
successively in a list, as in the example below.}
\begin{asex}
If {\tt l1} is the list of \altype{MachineInteger} [1,6,2,5,3,7,2,4],
then {\tt (l2, n) := find(2, l1)} sets {\tt l2} to [2,5,3,7,2,4]
and {\tt n} to 3, the further call {\tt (l3, n) := find(2, rest l2)}
sets {\tt l3} to [2,4] and {\tt n} to 4, and the further call
{\tt (l4, n) := find(2, rest l3)} sets {\tt l4} to
\alfunc{FiniteLinearStructureType}{empty}.
\end{asex}
#endif
	}
	first: % -> T;
#if ALDOC
\alpage{first}
\Usage{\name~l}
\Signature{\%}{T}
\Params{ {\em l} & \% & a nonempty list\\ }
\Retval{Returns the first entry of $l$.}
#endif
	merge!: (%, %, (T, T) -> Boolean) -> %;
	if T has TotallyOrderedType then {
		TotallyOrderedType;
		merge!: (%, %) -> %;
#if ALDOC
\alpage{merge!}
\Usage{\name($l_1,l_2$)\\ \name($l_1, l_2$, f)}
\Signature{(\%,\%,(T,T) $\to$ \altype{Boolean})}{\%}
\Params{
$l_1,l_2$ & \% & sorted lists\\
{\em f} & (T, T) $\to$ \altype{Boolean} & a comparison function\\
}
\Descr{Given sorted $l_i = [t_{i1},t_{i2},\dots]$ where $f(t_{ij}, t_{ik})$
is \true{} whenever $j < k$, returns the list containing all the
elements of $l_1$ and $l_2$, sorted using the ordering
$x < y \iff f(x,y)$. The comparison function $f$ is optional if
$T$ has \altype{TotallyOrderedType}, in which case the order
function of $T$ is taken.}
\Remarks{\name~does not make copies of $l_1$ and $l_2$, which are therefore
modified after the call. It is unsafe to use the variable $l_1$ and $l_2$
after the call, unless the result of the call has been assigned,
as in {\tt l := \name($l_1,l_2$)}.}
\alseealso{\alexp{sort!}}
#endif
	}
	rest: % -> %;
#if ALDOC
\alpage{rest}
\Usage{\name~l}
\Signature{\%}{\%}
\Params{ {\em l} & \% & a nonempty list\\ }
\Retval{Returns $l$ with the first element removed.
Does not make a copy of $l$.}
\alseealso{\alexp{setRest!}}
#endif
	reverse: % -> %;
	reverse!: % -> %;
#if ALDOC
\alpage{reverse}
\altarget{\name!}
\Usage{\name~l\\ \name!~l}
\Signature{\%}{\%}
\Params{ {\em l} & \% & a list\\ }
\Retval{\name(l) returns a copy of $l$ with the elements in reverse order,
while \name!(l) reverses $l$ without copying it.}
\Remarks{\name!~does not make a copy of $l$, which is therefore
modified after the call. It is unsafe to use the variable $l$
after the call, unless it has been assigned to the result
of the call, as in {\tt l := reverse!~l}.}
#endif
	setFirst!: (%, T) -> T;
#if ALDOC
\alpage{setFirst!}
\Usage{\name(l,t)}
\Signature{(\%,T)}{T}
\Params{
{\em l} & \% & a nonempty list\\
{\em t} & \% & an element\\
}
\Descr{Replaces the first element of {\em l} by {\em t} and returns {\em t}.}
\Remarks{\name(l,t) does not make a copy of {\em l}, which is therefore
modified after the call.}
\alseealso{\alexp{setRest!}}
#endif
	setRest!: (%, %) -> %;
#if ALDOC
\alpage{setRest!}
\Usage{\name(l,t)}
\Signature{(\%,\%)}{\%}
\Params{
{\em l} & \% & a nonempty list\\
{\em t} & \% & a list\\
}
\Descr{Replaces l by the list {\tt [first(l),t]} and returns t.}
\Remarks{\name(l,t) does not make a copy of {\em l}, which is therefore
modified after the call.}
\alseealso{\alexp{setFirst!}}
#endif
	sort!: (%, (T, T) -> Boolean) -> %;
	if T has TotallyOrderedType then sort!: % -> %;
#if ALDOC
\alpage{sort!}
\Usage{\name~l\\ \name(l, f)}
\Signature{(\%,(T,T) $\to$ \altype{Boolean})}{\%}
\Params{
{\em l} & \% & a list\\
{\em f} & (T, T) $\to$ \altype{Boolean} & a comparison function\\
}
\Descr{Sorts the list $a$ using the ordering
$x < y \iff f(x,y)$. The comparison function $f$ is optional if
$T$ has \altype{TotallyOrderedType}, in which case the order
function of $T$ is taken.}
\Remarks{\name~does not make a copy of $l$, which is therefore
modified after the call. It is unsafe to use the variable $l$
after the call, unless it has been assigned to the result
of the call, as in {\tt l := \name~l}.}
\alseealso{\alexp{merge!}}
#endif
	split!: (%, T -> Boolean) -> (%, %);
#if ALDOC
\alpage{split!}
\Usage{\name(l, f)}
\Signature{(\%,T $\to$ \altype{Boolean})}{(\%,\%)}
\Params{
{\em l} & \% & a list\\
{\em f} & T $\to$ \altype{Boolean} & a predicate\\
}
\Retval{Returns sublists $l_t$ and $l_f$ of \emph{l} such
that $l_t$ is the list of elements of \emph{l} for which \emph{f}
is \true, and $l_f$ is the list of elements of \emph{l} for which \emph{f}
is \false.}
\Remarks{\name~does not make a copy of \emph{l}, which is therefore
destroyed after the call. No new list is created by \name, but
the elements of \emph{l} are relinked into $l_t$ and $l_f$.}
#endif
	default {
		if T has TotallyOrderedType then {
			sort!(a:%):%		== sort!(a, <$T);
			merge!(a:%, b:%):%	== merge!(a, b, <$T);

			(a:%) < (b:%):Boolean ==  {
				import from T;
				while (~empty? a) and (~empty? b) repeat {
					first(a) ~= first(b) =>
						return(first(a) < first(b));
					a := rest a;
					b := rest b;
				}
				empty?(a) and ~empty?(b);
			}
		}
	}
}

extend Generator(T:Type):with {
	concat: List % -> %;
	interlacing: List % -> %;
} == add {
	-- 1.0.1 COMPILER BUG: DOES NOT COMPILE UNLESS THE CODE IS IN A
	-- SEPARATE PACKAGE
	interlacing(l:List %):% == workAround(l)$CompilerBugWorkAround(T);
	concat(l:List %):% == workAround0(l)$CompilerBugWorkAround(T);
}

local CompilerBugWorkAround(T:Type):with {
	workAround: List Generator T -> Generator T;
	workAround0: List Generator T -> Generator T;
} == add {
	-- WOULD LIKE TO USE a TRY/CATCH for a GeneratorExceptionType INSTEAD
	-- OF Partial T, BUT THE 1.0.1 COMPILER PRODUCES TRASH ON SUCH CODE
	workAround(l:List Generator T):Generator T == generate {
		import from Partial T;
		cont?:Boolean := true;
		while cont? repeat for g in l while cont? repeat {
			u := partialNext! g;
			if failed? u then cont? := false; else yield retract u;
		}
	}

	workAround0(l:List Generator T):Generator T == generate {
		for g in l repeat for t in g repeat yield t;
	}
}

#if ALDORTEST
#include "aldor"
#include "aldortest"

testSmallLists(): () == {
    import from Assert List Integer;
    import from Assert Integer;
    import from List Integer;
    import from Integer;
    none: List Integer := [];
    assertTrue(empty? none);
    one := [1];
    assertFalse(empty? one);
    assertEquals(1, first one);
    two := [2,1];
    assertFalse(empty? two);
    assertEquals(2, first two);
    assertEquals(one, rest two);
}


testSmallLists();

#endif

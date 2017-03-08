---------------------------- sal_set.as ------------------------------------
--
-- This file defines basic sets
--
-- Copyright (c) Manuel Bronstein 1999
-- Copyright (c) INRIA 1998, Version 29-10-98
-- Logiciel Salli (c) INRIA 1998, dans sa version du 29/10/1998
-----------------------------------------------------------------------------

#include "aldor"

macro {
	Ch == Character;
	Z  == MachineInteger;
}

#if ALDOC
\thistype{Set}
\History{Manuel Bronstein}{9/11/99}{created}
\Usage{import from \this~T}
\Params{{\em T} & \altype{PrimitiveType} & the type of the set entries\\}
\Descr{\this~provides sets of entries of type $T$, $1$-indexed and without
bound checking.}
\begin{exports}
\category{\altype{BoundedFiniteLinearStructureType} T}\\
\category{\altype{DynamicDataStructureType} T}\\
\alexp{$-$}: & (\%, \%) $\to$ \% & set difference\\
\alexp{intersection}: & (\%, \%) $\to$ \% & intersect two sets\\
\alexp{intersection!}: & (\%, \%) $\to$ \% & intersect two sets\\
\alexp{minus!}: & (\%, \%) $\to$ \% & set difference\\
\alexp{union}: & (\%, T) $\to$ \% & add an element\\
               & (\%, \%) $\to$ \% & add a set of elements\\
\alexp{union!}: & (\%, T) $\to$ \% & add an element\\
                & (\%, \%) $\to$ \% & add a set of elements\\
\end{exports}
#endif

Set(T:PrimitiveType): Join(BoundedFiniteLinearStructureType T,
					DynamicDataStructureType T) with {
	-: (%, %) -> %;
#if ALDOC
\alpage{$-$}
\Usage{$x - y$}
\Signature{(\%, \%)}{\%}
\Params{ {\em x,y} & \% & sets\\ }
\Retval{Return $x - y = \{ a \in x \st a \notin y\}$.}
\alseealso{\alexp{minus!}}
#endif
	intersection: (%, %) -> %;
	intersection!: (%, %) -> %;
#if ALDOC
\alpage{intersection}
\altarget{\name!}
\Usage{\name(x,y)\\ \name!(x,y)}
\Signature{(\%, \%)}{\%}
\Params{ {\em x,y} & \% & sets\\ }
\Retval{Return $x \cap y = \{ a \st a \in x \mbox{ and } a \in y\}$.}
\Remarks{\name!~does not make a copy of $x$, which is therefore
modified after the call. It is unsafe to use the variable $x$
after the call, unless it has been assigned to the result
of the call, as in {\tt x := \name!(x, y)}.}
#endif
	minus!: (%, %) -> %;
#if ALDOC
\alpage{minus!}
\Usage{\name(x,y)}
\Signature{(\%, \%)}{\%}
\Params{ {\em x,y} & \% & sets\\ }
\Descr{Return $x - y = \{ a \in x \st a \notin y\}$.}
\Remarks{\name~does not make a copy of $x$, which is therefore
modified after the call. It is unsafe to use the variable $x$
after the call, unless it has been assigned to the result
of the call, as in {\tt x := \name(x, y)}.}
\alseealso{\alexp{$-$}}
#endif
	union: (%, T) -> %;
	union: (%, %) -> %;
	union!: (%, T) -> %;
	union!: (%, %) -> %;
#if ALDOC
\alpage{union}
\altarget{\name!}
\Usage{\name(x,t)\\ \name(x,y)\\ \name!(x,t)\\ \name!(x,y)}
\Signature{(\%, \%)}{\%}
\Params{
{\em x,y} & \% & sets\\
{\em t} & T & an element\\
}
\Retval{\name(x,y) and \name!(x,y) both
return $x \cup y = \{ a \st a \in x \mbox{ or } a \in y\}$,
while \name(x,t) and \name!(x,t) both return $x \cup \{t\}$.}
\Remarks{\name!~does not make a copy of $x$, which is therefore
modified after the call. It is unsafe to use the variable $x$
after the call, unless it has been assigned to the result
of the call, as in {\tt x := \name!(x, y)}.}
#endif
} == add {
	Rep == List T;
	import from Rep;

	empty:%				== per empty;
	empty?(l:%):Boolean		== empty? rep l;
	apply(l:%, n:Z):T		== rep(l).n;
	firstIndex:Z			== firstIndex$Rep;
	copy(l:%):%			== per copy rep l;
	#(l:%):Z			== #(rep l);
	generator(l:%):Generator T	== generator rep l;
	free!(l:%):()			== free! rep l;
	(l:%) + (n:Z):%			== per(rep(l) + n);
	union(l:%, x:T):%		== union!(copy l, x);
	union(l1:%, l2:%):%		== union!(copy l1, l2);
--	intersection(l1:%, l2:%):%	== intersection!(copy l1, l2);
	(l1:%) - (l2:%):%		== minus!(copy l1, l2);
	map(f:T -> T)(l:%):%		== map!(f)(copy l);
	copy!(m:%, l:%):%		== per copy!(rep m, rep l);
	member?(t:T, l:%):Boolean	== member?(t, rep l);
	insert!(t:T, l:%):%		== insert(t, l);
	remove(t:T, l:%):%		== per remove(t, rep l);
	remove!(t:T, l:%):%		== per remove!(t, rep l);
	removeAll(t:T, l:%):%		== remove(t, l);
	removeAll!(t:T, l:%):%		== remove!(t, l);
	local comma:Ch			== char 44;
	local leftBracket:Ch		== char 123;
	local rightBracket:Ch		== char 125;
	linearSearch(t:T, s:%, n:Z):(Boolean, Z, T) == linearSearch(t,rep s,n);

	insert(t:T, l:%):% == {
		member?(t, l) => l;
		per cons(t, rep l);
	}

	map!(f:T -> T)(l:%):% == {
		empty? l  => empty;
		ll := rep l;
		ll.1 := f first ll;
		lr := map!(f)(per rest ll);
		member?(first ll, lr) => lr;
		setRest!(ll, rep lr);
		per ll;
	}

	minus!(l1:%, l2:%):% == {
		empty? l1 or empty? l2 => l1;
		x := first(l := rep l1);
		ll := per rest l;
		member?(x, l2) => minus!(ll, l2);
		setRest!(l, rep minus!(ll, l2));
		per l;
	}

	intersection(l1: %, l2: %): % == [elt for elt in l1 | member?(elt, l2)];
	--intersection(l1: %, l2: %): % == intersection!(copy l1, l2);
	intersection!(l1:%, l2:%):% == {
		empty? l1 or empty? l2 => empty;
		x := first(l := rep l1);
		ll := per rest l;
		member?(x, l2) => {
		    setRest!(l, rep intersection!(ll, l2));
		    l1;
		}
		intersection!(ll, l2);
	}

	union!(l:%, x:T):% == {
		member?(x, l) => l;
		per cons(x, rep l);
	}

	union!(l1:%, l2:%):% == {
		for x in l2 repeat l1 := union!(l1, x);
		l1;
	}

	new(n:Z, t:T):% == {
		assert(n >= 0);
		zero? n => empty;
		[t];
	}

	set!(l:%, n:Z, x:T):T == {
		assert(1 <= n);
		(ll, m) := find(x, rep l);
		rep(l).n := x;
		empty? ll or n = m => x;	-- no other occurence of x in l
		delete!(rep l, max(n, m));
		x;
	}

	bracket(t:Tuple T):% == {
		import from Z;
		l:% := empty;
		for n in length(t)..1 by -1 repeat l := union!(l, element(t,n));
		l;
	}

	bracket(g:Generator T):% == {
		l:% := empty;
		for t in g repeat l := union!(l, t);
		l;
	}

	(l1:%) = (l2:%):Boolean == {
		import from Z;
		#l1 ~= #l2 => false;
		for a in l1 repeat ~member?(a, l2) => return false;
		true;
	}

	if T has OutputType then {
		(p:TextWriter) << (l:%):TextWriter == {
			import from Character, Boolean, T;
			p := p << leftBracket;
			if ~empty?(ll := rep l) then {
				p := p << first ll;
				while ~empty?(ll := rest ll) repeat
					p := p << comma << first ll;
			}
			p << rightBracket;
		}
	}

	if T has SerializableType then {
		<< (p:BinaryReader):% == {
			import from Z, T;
			l:% := empty;
			n:Z := << p;			-- read size first
			for i in 1..n repeat l := union!(l, (<< p)@T);
			l;
		}
	}

	if T has InputType then {
		<< (p:TextReader):% == {
			import from Character, Z, T;
			local c:Character;
			while space?(c := << p) or c = newline repeat {};
			l:% := empty;
			c ~= leftBracket => throw SyntaxException;
			c := comma;
			while c = comma repeat {
				l := union!(l, (<< p)@T);
				while space?(c := <<p) or c = newline repeat {};
			}
			c = rightBracket => l;
			push!(c, p);
			throw SyntaxException;
		}
	}
}

#if ALDORTEST
#include "aldor"
#pile

test(): () ==
    import from Assert Set String, String
    s1: Set String := ["a"]
    s2: Set String := ["b", "a"]
    assertEquals(s1, intersection(s1, s2))
    s3: Set String := ["a", "b"]
    assertEquals(s1, intersection(s1, s3))

test()

#endif

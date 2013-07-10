---------------------------- ald_queue.as ------------------------------------
--
-- This file defines linear queues
--
-- Copyright (c) Manuel Bronstein 2003
-- Copyright (c) INRIA 2003
-- Logiciel libaldor (c) INRIA 2003
-----------------------------------------------------------------------------

#include "aldor"

macro {
	Ch == Character;
	Z  == MachineInteger;
}

#if ALDOC
\thistype{Queue}
\History{Manuel Bronstein}{17/11/2003}{created}
\Usage{import from \this~T}
\Params{{\em T} & Type & the type of the entries\\}
\Descr{\this~provides queues of entries of type $T$, $1$-indexed and without
bound checking.}
\begin{exports}
\category{\altype{BoundedFiniteLinearStructureType} T}\\
\category{\altype{DynamicDataStructureType} T}\\
\alexp{remove!}: & \% $\to$ T & remove an element\\
\end{exports}
#endif

Queue(T:Type): Join(BoundedFiniteLinearStructureType T,
			DynamicDataStructureType T) with {
	remove!: % -> T;
#if ALDOC
\alpage{remove!}
\Usage{\name~q}
\Signature{\%}{T}
\Params{ {\em q} & \% & a nonempty queue\\}
\Descr{Removes the oldest element of the queue {\em q} and returns it.}
#endif
} == add {
	Rep == Record(frst:List T, lst:List T);
	import from List T;

	empty:%				== { import from Rep;per [empty,empty];}
	empty?(l:%):Boolean		== empty? head l;
	local head(l:%):List T		== { import from Rep; rep(l).frst; }
	local tail(l:%):List T		== { import from Rep; rep(l).lst; }
	apply(l:%, n:Z):T		== head(l)(n);
	firstIndex:Z			== 1;
	copy(l:%):%			== queue copy head l;
	map(f:T -> T)(l:%):%		== queue map(f)(head l);
	map!(f:T -> T)(q:%):%		== { map!(f)(head q); q}
	local queue(l:List T):%		== { import from Rep; per [l, last l]; }
	copy!(a:%, b:%):%		== queue!(a, copy!(head a, head b));
	generator(q:%):Generator T	== generator head q;
	#(q:%):Z			== #(head q);
	[t:Tuple T]:%			== queue [t];
	[g:Generator T]:%		== queue [g];
	new(n:Z, t:T):%			== queue new(n, t);
	set!(q:%, n:Z, t:T):T		== set!(head q, n, t);
	insert(t:T, q:%):%		== insert!(t, copy q);

	local queue!(q:%, l:List T):% == {
		import from Rep;
		rep(q).frst := l;
		rep(q).lst := last l;
		q;
	}

	free!(q:%):() == {
		import from Rep;
		free! head q;
		dispose! rep q;
	}

	local last(l:List T):List T == {
		empty? l => empty;
		lst := l;
		while ~empty? rest lst repeat lst := rest lst;
		lst;
	}

	insert!(t:T, q:%):% == {
		import from Rep;
		l:List T := cons(t, empty);
		if empty? q then rep(q).frst := rep(q).lst := l;
		else rep(q).lst := setRest!(tail q, l);
		q;
	}

	remove!(q:%):T == {
		import from Rep;
		assert(~empty? q);
		t := first(l := head q);
		rep(q).frst := rest l;
		if empty?(rep(q).frst) then rep(q).lst := empty;
		t;
	}

	if T has PrimitiveType then {
		remove(t:T, q:%):%	== queue remove(t, head q);
		remove!(t:T, q:%):%	== queue!(q, remove!(t, head q));
		removeAll(t:T, q:%):%	== queue removeAll(t, head q);
		removeAll!(t:T, q:%):%	== queue!(q, removeAll!(t, head q));

		linearSearch(t:T, q:%, n:Z):(Boolean, Z, T) ==
			linearSearch(t, head q, n);
	}

	if T has HashType then { hash(q:%):Z == hash head q; }

	if T has InputType then {
		<< (port:TextReader):% == queue(<< port);
	}

	if T has OutputType then {
		(port:TextWriter) << (q:%):TextWriter == port << head q;
	}

	if T has SerializableType then {
		<< (port:BinaryReader):% == queue(<< port);
		(port:BinaryWriter) << (q:%):BinaryWriter == port << head q;
	}
}

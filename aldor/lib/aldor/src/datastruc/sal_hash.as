---------------------------- sal_hash.as ------------------------------------
--
-- This file defines hash tables
--
-- Copyright (c) Manuel Bronstein 1999
-- Copyright (c) INRIA 1999, Version 29-10-98
-- Logiciel Salli ©INRIA 1999, dans sa version du 29/10/1998
-----------------------------------------------------------------------------

#include "aldor"

macro {
	A == Array;
	L == List;
	Z == MachineInteger;
	Ch==Character;
}

#if ALDOC
\thistype{HashTable}
\History{Manuel Bronstein}{14/6/99}{created}
\Usage{import from \this(K, V)\\ import from \this(K, V, h)}
\Params{
{\em K} & \altype{PrimitiveType} & the type of the keys\\
	& \altype{HashType} & \\
{\em V} & Type & the type of the entries\\
{\em h} & K $\to$ \altype{MachineInteger} & the hash function to use\\
}
\Descr{\this~provides hash tables with keys of type $K$,
entries of type $V$ and that uses the hash--function $h$.
If $K$ has \altype{HashType}, then the parameter $h$ is
optional as the function \alfunc{HashType}{hash} is used
by default in that case.}
\begin{exports}
\category{\altype{TableType}(K, V)}\\
\alexp{forget}: & (K $\to$ V) $\to$ \% & creation of a table with default\\
\alexp{remember}: & (K $\to$ V) $\to$ \% & creation of a table with default\\
\alexp{remove!}: & (K, \%) $\to$ \% & removal of a key\\
\end{exports}
#endif

-- TEMPORARY: WORKAROUND FOR BUG1167
-- HashTable(K:HashType, V:Type):TableType(K, V) == HashTable(K, V, hash$K);
HashTable(K:HashType, V:Type): TableType(K, V) with {
	forget: (K -> V) -> %;
	remember: (K -> V) -> %;
	remove!: (K, %) -> %;
} == HashTable(K, V, hash$K) add;

HashTable(K:PrimitiveType, V:Type, hash: K -> Z): TableType(K, V) with {
	forget: (K -> V) -> %;
	remember: (K -> V) -> %;
#if ALDOC
\alpage{forget,remember}
\altarget{forget}
\altarget{remember}
\Usage{forget~f\\remember~f}
\Signature{(K $\to$ V)}{\%}
\Params{{\em f} & K $\to$ V & a default function\\ }
\Descr{Creates and return an empty table with default function {\em f}.
That function is used by \alfunc{TableType}{apply} and \alfunc{TableType}{find}
to compute the value of the entry whenever the key is not already present 
in the table. In addition, the pair $(k,v)$ is entered into the table
if {\tt remember} was used to create it. Thus the result of {\tt remember f}
can be viewed as a version of {\em f} that remembers the values of its
previous calls.}
\alseealso{\alfunc{TableType}{table}}
#endif
	remove!: (K, %) -> %;
#if ALDOC
\alpage{remove!}
\Usage{\name(k, t)}
\Params{
{\em k} & K & a key to remove\\
{\em t} & \% & a hash table\\
}
\Retval{Removes the occurence of \emph{k} in \emph{t} if any, and
returns \emph{t} after the removal.}
#endif
} == add {
	macro KV == KeyEntry(K, V);

	-- nentry = number of keys (not of slots)
	-- defType = 0 --> no default function
	--         > 0 --> remembering default function
	--         < 0 --> forgetting default function
	Rep == Record(nentr:Z, tbl:A L KV, defType:Z, defFun: K -> V);

	table():%		== { import from Z; table 8 };
	table(n:Z):%		== newTable(0, n);
	remember(f:K -> V):%	== { import from Z; newTable(0, 8, 1, f); }
	forget(f:K -> V):%	== { import from Z; newTable(0, 8, -1, f); }
	empty?(t:%):Boolean	== { import from Z; zero?(#t) }
	#(t:%):Z		== { import from A L KV; #(htable t) }
	numberOfEntries(t:%):Z	== { import from Rep; rep(t).nentr; }
	local htable(t:%):A L KV== { import from Rep; rep(t).tbl; }
	local index(t:%, k:K):Z	== hash(k) mod (#t);
	local type(t:%):Z	== { import from Rep; rep(t).defType; }
	local defaultFunction(t:%):K -> V == { import from Rep; rep(t).defFun; }
	local noDefault?(t:%):Boolean	== { import from Z; zero? type t; }
	local remember?(t:%):Boolean	== { import from Z; type(t) > 0; }

	-- must deep-copy so that changing tt.key does not change t.key
	copy(t:%):% == {
		import from Z, L KV, A L KV;
		zero?(sz := #t) => table 0;
		n := numberOfEntries t;
		tt := newTable(n, sz, type t, defaultFunction t);
		tab := htable t;
		ntab := htable tt;
		for i in 0..prev sz repeat ntab.i := deepcopyrev(tab.i);
		tt;
	}

	-- deep copies and reverses, order does not matter
	local deepcopyrev(l:L KV):L KV == {
		import from KV;
		ll:L KV := empty;
		for x in l repeat ll := cons(copy x, ll);
		ll;
	}

	-- returns the new number of entries
	local addEntry(t:%, n:Z):Z == {
		import from Rep;
		rep(t).nentr := rep(t).nentr + n;
	}

	-- n = number of keys in the table
	-- sz = number of slots in the table
	local newTable(n:Z, sz:Z):% ==
		newTable(n, sz, 0, nil$Pointer pretend K -> V);

	-- n = number of keys in the table
	-- sz = number of slots in the table
	local newTable(n:Z, sz:Z, type:Z, func:K -> V):% == {
		import from A L KV, L KV, Rep;
		per [n, new(sz, empty), type, func];
	}

	free!(t:%):() == {
		import from Boolean, KV, L KV, A L KV;
		for l in htable t repeat {
			for e in l repeat free! e;
			if ~empty? l then free! l;
		}
		free! htable t;
	}

	remove!(k:K, t:%):% == {
		import from Z, KV, L KV, A L KV;
		(found?, l, i, chase) := search(t, k);
		if found? then {
			assert(~empty? l);
			if empty? chase then htable(t).i := rest l;
			else setRest!(chase, rest l);
			addEntry(t, -1);
		}
		t;
	}

	set!(t:%, k:K, v:V):V == {
		import from Boolean, KV, List KV;
		(found?, l, i, ignore) := search(t, k);
		found? => {
			assert(~empty? l);
			setEntry!(first l, v);
		}
		add!(t, k, v, l, i);
	}

	-- this is only when k is not already in t
	local add!(t:%, k:K, v:V, l:List KV, i:Z):V == {
		import from Boolean, Z, KV, L KV, A L KV;
		e := [k, v];
		if empty? l then htable(t).i := cons(e,empty);else append!(l,e);
		-- grow when the load factor is greater than 2
		if addEntry(t, 1) > 2 * #t then grow! t;
		v;
	}

	local grow!(t:%):() == {
		import from Z, A L KV, L KV, KV, Rep;
		newsize := 2 * #t;
		assert(newsize > 0);
		newtable:A L KV := new(newsize, empty);
		for e in allEntries t repeat {
			i := hash(key e) mod newsize;
			l := newtable.i;
			if empty? l then newtable.i := cons(e, empty);
					else append!(l, e);
		}
		free! setTable!(t, newtable);
	}

	-- returns the old table
	local setTable!(t:%, htbl:A L KV):A L KV == {
		import from Rep;
		old := htable t;
		rep(t).tbl := htbl;
		old;
	}

	local allEntries(t:%):Generator KV == generate {
		import from L KV, A L KV;
		for l in htable t repeat for e in l repeat yield e;
	}

	generator(t:%):Generator Cross(K, V) == generate {
		import from KV;
		for e in allEntries t repeat yield explode e;
	}

	-- to be used only when k may or may not appear in t
	find(k:K, t:%):Partial V == {
		import from Boolean, KV, L KV;
		(found?, l, i, ignore) := search(t, k);
		found? => { assert(~empty? l); [entry first l] }
		noDefault? t => failed;
		v := defaultFunction(t)(k);
		remember? t => [add!(t, k, v, l, i)];
		[v];
	}

	-- to be used only when k appears in t
	apply(t:%, k:K):V == {
		import from Boolean, KV, L KV;
		(found?, l, i, ignore) := search(t, k);
		found? => {
			assert(~empty? l);
			entry first l;
		}
		noDefault? t => throw TableException;
		v := defaultFunction(t)(k);
		remember? t => add!(t, k, v, l, i);
		v;
	}

	-- returns (found?, l, i, pl) where
	--   i = index of l in the table
	--   l points to the key k if found? is true,
	--            to the last element of t.tab.i otherwise (could be empty)
	--   pl is empty if found? is false or if k is the first key in l,
	--   rest(pl) = l otherwise
	local search(t:%, k:K):(Boolean, L KV, Z, L KV) == {
		import from KV, A L KV;
		i := index(t, k);
		l := htable(t)(i);
		chase:List KV := empty;
		while ~empty? l repeat {
			k = key first l => return (true, l, i, chase);
			chase := l;
			l := rest l;
		}
		(false, chase, i, empty);
	}

	-- TEMPORARY: COMPILER INSISTS ON THAT FUNCTION (WHY?)
	if Cross(K, V) has HashType then {
		(t:%) = (s:%):Boolean == never;
	}
}

-----------------------------------------------------------------------------
----
---- table.as: Generic hash tables.
----
-----------------------------------------------------------------------------
---- Copyright (c) 1990-2007 Aldor Software Organization Ltd (Aldor.org).
-----------------------------------------------------------------------------

#include "axllib"

Hash ==> SingleInteger;

+++ `HashTable(Key, Val)' provides a parameterized hash-table data type.

HashTable(Key: BasicType, Value: BasicType): BasicType with {

	export from Key;
	export from Value;

	table: () -> %;
		++ `table()' creates a new table using the equality test `='
		++ and the hash function `hash' from the `Key' type.

	eqtable: () -> %;
		++ `eqtable()' creates a new table using instance equality.

	table: ((Key, Key) -> Boolean, Key->Hash) -> %;
		++ `table(=, hash)' creates a new hash table using the
		++ equality test `=' and the hash function `hash'.

	copy: % -> %;
		++ `copy t' creates a copy of the table `t'.

	#: % -> SingleInteger;
		++ `#t' returns the number of elements in `t'.

	search: (%, Key, Value) -> (Boolean, Value);
		++ `(b,v) := search(t,k,d)' searches table `t' for the value
		++ associated with key `k'. If there is such a value, `vk',
		++ then `b' is set to `true' and `v' is set to `vk'. 
		++ Otherwise `b' is `false' and `v' is set to `d'.

	apply: (%, Key) -> Value;
		++ `t.k' searches the table `t' for the value associated with
		++ the key `k'.  It is an error if there is no value for `k'.

	set!: (%, Key, Value) -> Value;
		++ `t.k := val' associates `val' with `k' in `t'.

	drop!: (%, Key) -> Value;
		++ `drop!(t, k)' removes the entry for `k' in `t'.

	dispose!: % -> ();
		++ `dispose! t' indicates a table will no longer be used.

	generator: % -> Generator Cross(Key, Value);
		++ `generator t' is a generator which produces all the
		++ `(key, value)' pairs from `t'.
}
== add {
	-- Parameters to tune table performance.
	InitBuckC ==> primes.3;
	MaxLoad	  ==> 5.0;
	MinLoad   ==> 0.5;


	-- primes.i is the largest prime <= 2^i.
	local primes: Array SingleInteger == [
		2,         3,          7,          13,
		31,        61,         127,        251,
		509,       1021,       2039,       4093,
		8191,      16381,      32749,      65521,
		131071,    262139,     524287,     1048573,
		2097143,   4194301,    8388593,    16777213,
		33554393,  67108859,   134217689,  268435399,
		536870909, 1073741789, 2147483647, 4294967291
	];
	local lg(n: SingleInteger): SingleInteger == {
		p := 1;
		for i in 0.. repeat { if n <= p then return i; p := p + p; }
		never
	}

	-- Representation
	Entry == Record(key: Key, value: Value, hash: Hash);

	Rep   == Record(isEq?:	Boolean,
			equal:	(Key, Key) -> Boolean,
			hash:	(Key) -> Hash,
			count:	SingleInteger,
			buckv:	Array List Entry);

	-- Local representation operaitons
	import from Rep;

	local new(isEq?: Boolean, e: (Key,Key)->Boolean, h: Key->Hash): % ==
		per [isEq?, e, h, 0, new(InitBuckC, nil)];

	local isEq? (t: %): Boolean              == rep(t).isEq?;
	local hash  (t: %): (Key) -> Hash	 == rep(t).hash;
	local equal (t: %): (Key,Key) -> Boolean == rep(t).equal;
	local buckv (t: %): Array List Entry	 == rep(t).buckv;
	local buckc (t: %): SingleInteger	 == #rep(t).buckv;

	local inc!(t: %): () == {
		import from SingleFloat;
		rep(t).count := rep(t).count + 1;
		if #t::SingleFloat/buckc(t)::SingleFloat > MaxLoad then
			enlarge! t;
	}
	local dec!(t: %): () == {
		import from SingleFloat;
		rep(t).count := rep(t).count - 1;
		if #t::SingleFloat/buckc(t)::SingleFloat < MinLoad then
			shrink! t;
	}
		
	local peq(k1: Key, k2: Key): Boolean == {
		import from Pointer;
		k1 pretend Pointer = k2 pretend Pointer
	}
	local phash(k1: Key): Hash == {
		k1 pretend Pointer pretend Hash
	}

	-- Find the chain for k, moving the link to the front on success.
	local findChain(t: %, k: Key): SingleInteger == {
		h := hash(t)(k);
		n := h mod buckc(t) + 1;
		b := buckv(t).n;
		p := nil;	-- Previous link or nil.

		while b repeat {
			e := first b;
			if h = e.hash then {
				if isEq? t or equal(t)(e.key, k) then {
					-- Move to front
					if p then {
						p.rest     := b.rest;
						b.rest     := buckv(t).n;
						buckv(t).n := b;
					}
					return n;
				}
			}
			p := b;
			b := rest b;
		}
		return 0;
	}

	-- Resize the table, larger or smaller.
	local enlarge!(t: %): % == resize!(t, lg buckc(t) + 1);
	local shrink! (t: %): % == resize!(t, lg buckc(t) - 1);

	local resize!(t: %, sizeix: SingleInteger): % == {
		sizeix < 1 or sizeix > #primes => t;

		nbuckc := primes sizeix;
		nbuckv := new(nbuckc, nil);

		for b0 in buckv t repeat {
			b := b0;
			while b repeat {
				hd := b;
				b  := b.rest;

				n  := (hd.first.hash mod nbuckc) + 1;
				hd.rest  := nbuckv.n;
				nbuckv.n := hd;
			}
		}
		dispose! rep(t).buckv;
		rep(t).buckv := nbuckv;
		t;
	}


	-- Exported operations
	sample: % == table();
	(t1: %) = (t2: %): Boolean == {
		import from Pointer;
		t1 pretend Pointer = t2 pretend Pointer
	}
	(out: TextWriter) << (t: %): TextWriter == {
		out << "table(";
		any? := false;
		for b in buckv(t) repeat
			for e in b repeat {
				if any? then out << ", " else any? := true;
				out << e.key << " = " << e.value;
			}
		out << ")"
	}

	
	#(t: %): SingleInteger == rep(t).count;

	eqtable(): % == new(true, peq, phash);
	table(): %   == new(false, =$Key, hash$Key);
	table(eq:(Key,Key)->Boolean, hash:Key->Hash): % == new(false,eq,hash);

	copy(t: %): % ==
		per [isEq? t, equal t, hash  t, #t,
		     [[[e.key, e.value, e.hash] for e in b] for b in buckv t]];

	search(t: %, k: Key, def: Value): (Boolean, Value) == {
		n := findChain(t, k);
		if n = 0 then
			(false, def)
		else
			(true,  buckv(t).n.first.value)
	}
	apply(t: %, k: Key) : Value == {
		n := findChain(t, k);
		n = 0 => error "Element missing from table.";
		buckv(t).n.first.value;
	}
	set!(t: %, k: Key, v: Value) : Value == {
		n := findChain(t, k);
		n > 0 => buckv(t).n.first.value := v;
		h := hash(t)(k);
		n := (h mod buckc(t)) + 1;
		buckv(t).n := cons([k,v,h], buckv(t).n);
		inc! t;
		v;
	}
	drop!(t: %, k: Key): Value == {
		n := findChain(t, k);
		n = 0 => error "Element missing from table.";
		e := buckv(t).n.first;
		v := e.value;
		buckv(t).n := disposeHead! buckv(t).n; -- Dispose of the link.
		dispose! e;                            -- Dispose of the record.
		dec! t;
		v;
	}

	dispose!(t: %): () == {
		for b in buckv(t) repeat dispose! b;
		dispose! buckv(t);
		dispose! rep(t);
	}

	generator(t: %): Generator Cross(Key, Value) == generate {
		for b in buckv t repeat
			for e in copy b repeat {
				c: Cross(Key, Value) := (e.key, e.value);
				yield c
			}
	}
}

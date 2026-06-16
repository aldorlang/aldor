----------------------------- sal_sortas.as ----------------------------------
--
-- sas.as: A basic constructor for sorted sets of key-entry pairs
--
--  Copyright (c) 1990-2007 Aldor Software Organization Ltd (Aldor.org).
-- Copyright: INRIA, and University of Lille I, 2001
-- Copyright: Marc Moreno Maza
------------------------------------------------------------------------------

#include "aldor"

macro Z == MachineInteger;

#if ALDOC
\thistype{SortedAssociationSet}
\History{Marc Moreno Maza}{23/5/2001}{created}
\Usage{import from \this(K, V)}
\Params{
{\em K} & \astype{TotallyOrderedType} & the type of the keys\\
{\em V} & \builtin{Type} & the type of the entries\\
}
\Descr{\this(K, V) implements sorted sets of key-entry pairs.
The keys come from {\em K} with a total ordering and the entries come
from {\em T} which is any type. A sorted association set can be viewed
as a hash-table with the identity as hash-function.}
\begin{exports}
\category{\altype{TableType}(K, V)}\\
\end{exports}
#endif

SortedAssociationSet(K: TotallyOrderedType, V: Type): TableType(K, V) == add {
	macro KV == KeyEntry(K, V);
        macro SKV == SortedSet KV;
	Rep == Record(table:SKV);

        import from K, V, KV, SKV, Rep;

        copy(x:%):%		== per [copy(rep(x).table)];
        empty?(x: %): Boolean	== empty?(rep(x).table);
        #(x:%):Z		== { #(rep(x).table); }
	numberOfEntries(x:%):Z	== #x;
	table(n:Z):%		== table();
	table():%		== per [empty];

	free!(t:%):() == {
		free!(rep(t).table);
		dispose! rep t;
	}
		
	find(k:K, x:%): Partial(V) == {
		import from Partial(V);
		local skv: SKV :=  rep(x).table;
		for kv in skv repeat {
			if (key(kv) = k) then return [entry(kv)];
		}
		return failed;
        }

        set!(x:%, k:K, v:V):V == {
		kv: KV := [k,v];
		local skv :=  rep(x).table;
		(found?, pos, kvfound) := linearSearch(kv, skv);
		found? => setEntry!(kvfound, v);
		rep(x).table := insert!(kv, skv);
		v;
        }

	generator(x: %): Generator Cross(K, V) == generate {
		import from SKV, KV;
		local skv := rep(x).table;
		for kv in skv repeat yield explode kv;
	}

	-- TEMPORARY: COMPILER INSISTS ON THAT FUNCTION (WHY?)
	if Cross(K, V) has HashType then {
		(t:%) = (s:%):Boolean == never;
	}
}


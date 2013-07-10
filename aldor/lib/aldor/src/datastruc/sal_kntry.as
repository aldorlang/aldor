--------------------------- sal_kntry.as ------------------------------------
--
-- This file defines read/write key-entry pairs for tables
--
--  Copyright (c) 1990-2007 Aldor Software Organization Ltd (Aldor.org).
-- Copyright: INRIA, UWO, and University of Lille I, 2001
-- Copyright: Marc Moreno Maza
-----------------------------------------------------------------------------

#include "aldor"

#if ALDOC
\thistype{KeyEntry}
\History{Marc Moreno Maza}{23/5/2001}{created}
\Usage{import from \this(K, V)}
\Params{
{\em K} & \builtin{Type} & the type of the keys\\
{\em V} & \builtin{Type} & the type of the entries\\
}
\Descr{\this(K, V) provides pairs consisting of a key from {\em K},
and a entry from {\em V}. When {\em K} is a \altype{PrimitiveType},
then two pairs are equal if they share the same key.
When {\em K} is a \altype{TotallyOrderedType}, then
the pair {\em x} is greater than the pair {\em y} if
$key(x) > key(y)$. Hence key-entry pairs are useful for building
tables of entries where each slot is given by a unique key.}
\begin{exports}
\category{\altype{CopyableType}}\\
\alexp{[]}: & (K, V) $\to$ \% & construction of a key-entry pair\\
\alexp{entry}: & \% $\to$ V & get the entry\\
\alexp{explode}: & \% $\to$ (K, V) & get the key and the entry\\
\alexp{key}: & \% $\to$ K & get the key\\
\alfunc{DataStructureType}{free!}: & \% $\to$ () & memory disposal\\
\alexp{setEntry!}: & (\%, V) $\to$ V & change the entry\\
\alexp{setKey!}: & (\%, K) $\to$ K & change the key\\
\end{exports}
\begin{exports}[if {\em K} has \altype{PrimitiveType} then]
\category{\altype{PrimitiveType}}\\
\end{exports}
\begin{exports}[if {\em K} has \altype{TotallyOrderedType} then]
\category{\altype{TotallyOrderedType}}\\
\end{exports}
#endif

KeyEntry(K:Type, V:Type): CopyableType with {
	if K has PrimitiveType then PrimitiveType;
	if K has TotallyOrderedType then TotallyOrderedType;
	bracket: (K, V) -> %;
#if ALDOC
\alpage{[]}
\Usage{[{\em k,v}]}
\Signature{(K, V)}{\%}
\Params{
{\em k} & K & a key\\
{\em v} & V & an entry\\
}
\Retval{Returns the key-entry pair [{\em k, v}].}
#endif
	entry: % -> V;
	explode: % -> (K, V);
	key: % -> K;
#if ALDOC
\alpage{entry,explode,key}
\altarget{entry}
\altarget{explode}
\altarget{key}
\Usage{entry~p\\ (k, v) := explode~p\\ key~p}
\Signatures{
entry: & \% $\to$ V\\
explode: & \% $\to$ (K, V)\\
key: & \% $\to$ K\\
}
\Params{ {\em p} & \% & a key-entry pair\\ }
\Retval{entry(p) and key(p) return respectively the entry and key of {\em p},
while explode(p) returns the pair (key p, entry p).}
#endif
	free!: % -> ();
	setEntry!: (%, V) -> V;
	setKey!: (%, K) -> K;
#if ALDOC
\alpage{setEntry!,setKey!}
\altarget{setEntry!}
\altarget{setKey!}
\Usage{setEntry!(p, v)\\ setKey!(p, k)}
\Signatures{
setEntry!: & (\%, V) $\to$ V\\
setKey!: & (\%, K) $\to$ K\\
}
\Params{
{\em p} & \% & a key-entry pair\\
{\em k} & K & a key\\
{\em v} & V & an entry\\
}
\Descr{setEntry!(p, v) (resp.~setKey!(p, k)) changes the entry (resp.~key)
of {\em p} to {\em v} (resp.~{\em k}) and returns {\em v} (resp.~{\em k}).}
#endif
} == add {
	Rep == Record(lft: K, rgt: V);
	import from Rep;

	key(e:%):K		== rep(e).lft;
	entry(e:%):V		== rep(e).rgt;
	pair(a:K, b:V):%	== per [a, b];
	bracket(a:K, b:V):%	== pair(a, b);
	setKey!(e:%, a:K):K	== rep(e).lft := a;
	setEntry!(e:%, b:V):V	== rep(e).rgt := b;
	free!(e:%):()		== dispose! rep e;
	explode(e:%):(K, V)	== explode rep e;
	copy(e:%):%		== pair explode e;

	copy!(x:%, y:%):% == {
		setKey!(x, key y);
		setEntry!(x, entry y);
		x;
	}

	if K has PrimitiveType then {
		(x:%) = (y:%):Boolean == { import from K; key(x) = key(y) }
	}

	if K has TotallyOrderedType then {
		(x:%) < (y:%):Boolean == { import from K; key(x) < key(y) }
	}
}


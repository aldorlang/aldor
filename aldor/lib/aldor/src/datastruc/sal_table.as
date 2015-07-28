---------------------------- sal_table.as ------------------------------------
--
-- This file defines a common category for various sort of tables
--
-- Copyright (c) Manuel Bronstein 1999
-- Copyright (c) INRIA 1999, Version 29-10-98
-- Logiciel Salli (c) INRIA 1999, dans sa version du 29/10/1998
-----------------------------------------------------------------------------

#include "aldor"

macro {
	Z  == MachineInteger;
	Ch == Character;
}

#if ALDOC
\thistype{TableException}
\History{Manuel Bronstein}{24/06/2003}{created}
\Usage{
throw \this\\
try \dots catch E in \{ E has \altype{TableExceptionType} $=>$ \dots \}
}
\Descr{\this~is an exception type thrown by table access.}
#endif
TableException: TableExceptionType == add;

#if ALDOC
\thistype{TableExceptionType}
\History{Manuel Bronstein}{24/06/2003}{created}
\Usage{\this: Category}
\Descr{\this~is the category of exceptions thrown by table access.}
#endif
define TableExceptionType:Category == with;

#if ALDOC
\thistype{TableType}
\History{Manuel Bronstein}{16/10/98}{created}
\Usage{\this(K, V): Category}
\Params{
{\em K} & \altype{PrimitiveType} & the type of the keys\\
{\em V} & Type & the type of the entries\\
}
\Descr{\this(K, V) is the category of tables, \ie~discrete
many-to-one mappings from keys to entries. More precisely,
every element of a domain of this category is a table
whose slots contain elements from {\em V} and such that every
slot is given by a unique key from {\em K}.}
\begin{exports}
\category{\altype{BoundedFiniteDataStructureType} \builtin{Cross}(K, V)}\\
\alexp{[]}:
& \builtin{Tuple} \builtin{Cross}(K, V) $\to$ \% & creation of a table\\
\alexp{apply}: & (\%, K) $\to$ V & extraction of an entry\\
\alexp{entries}: & \% $\to$ \altype{Generator} V & iterate through the entries\\
\alexp{find}: & (K, \%) $\to$ \altype{Partial} V & search for an entry\\
\alexp{keys}: & \% $\to$ \altype{Generator} K & iterate through the keys\\
\alexp{numberOfEntries}: & \% $\to$ \altype{MachineInteger}& number of entries\\
\alexp{set!}: & (\%, K, V) $\to$ V & modification of an entry\\
\alexp{table}: & () $\to$ \% & creation of a table\\
               & \altype{MachineInteger} $\to$ \% & \\
\end{exports}
\begin{exports}
[if $K$ has \altype{InputType} and $V$ has \altype{InputType} then]
\category{\altype{InputType}}\\
\end{exports}
\begin{exports}
[if $K$ has \altype{OutputType} and $V$ has \altype{OutputType} then]
\category{\altype{OutputType}}\\
\end{exports}
\begin{exports}
[if $K$ has \altype{SerializableType} and $V$ has \altype{SerializableType}
then]
\category{\altype{SerializableType}}\\
\end{exports}
\begin{exports}
[if $V$ has \altype{PrimitiveType} then]
\category{\altype{PrimitiveType}}\\
\end{exports}
#endif

define TableType(K:PrimitiveType, V:Type): Category ==
	BoundedFiniteDataStructureType Cross(K, V) with {
	if K has InputType and V has InputType then InputType;
	if K has OutputType and V has OutputType then OutputType;
	if K has SerializableType and V has SerializableType then
							SerializableType;
	if V has PrimitiveType then PrimitiveType;
	bracket: Tuple Cross(K, V) -> %;
	bracket: Generator Cross(K, V) -> %;
#if ALDOC
\alpage{[]}
\Usage{[$(k_1, v_1),\dots,(k_n, v_n)$]}
\Signature{\builtin{Tuple} \builtin{Cross}(K, V)}{\%}
\Params{
$k_1,\dots,k_n$ & K & keys\\
$v_1,\dots,v_n$ & V & values\\
}
\Retval{Returns the table $[k_1 = t_1,\dots,k_n = v_n]$.}
\alseealso{\alexp{table}}
#endif
	apply: (%, K) -> V;
#if ALDOC
\alpage{apply}
\Usage{\name(t, k)\\ t.k}
\Signature{(\%, K)}{V}
\Params{
{\em t} & \% & a table\\
{\em k} & K & a key\\
}
\Retval{Returns the element of {\em t} with key {\em k},
which must be present in the table.}
\Remarks{Produces an error if {\em k} is not in {\em t}, use
\alexp{find} if it is not known whether {\em k} is present in table.}
#endif
	entries: % -> Generator V;
	keys: % -> Generator K;
#if ALDOC
\alpage{entries, keys}
\altarget{entries}
\altarget{keys}
\Usage{
for v in entries t repeat \{\dots \}\\
for k in keys t repeat \{\dots \}}
\Signatures{
entries: \% $\to$ \altype{Generator} V\\
keys: \% $\to$ \altype{Generator} K\\
}
\Params{{\em t} & \% & a table\\}
\Descr{These generators yield respectively all the entries,
or keys in the table {\em t}.}
#endif
	find: (K, %) -> Partial V;
#if ALDOC
\alpage{find}
\Usage{\name(k, t)}
\Signature{(K, \%)}{\altype{Partial} V}
\Params{
{\em k} & K & a key\\
{\em t} & \% & a table\\
}
\Retval{Returns \failed if there is no element with key {\em k} in {\em t},
the element of {\em t} with key {\em k} otherwise.}
\alseealso{\alexp{apply}}
#endif
	numberOfEntries: % -> Z;
#if ALDOC
\alpage{numberOfEntries}
\Usage{\name~t}
\Params{{\em t} & \% & a table\\}
\Retval{Returns the actual number of entries in the table {\em t}.
That number can be different from the size of the table.}
#endif
	set!: (%, K, V) -> V;
#if ALDOC
\alpage{set!}
\Usage{\name(t, k, v)\\ t.k := v; }
\Signature{(\%, K, V)}{V}
\Params{
{\em t} & \% & a table\\
{\em k} & K & a key\\
{\em v} & V & an entry\\
}
\Retval{Sets the element of {\em t} with key {\em k} to {\em v}
and returns {\em v}.}
#endif
	table: () -> %;
	table: Z -> %;
#if ALDOC
\alpage{table}
\Usage{\name()\\ \name~n}
\Signature{\altype{MachineInteger}}{\%}
\Params{
{\em n} & \altype{MachineInteger} & a starting size (optional)\\
}
\Retval{Returns an empty table with initial space for {\em n} entries.
That space grows when needed as elements are inserted in the table.}
\alseealso{\alexp{[]}}
#endif
#if ALDOC
\alpage{bracket}
\Usage{\name()\\ \name~n}
\Signature{\altype{Generator} \altype{Cross}(K, V)}{\%}
\Params{{\em g} & \% & a generator of key-value pairs\\}
}
\Retval{Returns a new table containing the specified pairs}
\alseealso{\alexp{[]}}
#endif
        bracket: Generator Cross(K, V) -> %;

	default {
		local leftBracket:Ch	== { import from String; char "[" }
		local rightBracket:Ch	== { import from String; char "]" }
		local comma:Ch		== { import from String; char "," }

		[tp:Tuple Cross(K, V)]:% == {
			import from Z;
			t := table(n := length tp);
			for i in 1..n repeat {
				(k, v) := element(tp, i);
				t.k := v;
			}
			t;
		}

		[g: Generator Cross(K, V)]: % == {
		        t := table();
			for (k, v) in g repeat {
			    t.k := v;
			}
			t
		}

		apply(t:%, k:K):V == {
			import from Partial V;
			retract find(k, t);
		}

		keys(t:%):Generator K == generate {
			for pair in t repeat {
				(k, e) := pair;
				yield k;
			}
		}

		entries(t:%):Generator V == generate {
			for pair in t repeat {
				(k, e) := pair;
				yield e;
			}
		}

		if K has SerializableType and V has SerializableType then {
			(p:BinaryWriter) << (t:%):BinaryWriter == {
				import from Z, K, V;
				p := p << #t << numberOfEntries t;
				for pair in t repeat {
					(k, e) := pair;
					p := p << k << e;
				}
				p;
			}

			<< (p:BinaryReader):% == {
				import from Z, K, V;
				local k:K;
				t := table(<< p);
				n:Z := << p;
				for i in 1..n repeat { k := << p; t.k := << p }
				t;
			}
		}

		if K has InputType and V has InputType then {
			<< (p:TextReader):% == {
				import from Ch, Z, K, V;
				local c:Ch;
				local k:K;
				while space?(c:= << p) or c = newline repeat {};
				c ~= leftBracket => throw SyntaxException;
				t := table(<< p);
				while space?(c:= << p) or c = newline repeat {};
				while c = comma repeat {
					while space?(c := << p) repeat {};
					c~=leftBracket => throw SyntaxException;
					k := << p;
					while space?(c := << p)
						or c = newline repeat {};
					c ~= comma => throw SyntaxException;
					t.k := << p;
					while space?(c := << p)
						or c = newline repeat {};
					c~=rightBracket=> throw SyntaxException;
					while space?(c := << p)
						or c = newline repeat {};
				}
				c = rightBracket => t;
				push!(c, p);
				throw SyntaxException;
			}
		}

		if K has OutputType and V has OutputType then {
			(p:TextWriter) << (t:%):TextWriter == {
				import from Boolean, Ch, Z, K, V;
				p := p << leftBracket << #t;
				for pair in t repeat {
					(k, e) := pair;
					p := p << comma << leftBracket << k;
					p := p << comma << e << rightBracket;
				}
				p << rightBracket;
			}
		}
		if V has PrimitiveType then {
		    (a: %) = (b: %): Boolean == {
		        import from BooleanFold;
			import from Partial V;
			import from K, V, MachineInteger;
			check(k: K): Boolean == {
			    failed? find(k, b) => false;
			    a.k = b.k
			}
		        numberOfEntries a = numberOfEntries b and (_and)/(check k for k in keys a)
		    }
		}
	}
}

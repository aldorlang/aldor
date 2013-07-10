---------------------- ald_symtab.as -------------------------------
--
-- Symbol tables (e.g. for read-eval loops)
--
-- Copyright (c) Manuel Bronstein 1995
-- Copyright (c) INRIA 1999, Version 0.1.12
-- Logiciel Sum^it (c) INRIA 1999, dans sa version 0.1.12
-- Copyright (c) Swiss Federal Polytechnic Institute Zurich, 1995-97
-----------------------------------------------------------------------------

#include "aldor"

#if ALDOC
\thistype{SymbolTable}
\History{Manuel Bronstein}{10/01/96}{created}
\Usage{import from \this~T}
\Params{ T & \altype{PrimitiveType} & Type of the symbols in the table\\ }
\Descr{\this~T provides symbol tables where all the symbols have type T.}
\begin{exports}
\alexp{apply}:
& (\%, \altype{Symbol}) $\to$ \altype{Partial} T & Search for a symbol\\
\alexp{set!}: & (\%, \altype{Symbol}, T) $\to$ T & Add a symbol\\
\alexp{table}: & () $\to$ \% & Create an empty table\\
\end{exports}
#endif

SymbolTable(T:PrimitiveType):with {
	apply: (%, Symbol) -> Partial T;
#if ALDOC
\alpage{apply}
\Usage{ \name(t, x)\\t~x }
\Signature{(\%, \altype{Symbol})}{\altype{Partial} T}
\Params{
{\em t} & \% & A symbol table\\
{\em x} & \altype{Symbol} & A variable name\\
}
\Retval{Returns the value that x has in t if it is found, \failed otherwise.}
#endif
	set!: (%, Symbol, T) -> T;
#if ALDOC
\alpage{set!}
\Usage{ \name(t, x, v)\\t.x := v }
\Signature{(\%, \altype{Symbol}, T)}{T}
\Params{
{\em t} & \% & A symbol table\\
{\em x} & \altype{Symbol} & A variable name\\
{\em v} & T & A value\\
}
\Descr{Assigns the value v to x in t. If x already had a value in t, the
older value is lost.}
\Retval{Returns v.}
#endif
	table: () -> %;
#if ALDOC
\alpage{table}
\Usage{\name()}
\Signature{()}{\%}
\Retval{Returns an empty symbol table.}
\alseealso{\alexp{set!}}
#endif
} == add {
	Rep == HashTable(Symbol, T);
	import from Rep;

	apply(t:%, key:Symbol):Partial T	== find(key, rep t);
	set!(t:%, key:Symbol, v:T):T		== set!(rep t, key, v);
	table():%				== per table();
}

-- Those are documented in the corresponding types
-- Those hash-tables make those types non-reentrant
macro OPT == SymbolTable MachineInteger;
extend TextWriter: with { options:OPT } == add { options:OPT == table(); }
extend TextReader: with { options:OPT } == add { options:OPT == table(); }
extend BinaryWriter: with { options:OPT } == add { options:OPT == table(); }
extend BinaryReader: with { options:OPT } == add { options:OPT == table(); }


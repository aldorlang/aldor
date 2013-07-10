---------------------------- sal_bstruc.as ----------------------------------
--
-- This file defines bounded finite linear structures
--
-- Copyright (c) Manuel Bronstein 1998
-- Copyright (c) INRIA 1998, Version 29-10-98
-- Logiciel Salli ©INRIA 1998, dans sa version du 29/10/1998
-----------------------------------------------------------------------------

#include "aldor"

macro Z == MachineInteger;

#if ALDOC
\thistype{BoundedFiniteLinearStructureType}
\History{Manuel Bronstein}{16/10/98}{created}
\Usage{\this~T: Category}
\Params{{\em T} & \builtin{Type} & the type of the entries\\}
\Descr{\this~is the category of finite linear structures whose entries are
of type {\em T} and whose size is always known.}
\begin{exports}
\category{\altype{BoundedFiniteDataStructureType} T}\\
\category{\altype{FiniteLinearStructureType} T}\\
\alexp{map}: & (T $\to$ T) $\to$ \% $\to$ \% & lift a mapping\\
\alexp{map!}: & (T $\to$ T) $\to$ \% $\to$ \% & lift a mapping\\
\end{exports}
\begin{exports}[if $T$ has \altype{PrimitiveType} then]
\category{\altype{PrimitiveType}}\\
\alexp{linearSearch}:
& (T, \%) $\to$ (\altype{Boolean}, \altype{MachineInteger}, T) & linear search\\
& (T, \%, \altype{MachineInteger})
$\to$ (\altype{Boolean}, \altype{MachineInteger}, T) & \\
\end{exports}
\begin{exports}[if $T$ has \altype{InputType} then]
\category{\altype{InputType}}\\
\end{exports}
\begin{exports}[if $T$ has \altype{SerializableType} then]
\category{\altype{SerializableType}}\\
\end{exports}
#endif

define BoundedFiniteLinearStructureType(T:Type):Category ==
	Join(BoundedFiniteDataStructureType T,FiniteLinearStructureType T) with{
	if T has InputType then InputType;
	if T has PrimitiveType then PrimitiveType;
	if T has SerializableType then SerializableType;
	if T has PrimitiveType then {
		linearSearch: (T, %) -> (Boolean, Z, T);
		linearSearch: (T, %, Z) -> (Boolean, Z, T);
#if ALDOC
\alpage{linearSearch}
\Usage{\name(t, a)\\ \name(t, a, n)}
\Signature{(T, \%, \altype{MachineInteger})}
{\altype{Boolean}, \altype{MachineInteger}, T}
\Params{
{\em t} & T & the value to search for\\
{\em a} & \% & a finite linear structure\\
{\em n} & \altype{MachineInteger} & an initial index (optional)\\
}
\Retval{Returns (found?, i, a.i) such that $t = a.i$ if found?~is \true,
$t$ is not in $a$ otherwise. If the optional argument {\em n} is present,
then the search starts at the entry {\em a.n} and ignores the previous
ones.}
\alseealso{\alfunc{BoundedFiniteDataStructureType}{findAll}}
#endif
	}
	map: (T -> T) -> % -> %;
	map!: (T -> T) -> % -> %;
#if ALDOC
\alpage{map}
\astarget{\name!}
\Usage{\name~f\\\name!~f\\\name(f)(a)\\\name!(f)(a)}
\Signature{(T $\to$ T) $\to$ \%}{\%}
\Params{
{\em f} & T $\to$ T & a map\\
{\em a} & \% & a finite linear structure\\
}
\Retval{\name(f)(a) returns the new structure {\tt [f(x) for x in a]}, while
\name(f) returns the mapping $a \to$ {\tt [f(x) for x in a]}. In both cases,
\name!~does not make a copy of the structure a but modifies it in place.}
#endif
	default {
		if T has PrimitiveType then {
			linearSearch(t:T, a:%):(Boolean, Z, T) ==
				linearSearch(t, a, firstIndex);

			(a:%) = (b:%):Boolean == {
				import from Z, T;
				#a ~= #b => false;
				for x in a for y in b repeat
					x ~= y => return false;
				true;
			}

			equal?(a:%, b:%, n:Z):Boolean == {
				import from T;
				for x in a for y in b for i in 1..n repeat
					x ~= y => return false;
				true;
			}

		}

		if T has SerializableType then {
			(p:BinaryWriter) << (a:%):BinaryWriter == {
				import from Z, T;
				p := p << #a;		-- write size first
				for t in a repeat p := p << t;
				p;
			}
		}
	}
}

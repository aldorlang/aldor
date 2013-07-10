---------------------------- sal_bdata.as ----------------------------------
--
-- This file defines bounded finite data structures
--
-- Copyright (c) Manuel Bronstein 1998
-- Copyright (c) INRIA 1998, Version 29-10-98
-- Logiciel Salli ©INRIA 1998, dans sa version du 29/10/1998
-----------------------------------------------------------------------------

#include "aldor"

macro Z == MachineInteger;

#if ALDOC
\thistype{BoundedFiniteDataStructureType}
\History{Manuel Bronstein}{16/10/98}{created}
\Usage{\this~T: Category}
\Params{{\em T} & \builtin{Type} & the type of the entries\\}
\Descr{\this~is the category of finite general structures whose entries
are of type {\em T} and whose size is always known.}
\begin{exports}
\category{\altype{CopyableType}}\\
\category{\altype{DataStructureType}}\\
\alalias{\this}{size}{\#}:
& \% $\to$ \altype{MachineInteger} & number of entries\\
\alexp{generator}:
& \% $\to$ \altype{Generator} T & iteration over a structure\\
\end{exports}
\begin{exports}[if $T$ has \altype{PrimitiveType} then]
\alexp{findAll}: & (T, \%) $\to$
\altype{Generator} \builtin{Cross}(\altype{MachineInteger}, T) & linear search\\
\alexp{member?}: & (T, \%) $\to$ \altype{Boolean} & look for a value\\
\end{exports}
\begin{exports}[if $T$ has \altype{HashType} then]
\category{\altype{HashType}}\\
\end{exports}
\begin{exports}[if $T$ has \altype{OutputType} then]
\category{\altype{OutputType}}\\
\end{exports}
#endif

define BoundedFiniteDataStructureType(T:Type):Category ==
	Join(CopyableType, DataStructureType) with {
	if T has HashType then HashType;
	if T has OutputType then OutputType;
	#: % -> Z;
#if ALDOC
\alpage{size}
\Usage{\#~a}
\Signatures{ \#: & \% $\to$ \altype{MachineInteger}\\ }
\Params{ {\em a} & \% & a finite data structure\\ }
\Retval{Returns the number of entries in the structure $a$.}
#endif
	if T has PrimitiveType then {
		findAll: (T, %) -> Generator Cross(Z, T);
#if ALDOC
\alpage{findAll}
\Usage{for pair in \name(t, a) repeat \{ (pos, val) := pair; \dots \}}
\Signature{(T, \%)}
{\altype{Generator} \builtin{Cross}(\altype{MachineInteger}, T)}
\Params{
{\em t} & T & the value to search for\\
{\em a} & \% & a finite data structure\\
}
\Descr{Iterates trough all pairs $(i, x)$ such that $t = x$
(using the equality of the type {\em T}). The index {\em i} is
the position of {\em x} in the iteration of {\em t} by the function
\alexp{generator}: $i = 1$ means {\em x} is the first element generated,
$i = 2$ means {\em x} is the second element generated, etc.}
\alseealso{\alexp{member?}}
#endif
	}
	generator: % -> Generator T;
#if ALDOC
\alpage{generator}
\Usage{ for x in a repeat \{ \dots \}\\ for x in \name~a repeat \{ \dots \} }
\Signature{\%}{\altype{Generator} T}
\Params{ {\em a} & \% & a finite data structure\\ }
\Descr{This function allows a structure to be iterated independently of its
representation. This generator yields the elements of {\em a} in
some order, which is determined by the actual type.}
\begin{asex}
The following code computes the sum of all the elements
of an array of machine integers:
\begin{ttyout}
sum(a:Array MachineInteger, n:MachineInteger):MachineInteger == {
    s:MachineInteger := 0;
    for x in a repeat s := s + x;
    s;
}
\end{ttyout}
\end{asex}
#endif
	if T has PrimitiveType then {
		member?: (T, %) -> Boolean;
#if ALDOC
\alpage{member?}
\Usage{\name(t, a)}
\Signature{(T, \%)}{\altype{Boolean}}
\Params{
{\em t} & T & the value to search for\\
{\em a} & \% & a finite data structure\\
}
\Retval{Returns \true if t is a member of a, \false otherwise.}
#endif
	}
	default {
		if T has PrimitiveType then {
			findAll(t:T, a:%):Generator Cross(Z, T) == generate {
				import from Z;
				for x in a for i in 1.. repeat {
					if x = t then yield (i, x);
				}
			}

			member?(t:T, a:%):Boolean == {
				for x in a repeat { x = t => return true }
				false;
			}
		}

		if T has HashType then {
			hash(a:%):Z == {
				import from T;
				h:Z := 0;
				for x in a for i in 1.. repeat
					h := h + i * hash x;
				h;
			}
		}
	}
}


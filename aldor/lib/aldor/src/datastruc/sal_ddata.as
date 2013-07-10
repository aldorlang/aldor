---------------------------- sal_ddata.as ----------------------------------
--
-- This file defines dynamic bounded finite data structures
--
-- Copyright (c) Manuel Bronstein 1998
-- Copyright (c) INRIA 1998, Version 29-10-98
-- Logiciel Salli ©INRIA 1998, dans sa version du 29/10/1998
-----------------------------------------------------------------------------

#include "aldor"

macro Z == MachineInteger;

#if ALDOC
\thistype{DynamicDataStructureType}
\History{Manuel Bronstein}{9/7/2001}{created}
\Usage{\this~T: Category}
\Params{{\em T} & \builtin{Type} & the type of the entries\\}
\Descr{\this~is the category of finite general structures in which
entries of type {\em T} can be inserted or removed dynamically.}
\begin{exports}
\category{\altype{BoundedFiniteDataStructureType} T}\\
\alexp{insert}: & (T, \%) $\to$ \% & add an element\\
\alexp{insert!}: & (T, \%) $\to$ \% & add an element\\
\end{exports}
\begin{exports}[if {\em T} has \altype{PrimitiveType} then]
\alexp{remove}: & (T, \%) $\to$ \% & remove an element\\
\alexp{remove!}: & (T, \%) $\to$ \% & remove an element\\
\alexp{removeAll}: & (T, \%) $\to$ \% & remove all occurrences of an element\\
\alexp{removeAll!}: & (T, \%) $\to$ \% & remove all occurrences of an element\\
\end{exports}
#endif

define DynamicDataStructureType(T:Type):Category ==
	BoundedFiniteDataStructureType T with {
	insert: (T, %) -> %;
	insert!: (T, %) -> %;
#if ALDOC
\alpage{insert}
\altarget{\name!}
\Usage{\name(t, a)\\ \name!(t, a)}
\Signature{(T, \%)}{\%}
\Params{
{\em t} & T & an element to add\\
{\em a} & \% & a dynamic data structure\\
}
\Descr{Adds {\em t} to {\em a} and returns the new structure.
\name creates a new structure while \name! modifies {\em a} itself.}
#endif
	if T has PrimitiveType then {
		remove: (T, %) -> %;
		remove!: (T, %) -> %;
		removeAll: (T, %) -> %;
		removeAll!: (T, %) -> %;
#if ALDOC
\alpage{remove,removeAll}
\altarget{remove}
\altarget{remove!}
\altarget{removeAll}
\altarget{removeAll!}
\Usage{remove(t, a)\\ remove!(t, a)\\ removeAll(t, a)\\ removeAll!(t, a)}
\Params{
{\em t} & T & an element to remove\\
{\em a} & \% & a dynamic data structure\\
}
\Descr{remove(t, a) and remove!(t, a) remove the first occurence of {\em t}
in {\em a} and return the new structure, while removeAll(t, a) and
removeAll!(t, a) remove all the occurences of {\em t} in {\em a}.
remove and removeAll create a new structure, while remove! and removeAll!
modify {\em a} itself.}
#endif
	}
	default {
		insert!(t:T, a:%):% == insert(t, a);

		if T has PrimitiveType then {
			remove!(t:T, a:%):%	== remove(t, a);
			removeAll!(t:T, a:%):%	== removeAll(t, a);
		}
	}
}


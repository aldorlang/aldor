---------------------------- sal_fstruc.as ----------------------------------
--
-- This file defines finite linear structures
--
-- Copyright (c) Manuel Bronstein 1998
-- Copyright (c) INRIA 1998, Version 29-10-98
-- Logiciel Salli ©INRIA 1998, dans sa version du 29/10/1998
-----------------------------------------------------------------------------

#include "aldor"

#if ALDOC
\thistype{FiniteLinearStructureType}
\History{Manuel Bronstein}{16/10/98}{created}
\Usage{\this~T: Category}
\Params{{\em T} & Type & the type of the entries\\}
\Descr{\this~is the category of finite linear structures whose entries are
of type T.}
\begin{exports}
\category{\altype{LinearStructureType} T}\\
\alexp{[]}: & \builtin{Tuple} T $\to$ \% & construction of a structure\\
\alexp{empty}: & \%  & empty structure\\
\alexp{new}: & (\altype{MachineInteger}, T) $\to$ \% & creation of a structure\\
\end{exports}
#endif

define FiniteLinearStructureType(T:Type):Category == LinearStructureType T with{
	bracket: Tuple T -> %;
#if ALDOC
\alpage{[]}
\Usage{[$t_1,\dots,t_n$]}
\Signature{\builtin{Tuple} T}{\%}
\Params{ $t_1,\dots,t_n$ & T & elements of T\\ }
\Retval{Returns the structure $[t_1,\dots,t_n]$.}
#endif
	empty: %;
#if ALDOC
\alpage{empty}
\Usage{\name}
\alconstant{\%}
\Retval{Returns an empty structure.}
\alseealso{\alfunc{DataStructureType}{empty?}}
#endif
	new: (MachineInteger, T) -> %;
#if ALDOC
\alpage{new}
\Usage{\name(n, x)}
\Signature{(\altype{MachineInteger}, T)}{\%}
\Params{
{\em n} & \altype{MachineInteger} & a nonnegative size\\
{\em x} & T & an entry\\
}
\Retval{Returns a structure of $n$ entries, all of them set to $x$.}
#endif
}

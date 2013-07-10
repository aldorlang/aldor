----------------------------- sal_torder.as ----------------------------------
--
-- This file defines totally ordered types
--
-- Copyright (c) Manuel Bronstein 1998
-- Copyright (c) INRIA 1998, Version 29-10-98
-- Logiciel Salli (c) INRIA 1998, dans sa version du 29/10/1998
-----------------------------------------------------------------------------

#include "aldor"

#if ALDOC
\thistype{TotallyOrderedType}
\History{Manuel Bronstein}{28/9/98}{created}
\Usage{\this: Category}
\Descr{\this~is the category of totally ordered types, \ie~partially
ordered types where every pair $x\ne y$ is comparable.}
\begin{exports}
\category{\altype{PartiallyOrderedType}}\\
\alexp{compare}: & (\%,\%) $\to$ \altype{MachineInteger} & comparison\\
\alexp{max}: & (\%,\%) $\to$ \% & greater element\\
\alexp{min}: & (\%,\%) $\to$ \% & smaller element\\
\end{exports}
#endif

define TotallyOrderedType:Category == PartiallyOrderedType with {
	compare: (%, %) -> MachineInteger;
#if ALDOC
\alpage{compare}
\Signature{(\%,\%)}{\altype{MachineInteger}}
\Usage{\name(a,b)}
\Params{ {\em a, b} & \% & elements of the type\\}
\Retval{\name(a,b) returns $n < 0$ if $a < b$, $n = 0$ if $a = b$
and $n > 0$ if $a > b$.}
#endif
	max: (%, %) -> %;
	min: (%, %) -> %;
#if ALDOC
\alpage{max,min}
\altarget{max}
\altarget{min}
\Usage{max(a,b)\\min(a,b)}
\Signature{(\%,\%)}{\%}
\Params{ {\em a, b} & \% & elements of the type\\}
\Retval{max(a,b) and min(a,b) respectively the largest and the smallest
among a and b.}
#endif
	default {
		(a:%) > (b:%):Boolean	== ~(a <= b);
		(a:%) >= (b:%):Boolean	== ~(a < b);
		max(a:%, b:%):%		== { a < b => b; a };
		min(a:%, b:%):%		== { a < b => a; b };

		compare(a:%, b:%):MachineInteger == {
			a = b => 0;
			a < b => 0-1;
			1;
		}
	}
}


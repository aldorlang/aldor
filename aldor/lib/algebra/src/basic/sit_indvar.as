------------------------------- sit_indvar.as ----------------------------------
--
-- Indexed variables, e.g. x_1, x_2, ...
--
-- Copyright (c) Manuel Bronstein 2000
-- Copyright (c) INRIA 2000, Version 0.1.12
-- Logiciel Sum^it ©INRIA 2000, dans sa version 0.1.12
-----------------------------------------------------------------------------

#include "algebra"

#if ALDOC
\thistype{IndexedVariable}
\History{Manuel Bronstein}{10/3/2000}{created}
\Usage{import from \this~S\\ import from \this(S, x)}
\Params{
{\em S} & \astype{ExpressionType} & The type of the indices\\
        & \astype{TotallyOrderedType} & \\
{\em x} & \astype{Symbol} & The root variable (optional)\\
}
\Descr{\this~provides sorted symbols of the form $x_s$ where $s \in S$.}
\begin{exports}
\category{\astype{ExpressionType}}\\
\category{\astype{TotallyOrderedType}}\\
\asexp{index}: & \% $\to$ S & Index of a variable\\
\asexp{variable}: & S $\to$ \% & Create an indexed variable\\
\end{exports}
#endif

IndexedVariable(S:Join(ExpressionType, TotallyOrderedType), x:Symbol == new()):
	Join(ExpressionType, TotallyOrderedType) with {
		index: % -> S;
#if ALDOC
\aspage{index}
\Usage{\name~v}
\Signature{\%}{S}
\Params{{\em v} & \% & An indexed variable\\ }
\Retval{Returns the index of $v$.}
#endif
		variable: S -> %;
#if ALDOC
\aspage{variable}
\Usage{\name~s}
\Signature{S}{\%}
\Params{{\em s} & S & An index\\ }
\Retval{Returns the variable $x_s$.}
#endif
} == S add {
	Rep == S;

	index(v:%):S			== rep v;
	variable(s:S):%			== per s;
	local xtree:ExpressionTree	== extree x;

	extree(v:%):ExpressionTree == {
		import from S, List ExpressionTree;
		ExpressionTreeSubscript [xtree, extree index v];
	}

-- TEMPORARY: DEFAULT NOT FOUND OTHERWISE (1.1.12p4)
	(port:TextWriter) << (a:%):TextWriter == {
		import from ExpressionTree;
		tex(port, extree a);
	}
}

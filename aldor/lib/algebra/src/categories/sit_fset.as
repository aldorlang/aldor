------------------------------ sit_fset.as ------------------------------
-- Copyright (c) Manuel Bronstein 1997
-- Copyright (c) INRIA 1999, Version 0.1.12
-- Logiciel Sum^it (c) INRIA 1999, dans sa version 0.1.12
-- Copyright (c) Swiss Federal Polytechnic Institute Zurich, 1997
-----------------------------------------------------------------------------

#include "algebra"

#if ALDOC
\thistype{FiniteSet}
\History{Manuel Bronstein}{17/2/97}{created}
\Usage{\this: Category}
\Descr{\this~is the category of finite sets.}
\begin{exports}
\category{\astype{ExpressionType}}\\
\category{\astype{TotallyOrderedType}}\\
\alalias{\this}{size}{\#}: & \astype{Integer} & number of elements\\
\alexp{apply}: & (\%, \astype{ExpressionTree}) $\to$ \astype{ExpressionTree} &
Conversion to an expression tree\\
\alexp{index}: & \% $\to$ \astype{Integer} & Index of an element\\
\alexp{lookup}: & \astype{Integer} $\to$ \% & Element with a given index\\
\alexp{random}: & () $\to$ \% & Get a random element\\
\end{exports}
#endif

macro {
	Z	== Integer;
	TREE	== ExpressionTree;
	anon	== (-"\Box");
}

define FiniteSet:Category == Join(ExpressionType, TotallyOrderedType) with {
	#: Z;
#if ALDOC
\alpage{size}
\Usage{\#}
\Signatures{\#: & \astype{Integer}\\}
\Retval{Returns the number of elements of the type.}
#endif
	apply: (%, TREE) -> TREE;
#if ALDOC
\alpage{apply}
\Usage{ \name(p, x)\\p~x }
\Signature{(\%, \astype{ExpressionTree})}{\astype{ExpressionTree}}
\Params{
{\em p} & \% & An element\\
{\em x} & \astype{ExpressionTree} & A name for the variables\\
}
\Retval{Returns p as an expression tree, using x as root variable name.}
#endif
	index: % -> Z;
#if ALDOC
\alpage{index}
\Usage{ \name~p }
\Signature{\%}{\astype{Integer}}
\Params{ {\em p} & \% & An element\\ }
\Retval{Returns the index of $p$.}
\alseealso{lookup(\this)}
#endif
	lookup: Z -> %;
#if ALDOC
\alpage{lookup}
\Usage{ \name~j }
\Signature{\astype{Integer}}{\%}
\Params{ {\em j} & \astype{Integer} & An index \\ }
\Retval{Returns the element with index $j$.}
\alseealso{index(\this)}
#endif
	random: () -> %;
#if ALDOC
\alpage{random}
\Usage{\name()}
\Signature{()}{\%}
\Retval{Returns a random element.}
#endif
	default {
		extree(x:%):TREE== { import from String,Symbol; x extree anon; }
		(x:%) < (y:%):Boolean	== { import from Z; index x < index y; }

		apply(x:%, t:TREE):TREE == {
			import from Z, List TREE;
			ExpressionTreeSubscript [t, extree index x];
		}

		random():% == {
			import from Z;
			lookup next(random()$Z mod #$%);
		}
	}
}

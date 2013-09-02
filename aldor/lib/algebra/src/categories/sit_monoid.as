------------------------------- sit_monoid.as ----------------------------------
-- Copyright (c) Manuel Bronstein 1999
-- Copyright (c) INRIA 1999, Version 0.1.12
-- Logiciel Sum^it (c) INRIA 1999, dans sa version 0.1.12
-----------------------------------------------------------------------------

#include "algebra"

#if ALDOC
\thistype{Monoid}
\History{Manuel Bronstein}{24/3/99}{created}
\Usage{\this: Category}
\Descr{\this~is the category of monoids, not necessarily commutative.}
\begin{exports}
\category{\altype{ExpressionType}}\\
\alexp{1}: & \% & one\\
\alexp{*}: & (\%, \%) $\to$ \% & product\\
\alalias{\this}{**}{$\land$}:
& (\%, \altype{Integer}) $\to$ \% & exponentiation\\
\alexp{one?}: & \% $\to$ \altype{Boolean} & test for $1$\\
\alexp{times!}: & (\%, \%) $\to$ \% & In--place product\\
\end{exports}
#endif

define Monoid: Category == ExpressionType with {
        1: %;
#if ALDOC
\alpage{1}
\Usage{\name}
\alconstant{\%}
\Retval{Returns the constant $1$.}
#endif
	*: (%, %) -> %;
#if ALDOC
\alpage{*}
\Usage{$x$ \name $y$}
\Signature{(\%,\%)}{\%}
\Params{{\em x,y} & \% & elements of the monoid\\ }
\Retval{Returns the product $x y$.}
#endif
	^: (%, Integer) -> %;
#if ALDOC
\alpage{**}
\Usage{$x\land n$}
\Signatures{$\land$: (\%,\altype{Integer}) $\to$ \%}
\Params{
{\em x} & \% & an element of the monoid\\
{\em n} & \altype{Integer} & an exponent\\
}
\Retval{Returns $x^n$.}
#endif
	one?: % -> Boolean;
#if ALDOC
\alpage{one?}
\Usage{\name~x}
\Signature{\%}{\altype{Boolean}}
\Params{ {\em x} & \% & An element of the monoid\\ }
\Retval{Returns \true~if $x = 1$, \false~otherwise.}
#endif
        times!: (%, %) -> %;
#if ALDOC
\alpage{times!}
\Usage{\name(x, y)}
\Signature{(\%, \%)}{\%}
\Params{
{\em x} & \% & An element of the monoid (to be destroyed)\\
{\em y} & \% & An element of the monoid to be multiplied by x\\
}
\Retval{Returns the product $x y$, where the storage used by $x$ is allowed
to be destroyed or reused, so $x$ can be lost after this call.}
\Remarks{This function may cause $x$ to be destroyed, so do not use it unless
$x$ has been locally allocated, and is guaranteed not to share space
with other elements. Some functions are not necessarily copying their
arguments and can thus create memory aliases.}
#endif
	default {
		macro copy?	== % has CopyableType;
		one?(a:%):Boolean	== a = 1;

		times!(a:%, b:%):% == {
			one? a => { copy?=>copy(b)$(% pretend CopyableType); b }
			a * b;
		}
	}
}

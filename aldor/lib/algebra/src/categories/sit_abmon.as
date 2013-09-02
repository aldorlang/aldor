------------------------------- sit_abmon.as ----------------------------------
-- Copyright (c) Manuel Bronstein 1999
-- Copyright (c) INRIA 1999, Version 0.1.12
-- Logiciel Sum^it (c) INRIA 1999, dans sa version 0.1.12
-----------------------------------------------------------------------------

#include "algebra"

#if ALDOC
\thistype{AbelianMonoid}
\History{Manuel Bronstein}{24/3/99}{created}
\Usage{\this: Category}
\Descr{\this~is the category of commutative monoids.}
\begin{exports}
\category{\altype{ExpressionType}}\\
\alexp{$0$}: & \% & zero\\
\alexp{$+$}: & (\%, \%) $\to$ \% & sum\\
\alexp{$*$}: & (\altype{Integer}, \%) $\to$ \% & product by an integer\\
\alexp{add!}: & (\%, \%) $\to$ \% & In--place sum\\
\alexp{zero?}: & \% $\to$ \altype{Boolean} & test for $0$\\
\end{exports}
#endif

define AbelianMonoid: Category == ExpressionType with {
	0: %;
#if ALDOC
\alpage{$0$}
\Usage{\name}
\alconstant{\%}
\Retval{Returns the constant $0$.}
#endif
        +: (%, %) -> %;
#if ALDOC
\alpage{+}
\Usage{$x \name y$}
\Signature{(\%,\%)}{\%}
\Params{{\em x,y} & \% & elements of the monoid\\ }
\Retval{Returns the sum $x+y$.}
#endif
	*: (Integer, %) -> %;
#if ALDOC
\alpage{$*$}
\Usage{n \name~x}
\Signature{(Integer, \%)}{\%}
\Params{
{\em n} & Integer & An integer\\
{\em x} & \% & An element of the monoid to be multiplied by n\\
}
\Retval{Returns the product $n x$.}
#endif
	add!: (%, %) -> %;
#if ALDOC
\alpage{add!}
\Usage{\name(x, y)}
\Signature{(\%, \%)}{\%}
\Params{
{\em x} & \% & An element of the monoid (to be destroyed)\\
{\em y} & \% & An element of the monoid to be added to x\\
}
\Retval{Returns the sum $x + y$, where the storage used by x is allowed
to be destroyed or reused, so x is lost after this call.}
\Remarks{This function may cause x to be destroyed, so do not use it unless
x has been locally allocated, and is guaranteed not to share space
with other elements. Some functions are not necessarily copying their
arguments and can thus create memory aliases.}
#endif
	zero?: % -> Boolean;
#if ALDOC
\alpage{zero?}
\Usage{\name~x}
\Signature{\%}{\altype{Boolean}}
\Params{{\em x} & \% & an element of the monoid\\ }
\Retval{Returns the result of $x = 0$ using the semantics of $=$ of the monoid.}
#endif
	default {
		zero?(x:%):Boolean	== x = 0;

		add!(a:%, b:%):% == {
			zero? a => { % has CopyableType =>copy(b);b }
			a + b;
		}
	}
}

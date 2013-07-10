-------------------------- sal_lincomb.as ----------------------------------
-- Copyright (c) Manuel Bronstein 1997
-- Copyright (c) INRIA 1999, Version 0.1.12
-- Logiciel Salli (c) INRIA 1998, dans sa version du 29/10/1998
-----------------------------------------------------------------------------

#include "aldor"

#if ALDOC
\thistype{LinearCombinationType}
\History{Manuel Bronstein}{17/2/97}{created}
\Usage{\this~R:Category}
\Params{ {\em R} & \altype{AdditiveType} & The coefficient domain\\ }
\Descr{\this~R is the category of types containing linear
combinations of their elements with coefficients in R.}
\begin{exports}
\category{\altype{AdditiveType}}\\
\alexp{$*$}: & (R, \%) $\to$ \% & Left-multiplication by a scalar\\
\alexp{add!}: & (\%, R, \%) $\to$ \% & In--place product and sum\\
\alexp{times!}: & (R, \%) $\to$ \% & In--place product by a scalar\\
\end{exports}
#endif

define LinearCombinationType(R:AdditiveType): Category == AdditiveType with {
	*: (R, %) -> %;
#if ALDOC
\alpage{$*$}
\Usage{r \name~p}
\Signature{(R, \%)}{\%}
\Params{
{\em r} & R & A scalar\\
{\em p} & \% & An element of the type\\
}
\Retval{Returns the product $r p$.}
\alseealso{\alexp{times!}}
#endif
	add!: (%, R, %) -> %;
#if ALDOC
\alpage{add!}
\Usage{\name(p, r, q)}
\Signature{(\%, R, \%)}{\%}
\Params{
{\em p} & \% & An element of the type (to be destroyed)\\
{\em r} & R & A scalar\\
{\em q} & \% & An element of the type\\
}
\Retval{\name(p, r, q) returns $p + r q$.}
\Remarks{
The storage used by p is allowed
to be destroyed or reused, so p is lost after this call.
This function may cause p to be destroyed, so do not use it unless
p has been locally allocated, and is guaranteed not to share space
with other elements. Some functions are not necessarily copying their
arguments and can thus create memory aliases.}
\alseealso{\alexp{times!}}
#endif
	times!: (R, %) -> %;
#if ALDOC
\alpage{times!}
\Usage{\name(r, p)}
\Signature{(R, \%)}{\%}
\Params{
{\em r} & R & A scalar to be multiplied by p\\
{\em p} & \% & An element of the type (to be destroyed)\\
}
\Retval{Returns the product $r p$.}
\Remarks{The storage used by p is allowed to be destroyed or reused, so p
is lost after this call.
This may cause p to be destroyed, so do not use this unless
p has been locally allocated, and is thus guaranteed not to share space
with other elements. Some functions are
not necessarily copying their arguments and can thus create memory aliases.}
\alseealso{\alexp{$*$},\alexp{add!}}
#endif
	default {
		add!(p:%, c:R, q:%):%	== add!(p, c * q);
		times!(c:R, p:%):%	== c * p;
	}
}

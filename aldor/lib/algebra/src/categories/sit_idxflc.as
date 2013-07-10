----------------------------- sit_idxflc.as ----------------------------------
-- Copyright (c) Manuel Bronstein 2001
-- Copyright (c) INRIA 2001
-----------------------------------------------------------------------------

#include "algebra"

#if ALDOC
\thistype{IndexedFreeLinearCombinationType}
\History{Manuel Bronstein}{26/4/2001}{extracted from UnivariateFreeAlgebra}
\Usage{\this(R, E): Category}
\Params{
{\em R} & \altype{ExpressionType} & The coefficient domain\\
        & \altype{ArithmeticType} &\\
{\em E} & \altype{ExpressionType} & The index domain\\
}
\Descr{\this(R, E) is the category of types containing linear
combinations of their elements with coefficients in {\em R} with
respect to a linearly independent generating set.
Its elements are not assumed to have finite support, so this type
cannot be asserted to be a \altype{Module}~R even when {\em R}
is a \altype{Ring}.}
\begin{exports}
\category{\altype{FreeLinearCombinationType} R}\\
\alexp{add!}: & (\%, R, E) $\to$ \% & In--place addition of a term\\
\alexp{coefficient}: & (\%, E) $\to$ R & Extraction of a coefficient\\
\alexp{monomial}: & E $\to$ \% & Creation of a monic term\\
\alexp{term}: & (R, E) $\to$ \% & Creation of a term\\
\end{exports}
#endif

define IndexedFreeLinearCombinationType(R:Join(ArithmeticType, ExpressionType),
	E: ExpressionType): Category == FreeLinearCombinationType R with {
		add!: (%, R, E) -> %;
#if ALDOC
\alpage{add!}
\Usage{\name(p, c, e)}
\Signature{(\%, R, E)}{\%}
\Params{
{\em p} & \% & An element of the module (to be destroyed)\\
{\em c} & R & A scalar\\
{\em e} & E & The degree of the term to add\\
}
\Retval{\name(p, c, e) computes the sum $p + c e$.}
\Remarks{The storage used by p is allowed to be destroyed or reused, so p
is lost after this call. This may cause p to be destroyed, so do not use
this unless p has been locally allocated, and is thus guaranteed not to
share space with other polynomials. Some functions, like
\alfunc{FreeModule}{reductum} are
not necessarily copying their arguments and can thus create memory aliases.}
#endif
		coefficient: (%, E) -> R;
		setCoefficient!: (%, E, R) -> %;
#if ALDOC
\alpage{coefficient}
\Usage{\name(p, e)}
\Signature{(\%, E)}{R}
\Params{
{\em p} & \% & An element of the module\\
{\em e} & E & An exponent\\
}
\Retval{Returns the coefficient of $e$ in $p$.}
#endif
		monomial: E -> %;
		term: (R, E) -> %;
#if ALDOC
\alpage{monomial,term}
\altarget{monomial}
\altarget{term}
\Usage{monomial(e)\\ term(r, e)}
\Signatures{
monomial: & E $\to$ \%\\
term: & (R, E)$\to$ \%\\
}
\Params{
{\em r} & R & A scalar\\
{\em e} & E & An exponent\\
}
\Retval{monomial(e) and term(r, e) return respectively
the monomial $e$ and the term $r e$.}
#endif
		default {
			add!(p:%, c:R, v:E):%	== add!(p, term(c, v));
			monomial(e:E):%		== term(1$R, e);
		}
}


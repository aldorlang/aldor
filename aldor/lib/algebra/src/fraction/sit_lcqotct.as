------------------------ sit_lcqotct.as ---------------------------
-- Copyright (c) Manuel Bronstein 2000
-- Copyright (c) INRIA 1999, Version 0.1.12
-- Logiciel Sum^it (c) INRIA 1999, dans sa version 0.1.12
-----------------------------------------------------------------------------

#include "algebra"

macro {
	I == MachineInteger;
	V == Vector;
}

#if ALDOC
\thistype{LinearCombinationFraction}
\History{Manuel Bronstein}{15/2/2000}{created}
\Usage{\this(R, LR, Q, LQ): Category}
\Params{
{\em R} & \altype{IntegralDomain} & an integral domain\\
{\em LR} & \altype{LinearCombinationType} R & a type over R\\
{\em Q} & \altype{FractionCategory} R & a fraction domain of R\\
{\em LQ} & \altype{LinearCombinationType} Q & a type over Q\\
}
\Descr{\this(R, LR, Q, LQ) is a category for domains providing conversions
between types integral and rational coefficients.}
\begin{exports}
\alexp{$*$}: & (Q, LR) $\to$ LQ & Product of a fraction by an integral element\\
\alexp{makeIntegral}: & LQ $\to$ (R, LR) & Conversion to an integral element\\
\alexp{makeRational}: & LR $\to$ LQ & Conversion to a rational element\\
\end{exports}
\begin{exports}[if Q has \altype{FractionByCategory0} R]
\alexp{makeIntegralBy}:
& LQ $\to$ (\altype{Integer}, LR) & Conversion to an integral element\\
\end{exports}
\begin{exports}[if Q has \altype{FractionFieldCategory0} R]
\alexp{normalize}: & LR $\to$ (R, LQ) & Conversion to a monic rational element\\
\end{exports}
#endif

define LinearCombinationFraction(R:IntegralDomain, LR:LinearCombinationType R,
	Q:FractionCategory R, LQ:LinearCombinationType Q): Category == with {
	*: (Q, LR) -> LQ;
#if ALDOC
\alpage{$*$}
\Usage{$c \ast A$}
\Signature{(Q, LR)}{LQ}
\Params{
{\em c} & Q & A fraction\\
{\em A} & LR & An integral element\\
}
\Retval{Returns $c A$ as a rational element.}
#endif
	makeIntegral: LQ -> (R, LR);
#if ALDOC
\alpage{makeIntegral}
\Usage{(a, A) := \name~B}
\Signature{LQ}{(R, LR)}
\Params{ {\em B} & LQ & A rational element\\ }
\Retval{Returns $(a, A)$ such that $A = a B$ is integral.
If R is a \altype{GcdDomain}, then $A$ is primitive.}
#endif
	if Q has FractionByCategory0 R then {
		makeIntegralBy: LQ -> (Integer, LR);
#if ALDOC
\alpage{makeIntegralBy}
\Usage{($\mu$, A) := \name~B}
\Signature{LQ}{(\altype{Integer}, LR)}
\Params{ {\em B} & LQ & A rational element\\ }
\Retval{Returns $(\mu, A)$ such that $A = \mbox{shift}(B, \mu)$ is integral.}
#endif
	}
	makeRational: LR -> LQ;
#if ALDOC
\alpage{makeRational}
\Usage{\name~A}
\Signature{LR}{LQ}
\Params{ {\em A} & LR & An integral element\\ }
\Retval{Returns $A$ as a rational element.}
#endif
	if Q has FractionFieldCategory0 R then {
		normalize: LR -> (R, LQ);
#if ALDOC
\alpage{normalize}
\Usage{(a, B) := \name~A}
\Signature{LR}{(R, LQ)}
\Params{ {\em A} & LR & An integral element\\ }
\Retval{Returns $(a, B)$ such that $A = a B$,
and $B$ is a normalized rational element.}
\Remarks{The normalization depends on the actual type LQ. For polynomial,
it means that the leading coefficient if $1$, for vectors that the first
coordinate is $1$, etc.}
#endif
	}
}

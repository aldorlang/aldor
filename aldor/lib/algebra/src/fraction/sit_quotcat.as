------------------------ quotcat.as ---------------------------
-- Copyright (c) Laurent Bernardin 1994
-- Copyright (c) INRIA 1999, Version 0.1.12
-- Logiciel Sum^it (c) INRIA 1999, dans sa version 0.1.12
-- Copyright (c) Swiss Federal Polytechnic Institute Zurich, 1995
-----------------------------------------------------------------------------

#include "algebra"

#if ALDOC
\thistype{FractionCategory}
\History{Laurent Bernardin}{1/12/94}{created}
\History{Manuel Bronstein}{25/5/95}
{extracted the category from the type in order to create RationalCategory}
\History{Manuel Bronstein}{21/9/95}{added UnivariateGcdRing and gcd lifting}
\History{Manuel Bronstein}{21/3/96}{made it category of subrings}
\Usage{\this~R: Category}
\Params{ {\em R} & \altype{IntegralDomain} & an integral domain\\ }
\Descr{\this~R is the category of subrings of the fraction field of R.}
\begin{exports}
\category{\altype{Algebra} R}\\
\category{\altype{DifferentialExtension} R}\\
\category{\altype{LinearAlgebraRing}}\\
\alexp{denominator}: & \% $\to$ R & Denominator of a fraction\\
\alexp{normalize}: & \% $\to$ \% & Normalize a fraction\\
\alexp{numerator}: & \% $\to$ R & Numerator of a fraction\\
\end{exports}
\begin{exports}[if R has \altype{CharacteristicZero} then]
\category{\altype{CharacteristicZero}}\\
\end{exports}
\begin{exports}[if R has \altype{FactorizationRing} then]
\category{\altype{FactorizationRing}}\\
\end{exports}
\begin{exports}[if R has \altype{FiniteCharacteristic} then]
\category{\altype{FiniteCharacteristic}}\\
\end{exports}
% \begin{exports}[if R has \altype{OrderedRing} then]
% \category{\altype{OrderedRing}}\\
% \end{exports}
\begin{exports}[if R has \altype{RationalRootRing} then]
\category{\altype{RationalRootRing}}\\
\end{exports}
\begin{exports}[if R has \altype{Specializable} then]
\category{\altype{Specializable}}\\
\end{exports}
\begin{exports}[if R has \altype{UnivariateGcdRing} then]
\category{\altype{UnivariateGcdRing}}\\
\end{exports}
#endif

define FractionCategory(R: IntegralDomain): Category ==
	Join(DifferentialExtension R, Algebra R, LinearAlgebraRing) with {
--	if R has OrderedRing then OrderedRing;
	if R has CharacteristicZero then CharacteristicZero;
	if R has FiniteCharacteristic then FiniteCharacteristic;
	if R has Specializable then Specializable;
	if R has UnivariateGcdRing then UnivariateGcdRing;
	if R has RationalRootRing then RationalRootRing;
	if R has FactorizationRing then FactorizationRing;
	denominator:	% -> R;
	numerator:	% -> R;
#if ALDOC
\alpage{denominator,numerator}
\altarget{denominator}
\altarget{numerator}
\Usage{denominator~x\\ numerator~x}
\Signature{\%}{R}
\Params{ {\em x} & \% & A fraction\\ }
\Retval{Returns respectively the denominator and the numerator of a fraction.}
#endif
	normalize:	% -> %;
#if ALDOC
\alpage{normalize}
\Usage{\name~x}
\Signature{\%}{\%}
\Params{ {\em x} & \% & A fraction\\ }
\Descr{Normalize $x$ as much as possible given the category of $R$.}
#endif
	default {
		extree(a:%):ExpressionTree == {
			import from R, List ExpressionTree;
			zero? a => extree(0@R);
			tnum := extree numerator a;
			one? denominator a => tnum;
			tden := extree denominator a;
			-- move the minus sign out of the numerator to the front
			negative? tnum => {
				t := ExpressionTreeQuotient [negate tnum, tden];
				ExpressionTreeMinus [t];
			}
			ExpressionTreeQuotient [tnum, tden];
		}

		if R has Specializable then {

			specialization(Image:CommutativeRing):_
				PartialFunction(%,Image) == {
				import from PartialFunction(R, Image);
				import from Image, Partial Image;
				f := partialMapping(specialization(Image)$R);
				Image has IntegralDomain =>
					partialFunction((x:%):Partial(Image)+->{
						failed?(n := f numerator x) or
						failed?(d := f denominator x) or
						zero?(r := retract d) => failed;
						exactQuotient(retract n, r);
					});
				partialFunction((x:%):Partial(Image)+->{
					failed?(n := f numerator x) or
					failed?(d := f denominator x) or
					failed?(r1 := reciprocal retract d) =>
									failed;
					[retract(n) * retract(r1)];
				});
			}
		}
	}
}		

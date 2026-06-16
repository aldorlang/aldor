------------------------------ sit_mkpring.as ------------------------------
--
-- Turns any ring into a partial ring
--
-- Copyright (c) Manuel Bronstein 1998
-- Copyright (c) INRIA 1999, Version 0.1.12
-- Logiciel Sum^it ©INRIA 1999, dans sa version 0.1.12
-----------------------------------------------------------------------------

#include "algebra"

#if ALDOC
\thistype{MakePartialRing}
\History{Manuel Bronstein}{15/04/98}{created}
\Usage{import from \this(R, f)}
\Params{
R & \astype{Ring} & A ring\\
f & R $\to$ \astype{Partial} \astype{Integer} & A retraction to the integers\\
}
\Descr{\this(R, f) is a partial ring isomorphic to R. The function $f$
is used to retract elements of this partial ring to integers whenever
possible. This type can be used in order to build an evaluator that
interprets expression trees into R.}
\begin{exports}
\category{\astype{PartialRing}}\\
\asexp{coerce}:
& R $\to$ \% & Convert an element of R to one of the partial ring\\
\asexp{coerce}:
& \% $\to$ R & Convert an element of the partial ring to one of R\\
\end{exports}
#endif

MakePartialRing(R:Ring, int:R -> Partial Integer): PartialRing with {
	coerce: R -> %;
	coerce: % -> R;
} == add {
	Rep == R;
	macro U == Partial %;
	import from Rep;

	local field?:Boolean		== R has Field;
	local comring?:Boolean		== R has CommutativeRing;
	0:%				== 0$R :: %;
	1:%				== 1$R :: %;
	coerce(n:Integer):%		== n::R::%;
	coerce(p:R):%			== per p;
	coerce(a:%):R			== rep a;
	- (b:%):U			== [(-(b::R))::%];
	(a:%) + (b:%):U			== [(a::R + b::R)::%];
	(a:%) - (b:%):U			== [(a::R - b::R)::%];
	(a:%) * (b:%):U			== [(a::R * b::R)::%];
	(a:%) = (b:%):Boolean		== a::R = b::R;
	integer(a:%):Partial Integer	== int(a::R);
	extree(a:%):ExpressionTree	== extree(a::R);

	if R has Field then {
		import from R;
		(a:%) / (b:%):U	== [(a::R / b::R)::%];
	}
	else if R has CommutativeRing then {
		(a:%) / (b:%):U	== {
			import from R, Partial R;
			failed?(u := exactQuotient(a::R, b::R)) => failed;
			[retract(u)::%];
		}
	}
	else {
		(a:%) / (b:%):U	== {
			one?(b::R) => [a];
			failed;
		}
	}

	if R has TotallyOrderedType then {
		(a:%) < (b:%):U == {
			import from R;
			[(a::R < b::R)::%];
		}
	}

	(a:%) ^ (b:%):U	== {
		import from Partial Integer;
		failed?(u := integer b) => failed;
		[((a::R)^retract(u))::%];
	}
}

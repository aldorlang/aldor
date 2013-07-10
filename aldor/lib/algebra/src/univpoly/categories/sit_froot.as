----------------------------- sit_froot.as ----------------------------------
-- Copyright (c) Manuel Bronstein 1996
-- Copyright (c) INRIA 1999, Version 0.1.12
-- Logiciel Sum^it (c) INRIA 1999, dans sa version 0.1.12
-- Copyright (c) Swiss Federal Polytechnic Institute Zurich, 1996-97
-----------------------------------------------------------------------------

#include "algebra"

macro {
	Z  == Integer;
	RR == FractionalRoot Z;
	GEN== Generator;
}

#if ALDOC
\thistype{FractionalRoot}
\History{Manuel Bronstein}{11/12/96}{created RationalRoot}
\History{Manuel Bronstein}{17/8/2000}{added the parameter R, changed type name}
\Usage{import from \this}
\Descr{\this(R) provides fractions of R with multiplicities.}
\Params{ {\em R} & \altype{CommutativeRing} & A ring\\ }
\begin{exports}
\category{\altype{ExpressionType}}\\
\alexp{fractionalRoot}: & (R, R, \altype{Integer}) $\to$ \% & Create a root\\
\alexp{integral?}:
& \% $\to$ \altype{Boolean} & Test whether root is integral\\
\alexp{integralRoot}: & (R, \altype{Integer}) $\to$ \% & Create a root\\
\alexp{integralValue}: & \% $\to$ R & Value of an integral root\\
\alexp{multiplicity}: & \% $\to$ \altype{Integer} & Multiplicity of a root\\
\alexp{setMultiplicity!}:
& (\%, \altype{Integer}) $\to$ \% & Change a multiplicity\\
\alexp{value}: &  \% $\to$ (R, R) & Value of a root\\
\end{exports}
#endif

FractionalRoot(R:CommutativeRing): ExpressionType with {
	integral?: % -> Boolean;
#if ALDOC
\alpage{integral?}
\Signature{\%}{\altype{Boolean}}
\Usage{\name~r}
\Params{ {\em r} & \% & A root\\ }
\Retval{Return \true~if r is in R, \false~otherwise.}
#endif
	integralRoot: (R, Z) -> %;
	fractionalRoot: (R, R, Z) -> %;
#if ALDOC
\alpage{fractionalRoot,integralRoot}
\altarget{fractionalRoot}
\altarget{integralRoot}
\Usage{fractionalRoot(a, b, n)\\ integralRoot(a, n)}
\Signatures{
fractionalRoot: & (R, R, \altype{Integer}) $\to$ \%\\
integralRoot: & (R, \altype{Integer}) $\to$ \%\\
}
\Params{
{\em a} & R & A numerator\\
{\em b} & R & A denominator\\
{\em n} & \altype{Integer} & A multiplicity\\
}
\Retval{Return the root $a$ or $a/b$ with multiplicity $n$.}
#endif
	integralValue: % -> R;
#if ALDOC
\alpage{integralValue}
\Usage{ \name~r }
\Signature{\%}{\altype{Integer}}
\Params{ {\em r} & \% & A root\\ }
\Retval{Returns the value of the integral root $r$, ignoring its multiplicity.}
\alseealso{\alexp{value}}
#endif
	multiplicity: % -> Z;
#if ALDOC
\alpage{multiplicity}
\Usage{ \name~r }
\Signature{\%}{\altype{Integer}}
\Params{ {\em r} & \% & A root\\ }
\Retval{Return the multiplicity of $r$.}
#endif
	setMultiplicity!: (%, Z) -> %;
#if ALDOC
\alpage{setMultiplicity!}
\Usage{ \name(r, m) }
\Signature{(\%, \altype{Integer})}{\altype{Integer}}
\Params{
{\em r} & \% & A root\\
{\em m} & \altype{Integer} & Its new multiplicity\\
}
\Descr{Sets the multiplicity of $r$ to $m$ and returns $r$.}
#endif
	value: % -> (R, R);
#if ALDOC
\alpage{value}
\Usage{ (n, d) := \name~r }
\Signature{\%}{(\altype{Integer}, \altype{Integer})}
\Params{ {\em r} & \% & A root\\ }
\Retval{Return $(n, d)$ such that the value of $r$ is $n/d$.}
\alseealso{\alexp{integralValue}}
#endif
} == add {
	Rep == Record(num:R, den:R, mult:Z);

	import from Z, R, Rep;

	value(r:%):(R, R)		== (numerator r, denominator r);
	integralValue(r:%):R		== { assert(integral? r); numerator r }
	multiplicity(r:%):Z		== rep(r).mult;
	integralRoot(n:R, e:Z):%	== { assert(e > 0); per [n, 1, e] }
	integral?(r:%):Boolean		== one? denominator r;
	local numerator(r:%):R		== rep(r).num;
	local denominator(r:%):R	== rep(r).den;

	if R has Field then {
		fractionalRoot(n:R, d:R, e:Z):% == {
			import from Boolean;
			assert(~zero? d); assert(e > 0);
			per [n/d, 1, e];
		}
	}
	else if R has GcdDomain then {
		fractionalRoot(n:R, d:R, e:Z):% == {
			import from Boolean;
			assert(~zero? d); assert(e > 0);
			zero? n => per [0, 1, e];
			(g, nn, dd) := gcdquo(n, d);
			unit? dd => per [quotient(nn, dd), 1, e];
			per [nn, dd, e];
		}
	}
	else {
		fractionalRoot(n:R, d:R, e:Z):% == {
			import from Boolean, Partial R;
			assert(~zero? d); assert(e > 0);
			zero? n => per [0, 1, e];
			failed?(u := exactQuotient(n, d)) => per [n, d, e];
			per [retract u, 1, e];
		}
	}

	(a:%) = (b:%):Boolean ==
		numerator(a) = numerator(b) and
			denominator(a) = denominator(b) and
				multiplicity(a) = multiplicity(b);

	extree(r:%):ExpressionTree == {
		import from List ExpressionTree;
		ExpressionTreeList [extree value r, extree multiplicity r];
	}

	local extree(n:R, d:R):ExpressionTree == {
		import from List ExpressionTree;
		tnum := extree n;
		one? d => tnum;
		ExpressionTreeQuotient [tnum, extree d];
	}

	setMultiplicity!(r:%, e:Z):% == {
		assert(e > 0);
		rep(r).mult := e;
		r;
	}
}

